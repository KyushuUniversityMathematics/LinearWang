#ifndef LINEARWANG_OUTPUT_H
#define LINEARWANG_OUTPUT_H

#include "coloring.h"

void output_tiling(const Board& board, Coloring& coloring, int max_color, unsigned size_unit, const std::string& filename, bool exterior_output);
void output_board(const Board& board, unsigned size_unit, const std::string& filename);

#endif //LINEARWANG_OUTPUT_H
