#ifndef DIAMINY_GAMESTATE_H
#define DIAMINY_GAMESTATE_H

struct GameState {
    std::shared_ptr<std::set<idx_t>> gathered_diamonds;
    std::vector<Direction> moves;
    idx_t position;

    inline std::set<idx_t> &get_gathered_diamonds() const {
        return *gathered_diamonds;
    }

    inline GameState next(const Move &move) const {
        std::vector<Direction> next_moves = moves;
        next_moves.push_back(move.direction);

        std::shared_ptr<std::set<idx_t>> next_diamonds = gathered_diamonds;
        if (move.diamonds.empty()) {
            next_diamonds = gathered_diamonds;
        } else {
            std::set<idx_t> gathered_diamonds_copy = *gathered_diamonds;
            std::copy(move.diamonds.begin(), move.diamonds.end(),
                      std::inserter(gathered_diamonds_copy, gathered_diamonds_copy.begin()));
            next_diamonds = std::make_shared<std::set<idx_t>>(std::move(gathered_diamonds_copy));
        }

        return (GameState) {
                std::move(next_diamonds),
                std::move(next_moves),
                move.to
        };
    }

    bool is_worse_than(const GameState &state) const;

    inline static GameState initial(const Board *board) {
        return {std::make_shared<std::set<idx_t>>(), {}, board->get_ball_index()};
    }

    inline static GameState from(const idx_t from) {
        return {std::make_shared<std::set<idx_t>>(), {}, from};
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
        return g.get_gathered_diamonds().size();
    }

public:
    inline bool operator()(const GameState &a, const GameState &b) {
        return priority(a) < priority(b);
    }
};

#endif
