#include "Board.h"

Board::Board(dim_t width, dim_t height) :
        width(width), height(height) {
    board = new CellType *[width];
    for (size_t i = 0; i < width; ++i) {
        board[i] = new CellType[height];
    }
}

void Board::set_cell(dim_t x, dim_t y, CellType type) {
    if (x >= width || y >= height) {
        throw std::invalid_argument("out or range");
    }

    board[x][y] = type;
}

void Board::set_ball(dim_t x, dim_t y) {
    if (x >= width || y >= height) {
        throw std::invalid_argument("out or range");
    }

    ball_x = x;
    ball_y = y;
}

CellType Board::get_cell(dim_t x, dim_t y) {
    if (x >= width || y >= height) {
        throw std::invalid_argument("out or range");
    }

    return board[x][y];
}

dim_t Board::get_ball_x() {
    return ball_x;
}

dim_t Board::get_ball_y() {
    return ball_y;
}

Board *BoardParser::parse(std::istream &input) {
    dim_t width, height;
    size_t max_moves;
    std::vector<std::string> lines;

    input >> height;
    input >> width;
    input >> max_moves;

    auto *board = new Board(width, height);

    for (dim_t y = 0; y < height; ++y) {
        for (dim_t x = 0; x < width; ++x) {
            CellType type;

            char ch;
            do {
                input.get(ch);
            } while (ch == '\n');

            switch (ch) {
                case '+':
                    type = DIAMOND;
                    break;

                case '*':
                    type = MINE;
                    break;

                case '#':
                    type = CONCRETE;
                    break;

                case 'O':
                    type = HOLE;
                    break;

                case '.':
                    board->set_ball(x, y);

                case ' ':
                    type = SPACE;
                    break;

                default:
                    throw std::invalid_argument("invalid cell type");
            }

            board->set_cell(x, y, type);
        }
    }

    return board;
}
