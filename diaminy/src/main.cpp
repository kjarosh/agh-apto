#include "all.cpp"

int main(int argc, char **argv) {
    Board *board = BoardParser::parse(std::cin);

    std::cout << "Done" << std::endl;

    std::cout << board->get_cell(0, 0) << std::endl;

    delete board;
}
