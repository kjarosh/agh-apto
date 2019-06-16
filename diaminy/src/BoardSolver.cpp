#include "BoardSolver.h"

solution_t BoardSolver::solve(Board *board) {
    BoardGraph graph(board);
    return graph.search();
}

BoardGraph::BoardGraph(Board *board) :
        board(board),
        adjacency(board->get_width() * board->get_height()) {
    std::set<size_t> filled = {};
    fill_graph(board->get_ball_x(), board->get_ball_y(), filled);
}

void BoardGraph::fill_graph(dim_t x, dim_t y, std::set<size_t> &filled) {
    size_t ix = board->to_index(x, y);
    filled.insert(ix);

    for (auto &dir : all_directions) {
        Move move = board->move(x, y, dir);

        if (!move.is_valid()) {
            // no move
            continue;
        }

        size_t next_ix = board->to_index(move.to_x, move.to_y);
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
    size_t ix = board->to_index(state.x, state.y);

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
