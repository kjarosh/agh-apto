#ifndef DIAMINY_BOARDSOLVER_H
#define DIAMINY_BOARDSOLVER_H

#include "Board.h"
#include "GameState.h"

using solution_t = std::vector<Direction>;

class BoardGraph {
private:
    std::vector<std::vector<Move>> adjacency;

    Board *board;

    std::vector<std::vector<idx_t>> adjacency_transpose;

    std::set<idx_t> scc_leaders;

    std::vector<idx_t> scc_leader_mapping;

    std::vector<std::set<idx_t>> scc_adjacency;

private:
    void fill_graph(idx_t position, std::set<idx_t> &filled);

    void fill_transpose();

    void dfs_visit(idx_t position, std::set<idx_t> &visiting, std::vector<idx_t> &visited_stack) const;

    void dfs_visit_transpose(idx_t leader, idx_t ix, std::set<idx_t> &visiting);

    void build_scc(std::vector<idx_t> &vector);

    solution_t search0(std::vector<std::unordered_set<GameState>> &computed_states, const GameState &state);

public:
    explicit BoardGraph(Board *board);

    virtual ~BoardGraph();

    void print();

    solution_t search();

    void build_scc_graph();
};

class BoardSolver {
public:
    static solution_t solve(Board *board);
};

#endif
