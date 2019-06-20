#include "BoardSolver.h"

#include "exceptions.h"

solution_t BoardSolver::solve(Board *board) {
    BoardGraph graph(board);
#ifdef DEBUG
    graph.print();
#endif
    return graph.bfs();
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


    /*fill_transpose();
    std::set<idx_t> visiting;
    std::vector<idx_t> visited_stack;
    dfs_visit(board->get_ball_index(), visiting, visited_stack);
    build_scc(visited_stack);
    build_scc_graph();*/
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

        std::cout << j++ << ": " << d.size() << '\n';
    }

    std::cout << "SCC adjacency:" << '\n';
    size_t i2 = 0;
    for (auto &neighs : scc_adjacency) {
        if (scc_leaders.find(i2) == scc_leaders.end()) {
            ++i2;
            continue;
        }

        std::cout << i2++ << ": [";
        for (const SCCMove &move : neighs) {
            std::cout << move.to_leader << '/' << move.diamonds.size() << "; ";
        }
        std::cout << ']' << '\n';
    }
    std::cout << '\n';
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

solution_t BoardGraph::solve_using_scc() {
    std::vector<idx_t> scc_leaders_path;
    const idx_t first_leader = *scc_leaders.rbegin();
    std::set<idx_t> gathered_diamonds = scc_find_path(scc_leaders_path, first_leader);
    if (gathered_diamonds != board->get_diamond_positions()) {
        // it's impossible to traverse scc-s and gather all diamonds
        throw no_solution_exception();
    }

#ifdef DEBUG
    std::cout << "Required SCC path:\n";
    for (auto &ix : scc_leaders_path) {
        std::cout << ix << " ";
    }
    std::cout << '\n';
#endif

    std::vector<SCCSolution> final_solutions;
    final_solutions.push_back({{}, board->get_ball_index()});
    idx_t current_leader = first_leader;
    for (auto &next_leader : scc_leaders_path) {
        if (next_leader == current_leader) continue;
        std::vector<SCCSolution> old_final_solutions = std::move(final_solutions);
        final_solutions = {};

        for (auto old_final_solution : old_final_solutions) {
            std::vector<SCCSolution> partial_solutions = search_within_scc(
                    old_final_solution.final_position, current_leader, next_leader);

            for (auto &partial_solution : partial_solutions) {
                old_final_solution.moves.insert(
                        old_final_solution.moves.end(),
                        partial_solution.moves.begin(), partial_solution.moves.end());
                old_final_solution.final_position = partial_solution.final_position;
                final_solutions.push_back(old_final_solution);
            }
        }

        current_leader = next_leader;
    }

    std::vector<SCCSolution> old_final_solutions = std::move(final_solutions);
    final_solutions = {};

    for (auto old_final_solution : old_final_solutions) {
        std::vector<SCCSolution> partial_solutions = search_final_within_scc(
                old_final_solution.final_position, current_leader);

        for (auto &partial_solution : partial_solutions) {
            old_final_solution.moves.insert(
                    old_final_solution.moves.end(),
                    partial_solution.moves.begin(), partial_solution.moves.end());
            old_final_solution.final_position = partial_solution.final_position;
            final_solutions.push_back(old_final_solution);
        }
    }

    size_t best_size = SIZE_MAX;
    SCCSolution best_solution;
    for (auto &solution : final_solutions) {
        if (solution.moves.size() < best_size) {
            best_size = solution.moves.size();
            best_solution = solution;
        }
    }
    return best_solution.moves;
}

std::set<idx_t> BoardGraph::scc_find_path(std::vector<idx_t> &scc_leaders_path, idx_t leader) {
    std::set<idx_t> diamonds = scc_diamonds[leader];

    scc_leaders_path.push_back(leader);
    std::list<SCCMove> &neighbors = scc_adjacency[leader];
    if (neighbors.empty()) {
        return diamonds;
    }

    bool has_path_with_diamonds = false;
    idx_t path_with_diamonds = 0;
    for (auto &neighbor : neighbors) {
        if (!neighbor.diamonds.empty()) {
            if (has_path_with_diamonds) {
                // two paths with diamonds
                throw no_solution_exception();
            }

            has_path_with_diamonds = true;
            path_with_diamonds = neighbor.to_leader;
        }
    }

    if (has_path_with_diamonds) {
        // the only one path with diamonds
        scc_leaders_path.push_back(path_with_diamonds);
        std::set<idx_t> diamonds_rest = scc_find_path(scc_leaders_path, path_with_diamonds);
        diamonds.insert(diamonds_rest.begin(), diamonds_rest.end());
        return diamonds;
    } else {
        // doesn't have a path with diamonds

        std::vector<idx_t> scc_leaders_path_rest = scc_leaders_path;
        std::set<idx_t> diamonds_rest = {};

        for (auto &neighbor : neighbors) {
            std::vector<idx_t> scc_leaders_path_rest2 = scc_leaders_path;
            std::set<idx_t> diamonds_rest2 = scc_find_path(scc_leaders_path_rest2, neighbor.to_leader);

            if (diamonds_rest.empty()) {
                // everything's better than nothing
                diamonds_rest = diamonds_rest2;
                scc_leaders_path_rest = scc_leaders_path_rest2;
                continue;
            }

            if (diamonds_rest2.empty()) {
                // nothing's not better than anything
                continue;
            }

            // this checking is not the fastest one but it doesn't need to be I hope
            bool rest2_contains_rest = std::all_of(
                    diamonds_rest.begin(), diamonds_rest.end(),
                    [&diamonds_rest2](idx_t ix) { return diamonds_rest2.find(ix) != diamonds_rest2.end(); });
            bool rest_contains_rest2 = std::all_of(
                    diamonds_rest2.begin(), diamonds_rest2.end(),
                    [&diamonds_rest](idx_t ix) { return diamonds_rest.find(ix) != diamonds_rest.end(); });

            if (rest2_contains_rest && !rest_contains_rest2) {
                diamonds_rest = diamonds_rest2;
                scc_leaders_path_rest = scc_leaders_path_rest2;
            } else if (rest_contains_rest2 && !rest2_contains_rest) {
                // do nothing
            } else {
                throw no_solution_exception();
            }
        }

        scc_leaders_path = scc_leaders_path_rest;
        diamonds.insert(diamonds_rest.begin(), diamonds_rest.end());
        return diamonds;
    }
}

std::vector<SCCSolution> BoardGraph::search_within_scc(idx_t from, idx_t leader, idx_t target_leader) {
    GameState initial = GameState::from(from);
    return search_within_scc0(initial, leader, target_leader);
}

std::vector<SCCSolution> BoardGraph::search_final_within_scc(idx_t from, idx_t leader) {
    GameState initial = GameState::from(from);
    return search_within_scc0(initial, leader, 0);
}

std::vector<SCCSolution> BoardGraph::search_within_scc0(
        const GameState &original_state, idx_t leader, idx_t target_leader) {
    std::queue<GameState> state_queue;
    std::vector<std::vector<GameState>> computed_states(board->get_size());

    std::set<idx_t> solved_targets;
    std::vector<SCCSolution> solutions;

    state_queue.push(original_state);

    while (!state_queue.empty()) {
        const GameState current = state_queue.front();
        state_queue.pop();

        if (current.gathered_diamonds == scc_diamonds[leader]) {
            if (target_leader == 0 || scc_leader_mapping[current.position] == target_leader) {
                if (solved_targets.find(current.position) == solved_targets.end()) {
                    solved_targets.insert(current.position);
                    solutions.push_back({current.moves, current.position});
                }
                continue;
            }
        }

        if (scc_leader_mapping[current.position] != leader) {
            // outside of the scc
            continue;
        }

        if (current.moves.size() >= board->get_max_moves()) {
            continue;
        }

        bool ignore = false;
        for (auto &computed_state : computed_states[current.position]) {
            if (current.is_worse_than(computed_state)) {
                ignore = true;
                break;
            }
        }

        if (ignore) continue;

        computed_states[current.position].push_back(current);

        for (auto &move : adjacency[current.position]) {
            state_queue.push(current.next(move));
        }
    }

    return solutions;
}

solution_t BoardGraph::bfs() {
    std::priority_queue<GameState, std::vector<GameState>, GameStatePriority> state_queue;
    std::vector<std::vector<GameState>> computed_states(board->get_size());

    state_queue.push(GameState::initial(board));

    while (!state_queue.empty()) {
        const GameState current = state_queue.top();
        state_queue.pop();

        if (board->get_diamond_positions() == current.gathered_diamonds) {
            return current.moves;
        }

        if (current.moves.size() >= board->get_max_moves()) {
            continue;
        }

        bool ignore = false;
        for (auto &computed_state : computed_states[current.position]) {
            if (current.is_worse_than(computed_state)) {
                ignore = true;
                break;
            }
        }

        if (ignore) continue;

        computed_states[current.position].push_back(current);

        for (auto &move : adjacency[current.position]) {
            state_queue.push(current.next(move));
        }
    }

    throw no_solution_exception();
}
