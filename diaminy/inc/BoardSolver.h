#ifndef DIAMINY_BOARDSOLVER_H
#define DIAMINY_BOARDSOLVER_H

#include "Board.h"
#include "GameState.h"

using solution_t = std::vector<Direction>;

class BoardGraph {
private:
    Board const *const board;

    std::vector<std::vector<Move>> adjacency;

private:
    void fill_graph(idx_t position, std::set<idx_t> &filled);

public:
    explicit BoardGraph(Board const *board);

    void print() const;

    solution_t bfs() const;
};

class BoardSolver {
public:
    static solution_t solve(Board const *board);
};

#endif
