#ifndef DIAMINY_BOARDSOLVER_H
#define DIAMINY_BOARDSOLVER_H

#include "Board.h"
#include "GameState.h"

using solution_t = std::vector<Direction>;

struct SCCSolution {
    solution_t moves;
    idx_t final_position;
};

class BoardGraph {
private:
    std::vector<std::vector<Move>> adjacency;

    Board *board;

    std::vector<std::vector<idx_t>> adjacency_transpose;

    std::set<idx_t> scc_leaders;

    std::vector<idx_t> scc_leader_mapping;

    std::vector<std::list<SCCMove>> scc_adjacency;

    std::vector<std::set<idx_t>> scc_diamonds;

private:
    void fill_graph(idx_t position, std::set<idx_t> &filled);

public:
    explicit BoardGraph(Board *board);

    virtual ~BoardGraph();

    void print() const;

    solution_t bfs();
};

class BoardSolver {
public:
    static solution_t solve(Board *board);
};

#endif
