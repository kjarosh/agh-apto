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

struct Move {
    dim_t to_x, to_y;
    std::set<idx_t> diamonds;
    Direction direction;

    inline bool position_equals(dim_t from_x, dim_t from_y) {
        return from_x == to_x && from_y == to_y;
    }
};

class Board {
private:
    dim_t width, height;

    size_t max_moves;

    CellType **board;

    std::set<idx_t> diamond_positions;

    dim_t ball_x{0}, ball_y{0};

public:
    Board(dim_t width, dim_t height, size_t max_moves);

    void set_cell(dim_t x, dim_t y, CellType type);

    void set_ball(dim_t x, dim_t y);

    CellType get_cell(dim_t x, dim_t y) const;

    inline size_t get_max_moves() const {
        return max_moves;
    }

    inline dim_t get_ball_x() const {
        return ball_x;
    }

    inline dim_t get_ball_y() const {
        return ball_y;
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

    std::tuple<dim_t, dim_t> move_single(dim_t x, dim_t y, Direction dir) const;

    Move move(dim_t x, dim_t y, Direction dir) const;
};

class BoardParser {
public:
    static Board *parse(std::istream &input);
};

#endif
