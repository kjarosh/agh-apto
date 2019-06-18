#include "BoardSolver.h"

#include "exceptions.h"

solution_t BoardSolver::solve(Board *board) {
    BoardGraph graph(board);
#ifdef DEBUG
    graph.print();
#endif
    return graph.search();
}

BoardGraph::BoardGraph(Board *board) :
        board(board),
        adjacency(board->get_size()),
        adjacency_transpose(board->get_size()),
        scc_leader_mapping(board->get_size()),
        scc_adjacency(board->get_size()),
        scc_diamonds(board->get_size()) {
    std::set<idx_t> filled = {};
    fill_graph(board->get_ball_index(), filled);
    fill_transpose();

    std::set<idx_t> visiting;
    std::vector<idx_t> visited_stack;
    dfs_visit(board->get_ball_index(), visiting, visited_stack);
    build_scc(visited_stack);
    build_scc_graph();
}

BoardGraph::~BoardGraph() {

}

void BoardGraph::fill_graph(idx_t ix, std::set<idx_t> &filled) {
    filled.insert(ix);

    for (auto &dir : all_directions) {
        Move move = board->move(ix, dir);

        if (move.position_equals(ix)) {
            // no move
            continue;
        }

        idx_t next_ix = move.to;
        adjacency[ix].push_back(move);

        if (filled.find(next_ix) == filled.end()) {
            fill_graph(move.to, filled);
        }
    }
}

void BoardGraph::fill_transpose() {
    for (idx_t ix = 0; ix < adjacency.size(); ++ix) {
        for (auto &move : adjacency[ix]) {
            idx_t ix_to = move.to;
            adjacency_transpose[ix_to].push_back(ix);
        }
    }
}

void BoardGraph::print() const {
    size_t i = 0;
    for (auto &a : adjacency) {
        size_t x, y;
        std::tie(x, y) = board->from_index(i++);
        std::cout << x << ',' << y << ": [";
        for (auto &a2 : a) {
            size_t x2, y2;
            std::tie(x2, y2) = board->from_index(a2.to);
            std::cout << '(' << a2.direction << ": " << x2 << ',' << y2 << '/' << a2.diamonds.size() << "); ";
        }
        std::cout << ']' << '\n';
    }

    std::cout << "SCC leaders:" << '\n';
    for (auto &leader : scc_leaders) {
        size_t x, y;
        std::tie(x, y) = board->from_index(leader);
        std::cout << x << ',' << y << "; ";
    }
    std::cout << '\n';

    std::cout << "SCC diamonds:" << '\n';
    size_t j = 0;
    for (auto &d : scc_diamonds) {
        if (scc_leaders.find(j) == scc_leaders.end()) {
            ++j;
            continue;
        }
        size_t x, y;
        std::tie(x, y) = board->from_index(j++);
        std::cout << x << ',' << y << ": " << d.size() << '\n';
    }

    std::cout << "SCC adjacency:" << '\n';
    size_t i2 = 0;
    for (auto &neighs : scc_adjacency) {
        if (scc_leaders.find(i2) == scc_leaders.end()) {
            ++i2;
            continue;
        }

        size_t x, y;
        std::tie(x, y) = board->from_index(i2++);
        std::cout << x << ',' << y << ": [";
        for (const SCCMove &move : neighs) {
            size_t x2, y2;
            std::tie(x2, y2) = board->from_index(move.to_leader);
            std::cout << x2 << ',' << y2 << '/' << move.diamonds.size() << "; ";
        }
        std::cout << ']' << '\n';
    }
    std::cout << '\n';
}

solution_t BoardGraph::search() {
    std::vector<std::unordered_set<GameState>> computed_states(board->get_width() * board->get_height());
    GameState initial = GameState::initial(board);
    return search0(computed_states, initial);
}

solution_t BoardGraph::search0(std::vector<std::unordered_set<GameState>> &computed_states, const GameState &state) {
    idx_t ix = state.position;

    if (state.is_finished()) {
        return state.moves;
    }

    if (state.moves.size() >= board->get_max_moves()) {
        return {};
    }

    std::unordered_set<GameState> &current_computed_states = computed_states[ix];
    for (auto &st : current_computed_states) {
        if (st.compare(state) > 0) {
            // there was already a better solution
            return {};
        }
    }

    computed_states[ix].insert(state);

    std::vector<solution_t> solutions;

    for (auto &move : adjacency[ix]) {
        solution_t ret = search0(computed_states, state.next(move));
        if (ret.empty()) continue;

        solutions.push_back(ret);
    }

    solution_t best = {};
    size_t best_size = SIZE_MAX;
    for (auto &s : solutions) {
        if (s.size() < best_size) {
            best_size = s.size();
            best = s;
        }
    }

    return best;
}

void BoardGraph::dfs_visit(idx_t position, std::set<idx_t> &visiting, std::vector<idx_t> &visited_stack) const {
    visiting.insert(position);

    for (const Move &move : adjacency[position]) {
        idx_t neigh = move.to;
        if (visiting.find(neigh) == visiting.end()) {
            dfs_visit(neigh, visiting, visited_stack);
        }
    }

    visited_stack.push_back(position);
}

void BoardGraph::build_scc(std::vector<idx_t> &visited_stack) {
    std::set<idx_t> visiting;
    for (auto i = visited_stack.rbegin(); i != visited_stack.rend(); ++i) {
        idx_t ix = *i;
        if (visiting.find(ix) != visiting.end()) {
            continue;
        }

        scc_leaders.insert(ix);

        dfs_visit_transpose(ix, ix, visiting);
    }
}

void BoardGraph::dfs_visit_transpose(idx_t leader, idx_t ix, std::set<idx_t> &visiting) {
    visiting.insert(ix);
    scc_leader_mapping[ix] = leader;

    for (idx_t &neigh : adjacency_transpose[ix]) {
        if (visiting.find(neigh) == visiting.end()) {
            dfs_visit_transpose(leader, neigh, visiting);
        }
    }
}

void BoardGraph::build_scc_graph() {
    size_t ix = 0;
    for (auto &neighbors : adjacency) {
        idx_t ix_leader = scc_leader_mapping[ix++];

        for (auto &move : neighbors) {
            idx_t ix_to_leader = scc_leader_mapping[move.to];

            if (ix_leader == ix_to_leader) {
                // internal diamonds
                for (auto &diamond : move.diamonds) {
                    scc_diamonds[ix_leader].insert(diamond);
                }

                continue;
            }

            SCCMove current_move = SCCMove::create(ix_to_leader, move.diamonds);

            std::list<SCCMove> &scc_moves = scc_adjacency[ix_leader];
            auto begin = scc_moves.begin();
            auto end = scc_moves.end();
            bool insert = true;
            for (auto i = begin; i != end; ++i) {
                const SCCMove &scc_move = *i;
                if (scc_move.to_leader != ix_to_leader) continue;

                if (scc_move.diamonds.empty()) {
                    if (!current_move.diamonds.empty()) {
                        // remove all moves without diamonds because we have diamonds
                        scc_moves.erase(i);
                    } else {
                        // if there's another move without diamonds it means that
                        // all subsequent moves don't have diamonds
                        scc_moves.insert(i, current_move);
                        insert = false;
                        break;
                    }
                } else {
                    if (!current_move.diamonds.empty()) {
                        // two moves with diamonds
                        throw no_solution_exception();
                    } else {
                        // ignore, there's aready a move with diamonds
                        insert = false;
                        break;
                    }
                }
            }

            if (insert) {
                scc_moves.insert(end, current_move);
            }
        }
    }
}
