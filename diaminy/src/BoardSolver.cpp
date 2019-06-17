#include "BoardSolver.h"

solution_t BoardSolver::solve(Board *board) {
    BoardGraph graph(board);
    return graph.search();
}

BoardGraph::BoardGraph(Board *board) :
        board(board),
        adjacency(board->get_width() * board->get_height()) {
    std::set<idx_t> filled = {};
    fill_graph(board->get_ball_x(), board->get_ball_y(), filled);
}

void BoardGraph::fill_graph(dim_t x, dim_t y, std::set<idx_t> &filled) {
    idx_t ix = board->to_index(x, y);
    filled.insert(ix);

    for (auto &dir : all_directions) {
        Move move = board->move(x, y, dir);

        if (move.position_equals(x, y)) {
            // no move
            continue;
        }

        idx_t next_ix = board->to_index(move.to_x, move.to_y);
        adjacency[ix].push_back(move);

        if (filled.find(next_ix) == filled.end()) {
            fill_graph(move.to_x, move.to_y, filled);
        }
    }
}

void BoardGraph::print() {
    size_t i = 0;
    for (auto &a : adjacency) {
        dim_t x, y;
        std::tie(x, y) = board->from_index(i++);
        std::cout << x << ',' << y << ": [";
        for (auto &a2 : a) {
            std::cout << '(' << a2.direction << ": " << a2.to_x << ',' << a2.to_y << '/' << a2.diamonds.size() << "); ";
        }
        std::cout << ']' << '\n';
    }
}

solution_t BoardGraph::search() {
    std::vector<std::unordered_set<GameState>> computed_states(board->get_width() * board->get_height());
    GameState initial = GameState::initial(board);
    return search0(computed_states, initial);
}

solution_t BoardGraph::search0(std::vector<std::unordered_set<GameState>> &computed_states, const GameState &state) {
    idx_t ix = board->to_index(state.x, state.y);

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

int GameState::compare(const GameState &other) const {
    const std::set<idx_t> &mine = gathered_diamonds;
    const std::set<idx_t> &theirs = other.gathered_diamonds;
    auto mine_end = mine.end();
    auto theirs_end = theirs.end();
    bool i_have_more = false;
    bool they_have_more = false;

    auto my_iter = mine.begin();
    auto their_iter = theirs.begin();

    while (my_iter != mine_end && their_iter != theirs_end) {
        idx_t my_val = *my_iter;
        idx_t their_val = *my_iter;
        if (my_val > their_val) {
            they_have_more = true;
            ++their_iter;
        } else if (my_val < their_val) {
            i_have_more = true;
            ++my_iter;
        } else {
            ++their_iter;
            ++my_iter;
        }
    }

    size_t other_moves_size = other.moves.size();
    size_t moves_size = moves.size();
    if (my_iter == mine_end && their_iter == theirs_end) {
        if (!i_have_more && !they_have_more) {
            return other_moves_size - moves_size;
        }
    } else if (my_iter == mine_end && their_iter != theirs_end) {
        they_have_more = true;
    } else if (my_iter != mine_end && their_iter == theirs_end) {
        i_have_more = true;
    }

    if (i_have_more == they_have_more) {
        return 0;
    }

    if (they_have_more && other_moves_size < moves_size) {
        return -1;
    }

    if (i_have_more && other_moves_size > moves_size) {
        return 1;
    }

    return 0;
}
