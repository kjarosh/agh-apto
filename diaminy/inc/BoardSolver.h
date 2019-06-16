#ifndef DIAMINY_BOARDSOLVER_H
#define DIAMINY_BOARDSOLVER_H

#include "Board.h"

typedef std::vector<Move> solution_t;

struct GameState {
    const Board *board;
    std::set<size_t> gathered_diamonds;
    std::vector<Move> moves;
    dim_t x, y;

    inline GameState next(Move &move) const {
        GameState ret;
        ret.gathered_diamonds = gathered_diamonds;
        std::copy(move.diamonds.begin(), move.diamonds.end(),
                  std::inserter(ret.gathered_diamonds, ret.gathered_diamonds.end()));
        ret.moves = moves;
        ret.moves.push_back(move);
        ret.x = move.to_x;
        ret.y = move.to_y;
        ret.board = board;
        return ret;
    }

    inline int compare(const GameState &other) const {
        const std::set<size_t> &mine = gathered_diamonds;
        const std::set<size_t> &theirs = other.gathered_diamonds;
        auto mine_end = mine.end();
        auto theirs_end = theirs.end();
        bool i_have_more = false;
        bool they_have_more = false;

        auto my_iter = mine.begin();
        auto their_iter = theirs.begin();

        while (my_iter != mine_end && their_iter != theirs_end) {
            size_t my_val = *my_iter;
            size_t their_val = *my_iter;
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

        if (my_iter == mine_end && their_iter == theirs_end) {
            if (!i_have_more && !they_have_more) {
                return other.moves.size() - moves.size();
            }
        } else if (my_iter == mine_end && their_iter != theirs_end) {
            they_have_more = true;
        } else if (my_iter != mine_end && their_iter == theirs_end) {
            i_have_more = true;
        }

        if (i_have_more == they_have_more) {
            return 0;
        }

        if (they_have_more && other.moves.size() < moves.size()) {
            return -1;
        }

        if (i_have_more && other.moves.size() > moves.size()) {
            return 1;
        }

        return 0;
    }

    inline static GameState initial(const Board *board) {
        return {board, {}, {}, board->get_ball_x(), board->get_ball_y()};
    }

    inline bool is_finished() const {
        return board->get_diamond_positions() == gathered_diamonds;
    }

    inline bool operator==(const GameState &g) const {
        auto i1 = moves.begin();
        auto e1 = moves.end();
        auto i2 = g.moves.begin();
        auto e2 = g.moves.end();
        while (i1 != e1 && i2 != e2) {
            if ((*i1).direction != (*i2).direction) {
                return false;
            }

            ++i1;
            ++i2;
        }
        return i1 == e1 && i2 == e2;
    }
};

namespace std {
    template<>
    struct hash<GameState> {
        std::size_t operator()(const GameState &g) const {
            std::size_t seed = g.moves.size();
            for (auto &i : g.moves) {
                seed ^= i.direction + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}

class BoardGraph {
private:
    std::vector<std::vector<Move>> adjacency;

    Board *board;

    void fill_graph(dim_t x, dim_t y, std::set<size_t> &filled);

    solution_t search0(std::vector<std::unordered_set<GameState>> &computed_states, const GameState &state);

public:
    explicit BoardGraph(Board *board);

    void print();

    solution_t search();
};

class BoardSolver {
public:
    static solution_t solve(Board *board);
};

#endif
