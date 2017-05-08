#ifndef LINEARWANG_TREE_SOLVER_H
#define LINEARWANG_TREE_SOLVER_H

#include "board.h"
#include "coloring.h"
#include "wang.h"

bool solve_tree_from_root(ColorGeneration g, Board& board, Coloring& coloring, coord_type root);

#endif //LINEARWANG_TREE_SOLVER_H
