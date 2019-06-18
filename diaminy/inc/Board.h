#ifndef DIAMINY_BOARD_H
#define DIAMINY_BOARD_H

using dim_t = uint8_t;
using idx_t = uint16_t;

enum CellType {
    SPACE = 0,
    CONCRETE = 1,
    MINE = 2,
    DIAMOND = 3,
    HOLE = 4,
};

enum Direction {
    UP = 0,
    UP_RIGHT = 1,
    RIGHT = 2,
    DOWN_RIGHT = 3,
    DOWN = 4,
    DOWN_LEFT = 5,
    LEFT = 6,
    UP_LEFT = 7,
};

Direction all_directions[] = {
        UP,
        UP_RIGHT,
        RIGHT,
        DOWN_RIGHT,
        DOWN,
        DOWN_LEFT,
        LEFT,
        UP_LEFT,
};

struct SCCMove {
    const idx_t to_leader;
    std::set<idx_t> diamonds;

    inline static SCCMove create(idx_t to_leader, std::set<idx_t> &diamonds) {
        return {to_leader, diamonds};
    }
};

struct Move {
    idx_t to;
    std::set<idx_t> diamonds;
    Direction direction;

    inline bool position_equals(idx_t position) {
        return to == position;
    }
};

class Board {
private:
    dim_t width, height;
    idx_t size;

    size_t max_moves;

    CellType *board;

    std::set<idx_t> diamond_positions;

    idx_t ball_position;

public:
    Board(dim_t width, dim_t height, size_t max_moves);

    virtual ~Board();

    void set_cell(idx_t position, CellType type);

    void set_ball(idx_t position);

    CellType get_cell(idx_t position) const;

    inline idx_t get_size() const {
        return size;
    }

    inline size_t get_max_moves() const {
        return max_moves;
    }

    inline idx_t get_ball_index() const {
        return ball_position;
    }

    inline std::set<idx_t> get_diamond_positions() const {
        return diamond_positions;
    }

    inline dim_t get_width() const {
        return width;
    }

    inline dim_t get_height() const {
        return height;
    }

    inline idx_t to_index(dim_t x, dim_t y) const {
        return x + y * width;
    }

    inline std::tuple<dim_t, dim_t> from_index(idx_t ix) const {
        return std::make_tuple(ix % width, ix / width);
    }

    idx_t move_single(idx_t position, Direction dir) const;

    Move move(idx_t position, Direction dir) const;
};

class BoardParser {
public:
    static Board *parse(std::istream &input);
};

#endif
