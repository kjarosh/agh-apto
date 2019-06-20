#include "all.cpp"
#ifdef TEST
#include "test.cpp"
#endif

int main(int argc, char **argv) {
#ifdef TEST
    return test();
#endif

    Board *board = nullptr;
    try {
        board = BoardParser::parse(std::cin);
        solution_t solution = BoardSolver::solve(board);

        if (solution.empty()) {
            throw no_solution_exception();
        }

        for (auto &i : solution) {
            std::cout << i;
        }

        std::cout << '\n';
    } catch (no_solution_exception &e) {
        std::cout << "BRAK\n";
    }

    delete board;
}
