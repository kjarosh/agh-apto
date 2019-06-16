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

BoardParser::BoardParser(std::istream input) {
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }

    if (lines.empty()) {
        throw std::invalid_argument("empty board");
    }
}

Board *BoardParser::parse() {

}
