#include "Board.h"

Board::Board(dim_t width, dim_t height, size_t max_moves) :
        width(width), height(height), max_moves(max_moves) {
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
    if (type == DIAMOND) {
        diamond_positions.insert(to_index(x, y));
    }
}

void Board::set_ball(dim_t x, dim_t y) {
    if (x >= width || y >= height) {
        throw std::invalid_argument("out or range");
    }

    ball_x = x;
    ball_y = y;
}

CellType Board::get_cell(dim_t x, dim_t y) const {
    if (x >= width || y >= height) {
        throw std::invalid_argument("out or range");
    }

    return board[x][y];
}

std::tuple<dim_t, dim_t> Board::move_single(dim_t x, dim_t y, Direction dir) const {
    dim_t x2 = x;
    dim_t y2 = y;
    switch (dir) {
        case UP:
            --y2;
            break;
        case UP_RIGHT:
            ++x2;
            --y2;
            break;
        case RIGHT:
            ++x2;
            break;
        case DOWN_RIGHT:
            ++x2;
            ++y2;
            break;
        case DOWN:
            ++y2;
            break;
        case DOWN_LEFT:
            --x2;
            ++y2;
            break;
        case LEFT:
            --x2;
            break;
        case UP_LEFT:
            --x2;
            --y2;
            break;
    }
    if (x2 < 0 || x2 >= width || y2 < 0 || y2 >= height) {
        std::tuple<dim_t, dim_t> ret(x, y);
        return ret;
    }
    std::tuple<dim_t, dim_t> ret(x2, y2);
    return ret;
}

Move Board::move(dim_t original_x, dim_t original_y, Direction dir) const {
    Move ret{};
    ret.direction = dir;

    dim_t x = original_x;
    dim_t y = original_y;

    bool finish = false;
    while (!finish) {
        dim_t next_x, next_y;
        std::tie(next_x, next_y) = move_single(x, y, dir);
        CellType next_cell = get_cell(next_x, next_y);

        switch (next_cell) {
            case CONCRETE:
                ret.to_x = x;
                ret.to_y = y;
                return ret;

            case MINE:
                ret.diamonds.clear();
                ret.to_x = original_x;
                ret.to_y = original_y;
                return ret;

            case DIAMOND:
                ret.diamonds.insert(to_index(next_x, next_y));
            case SPACE:
                x = next_x;
                y = next_y;
                break;

            case HOLE:
                ret.to_x = next_x;
                ret.to_y = next_y;
                return ret;

            default:
                finish = true;
        }
    }

    throw std::invalid_argument("unknown direction");
}

Board *BoardParser::parse(std::istream &input) {
    size_t width, height;
    size_t max_moves;
    std::vector<std::string> lines;

    input >> height;
    input >> width;
    input >> max_moves;

    auto *board = new Board(width, height, max_moves);

    for (dim_t y = 0; y < height; ++y) {
        for (dim_t x = 0; x < width; ++x) {
            CellType type;

            char ch;
            input.get(ch);

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
                    --x;
                    continue;
            }

            board->set_cell(x, y, type);
        }
    }

    return board;
}
