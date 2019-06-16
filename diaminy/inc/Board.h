#ifndef DIAMINY_BOARD_H
#define DIAMINY_BOARD_H

typedef size_t dim_t;

enum CellType {
    SPACE = 0,
    CONCRETE = 1,
    MINE = 2,
    DIAMOND = 3,
    HOLE = 4,
};

class Board {
private:
    dim_t width, height;

    CellType **board;

    dim_t ball_x{0}, ball_y{0};

public:
    Board(dim_t width, dim_t height);

    void set_cell(dim_t x, dim_t y, CellType type);

    void set_ball(dim_t x, dim_t y);

    CellType get_cell(dim_t x, dim_t y);

    dim_t get_ball_x();

    dim_t get_ball_y();
};

class BoardParser {
public:
    static Board *parse(std::istream &input);
};

#endif
