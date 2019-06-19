#ifndef DIAMINY_GAMESTATE_H
#define DIAMINY_GAMESTATE_H

struct GameState {
    const Board *board;
    const std::set<idx_t> gathered_diamonds;
    const std::vector<Direction> moves;
    const idx_t position;

    inline GameState next(Move &move) const {
        std::vector<Direction> next_moves = moves;
        next_moves.push_back(move.direction);

        std::set<idx_t> next_diamonds = gathered_diamonds;
        std::copy(move.diamonds.begin(), move.diamonds.end(),
                  std::inserter(next_diamonds, next_diamonds.end()));

        return (GameState) {
                board,
                std::move(next_diamonds),
                std::move(next_moves),
                move.to
        };
    }

    int compare(const GameState &other) const;

    bool is_worse_than(const GameState &state) const;

    inline static GameState initial(const Board *board) {
        return {board, {}, {}, board->get_ball_index()};
    }

    inline static GameState from(const Board *board, const idx_t from) {
        return {board, {}, {}, from};
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
            if ((*i1) != (*i2)) {
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
                seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };
}

#endif
