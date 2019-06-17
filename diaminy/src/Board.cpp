#include "Board.h"

Board::Board(dim_t width, dim_t height, size_t max_moves) :
        width(width), height(height), max_moves(max_moves), size(width * height) {
    board = new CellType[size];
}

Board::~Board() {
    delete[] board;
}

void Board::set_cell(idx_t position, CellType type) {
    if (position < 0 || position >= size) {
        throw std::invalid_argument("out or range");
    }

    board[position] = type;
    if (type == DIAMOND) {
        diamond_positions.insert(position);
    }
}

void Board::set_ball(idx_t position) {
    if (position < 0 || position >= size) {
        throw std::invalid_argument("out or range");
    }

    ball_position = position;
}

CellType Board::get_cell(idx_t position) const {
    if (position < 0 || position >= size) {
        throw std::invalid_argument("out or range");
    }

    return board[position];
}

idx_t Board::move_single(idx_t position, Direction dir) const {
    dim_t x, y;
    std::tie(x, y) = from_index(position);
    switch (dir) {
        case UP:
            --y;
            break;
        case UP_RIGHT:
            ++x;
            --y;
            break;
        case RIGHT:
            ++x;
            break;
        case DOWN_RIGHT:
            ++x;
            ++y;
            break;
        case DOWN:
            ++y;
            break;
        case DOWN_LEFT:
            --x;
            ++y;
            break;
        case LEFT:
            --x;
            break;
        case UP_LEFT:
            --x;
            --y;
            break;
    }
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return position;
    }
    return to_index(x, y);
}

Move Board::move(idx_t original, Direction dir) const {
    Move ret{};
    ret.direction = dir;

    idx_t position = original;

    bool finish = false;
    while (!finish) {
        idx_t next = move_single(position, dir);
        CellType next_cell = get_cell(next);

        switch (next_cell) {
            case CONCRETE:
                ret.to = position;
                return ret;

            case MINE:
                ret.diamonds.clear();
                ret.to = original;
                return ret;

            case DIAMOND:
                ret.diamonds.insert(next);
            case SPACE:
                position = next;
                break;

            case HOLE:
                ret.to = next;
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
                    board->set_ball(board->to_index(x, y));

                case ' ':
                    type = SPACE;
                    break;

                default:
                    --x;
                    continue;
            }

            board->set_cell(board->to_index(x, y), type);
        }
    }

    return board;
}
