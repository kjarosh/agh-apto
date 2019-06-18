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

    void fill_transpose();

    void dfs_visit(idx_t position, std::set<idx_t> &visiting, std::vector<idx_t> &visited_stack) const;

    void dfs_visit_transpose(idx_t leader, idx_t ix, std::set<idx_t> &visiting);

    void build_scc(std::vector<idx_t> &visited_stack);

    void build_scc_graph();

    solution_t search0(std::vector<std::unordered_set<GameState>> &computed_states, const GameState &state);

    std::set<idx_t> scc_find_path(std::vector<idx_t> &vector, idx_t leader);

    SCCSolution search_within_scc(idx_t from, idx_t leader, idx_t target_leader);

    SCCSolution search_final_within_scc(idx_t from, idx_t leader);

    /**
     * @param computed_states best computed states per position
     * @param state current state
     * @param leader current SCC leader
     * @param target position we desire as the final one
     * @return found best solution
     */
    SCCSolution search_within_scc0(
            std::vector<std::unordered_set<GameState>> &computed_states,
            const GameState &state, idx_t leader, idx_t target_leader);

public:
    explicit BoardGraph(Board *board);

    virtual ~BoardGraph();

    void print() const;

    solution_t search();

    solution_t solve_using_scc();
};

class BoardSolver {
public:
    static solution_t solve(Board *board);
};

#endif
