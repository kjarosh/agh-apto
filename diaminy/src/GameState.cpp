#include "GameState.h"

int GameState::compare(const GameState &other) const {
    const std::set<idx_t> &mine = gathered_diamonds;
    const std::set<idx_t> &theirs = other.gathered_diamonds;
    auto mine_end = mine.end();
    auto theirs_end = theirs.end();
    bool i_have_more = false;
    bool they_have_more = false;

    auto my_iter = mine.begin();
    auto their_iter = theirs.begin();

    while (my_iter != mine_end && their_iter != theirs_end) {
        idx_t my_val = *my_iter;
        idx_t their_val = *my_iter;
        if (my_val > their_val) {
            they_have_more = true;
            ++their_iter;
        } else if (my_val < their_val) {
            i_have_more = true;
            ++my_iter;
        } else {
            ++their_iter;
            ++my_iter;
        }
    }

    size_t other_moves_size = other.moves.size();
    size_t moves_size = moves.size();
    if (my_iter == mine_end && their_iter == theirs_end) {
        if (!i_have_more && !they_have_more) {
            return other_moves_size - moves_size;
        }
    } else if (my_iter == mine_end && their_iter != theirs_end) {
        they_have_more = true;
    } else if (my_iter != mine_end && their_iter == theirs_end) {
        i_have_more = true;
    }

    if (i_have_more == they_have_more) {
        return 0;
    }

    if (they_have_more && other_moves_size < moves_size) {
        return -1;
    }

    if (i_have_more && other_moves_size > moves_size) {
        return 1;
    }

    return 0;
}
