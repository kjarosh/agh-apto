#include "all.cpp"

int main(int argc, char **argv) {
    Board *board = BoardParser::parse(std::cin);
    solution_t solution = BoardSolver::solve(board);

    if (solution.empty()) {
        std::cout << "BRAK";
    } else {
        for (auto &i : solution) {
            std::cout << i;
        }
    }

    std::cout << '\n';

    delete board;
}
