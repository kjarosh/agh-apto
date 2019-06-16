#include "all.cpp"

int main(int argc, char **argv) {
    Board *board = BoardParser::parse(std::cin);
    solution_t solution = BoardSolver::solve(board);

    for (auto &i : solution) {
        std::cout << i.direction; // << '/' << i.to_x << ',' << i.to_y << ' ';
    }

    std::cout << '\n';

    delete board;
}
