#ifndef LINEARWANG_GENERAL_H
#define LINEARWANG_GENERAL_H

#include "board.h"
#include "coloring.h"
#include "wang.h"

std::vector<coord_type> find_cycle_by_dfs(Board& board);

void complete_coloring (ColorGeneration gen, Board& board, Coloring& coloring);

#endif //LINEARWANG_GENERAL_H
