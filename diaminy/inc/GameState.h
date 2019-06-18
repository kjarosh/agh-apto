#ifndef DIAMINY_GAMESTATE_H
#define DIAMINY_GAMESTATE_H

struct GameState {
    const Board *board;
    std::set<idx_t> gathered_diamonds;
    std::vector<Direction> moves;
    idx_t position;

    inline GameState next(Move &move) const {
        GameState ret;
        ret.gathered_diamonds = gathered_diamonds;
        std::copy(move.diamonds.begin(), move.diamonds.end(),
                  std::inserter(ret.gathered_diamonds, ret.gathered_diamonds.end()));
        ret.moves = moves;
        ret.moves.push_back(move.direction);
        ret.position = move.to;
        ret.board = board;
        return ret;
    }

    int compare(const GameState &other) const;

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
