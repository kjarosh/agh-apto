#include "BoardSolver.h"

#include "exceptions.h"

solution_t BoardSolver::solve(Board *board) {
    BoardGraph graph(board);
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
}

solution_t BoardGraph::bfs() {
    std::priority_queue<GameState, std::vector<GameState>, GameStatePriority> state_queue;
    std::vector<std::vector<GameState>> computed_states(board->get_size());

    state_queue.push(GameState::initial(board));

    while (!state_queue.empty()) {
        const GameState current = state_queue.top();
        state_queue.pop();

        if (board->get_diamond_positions() == current.get_gathered_diamonds()) {
            return current.moves;
        }

        if (current.moves.size() >= board->get_max_moves()) {
            continue;
        }

        bool ignore = false;
        auto &curr_computed_states = computed_states[current.position];
        for (auto i = curr_computed_states.rbegin(), end = curr_computed_states.rend(); i != end; ++i) {
            if (current.is_worse_than(*i)) {
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
