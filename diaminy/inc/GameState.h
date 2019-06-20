#ifndef DIAMINY_GAMESTATE_H
#define DIAMINY_GAMESTATE_H

struct GameState {
    std::set<idx_t> gathered_diamonds;
    std::vector<Direction> moves;
    idx_t position;

    inline GameState next(Move &move) const {
        std::vector<Direction> next_moves = moves;
        next_moves.push_back(move.direction);

        std::set<idx_t> next_diamonds = gathered_diamonds;
        std::copy(move.diamonds.begin(), move.diamonds.end(),
                  std::inserter(next_diamonds, next_diamonds.begin()));

        return (GameState) {
                std::move(next_diamonds),
                std::move(next_moves),
                move.to
        };
    }

    int compare(const GameState &other) const;

    bool is_worse_than(const GameState &state) const;

    inline static GameState initial(const Board *board) {
        return {{}, {}, board->get_ball_index()};
    }

    inline static GameState from(const idx_t from) {
        return {{}, {}, from};
    }

    inline bool operator==(const GameState &g) const {
        auto i1 = moves.begin();
        auto e1 = moves.end();
        auto i2 = g.moves.begin();
        auto e2 = g.moves.end();
        while (i1 != e1 && i2 != e2) {
            if ((*i1) != (*i2)) {
                return false;
            }

            ++i1;
            ++i2;
        }
        return i1 == e1 && i2 == e2;
    }
};

class GameStatePriority {
    inline double priority(const GameState &g) {
        return g.gathered_diamonds.size();
    }

public:
    inline bool operator()(const GameState &a, const GameState &b) {
        return priority(a) < priority(b);
    }
};

#endif
