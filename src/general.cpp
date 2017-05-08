#include <vector>
#include <map>
#include <iostream>
#include "general.h"
#include "wang.h"
#include "cycle_solver.h"
#include "tree_solver.h"

std::vector<coord_type> find_cycle_by_dfs(Board& board, coord_type first_cell){
    board.clean_marks();
    std::vector<coord_type> precedent;



    board.mark(first_cell);
    precedent.push_back(first_cell);
    std::set<Edge, EdgeLess> visitedEdges;

    while(!precedent.empty()){
        auto current = precedent.back();

        auto adjacents = adjacent_edges(current);
        auto nextIt = std::find_if(adjacents.begin(), adjacents.end(), [board, &visitedEdges](Edge& e){ return (board.is_interior_edge(e) && visitedEdges.count(e) == 0); });

        if (nextIt == adjacents.end()){
            //All adjacent edges have been visited
            precedent.pop_back();
        } else {
            auto next = *nextIt;
            visitedEdges.insert(next);
            auto f = first(next);
            auto s = second(next);
            std::pair<int, int> nextCell;
            if (f == current)
                nextCell = s;
            else
                nextCell = f;

            if (board.is_marked(nextCell)){
                //We found a cycle
                auto start = std::find(precedent.begin(), precedent.end(), nextCell);
                std::vector<coord_type> cycle;
                std::copy(start, precedent.end(), std::back_inserter(cycle));
                board.clean_marks();
                return cycle;
            } else {
                board.mark(nextCell);
                precedent.push_back(nextCell);
            }

        }
    }
    board.clean_marks();
    return std::vector<coord_type>();
}

void complete_coloring (ColorGeneration gen, Board& board, Coloring& coloring){

    while(true) {

        coord_type first_cell;

        try {
            first_cell = board.find_a_cell();
        } catch (Board::EmptyBoard) {
            break;
        }


        auto cycle = find_cycle_by_dfs(board, first_cell);


        if (!cycle.empty())
        {
            for (auto c: cycle) {
                board.mark(c);
            }

            for (auto c: cycle) {
                auto neighbors = board.neighbors(c);
                for (auto n: neighbors) {
                    if (board.is_marked(n))
                        continue;
                    board.mark(n);

                    std::vector<coord_type> stack;
                    stack.push_back(n);
                    while (!stack.empty()) {
                        auto current = stack.back();
                        auto candidates = board.neighbors(current);
                        std::vector<coord_type> next;
                        auto nextIt = std::find_if(candidates.begin(), candidates.end(),
                                                   [board](coord_type c) { return !board.is_marked(c); });
                        if (nextIt == candidates.end()) {
                            stack.pop_back();
                            auto t = get_tile(coloring, current);
                            gen.complete_tile(t);
                            set_tile(coloring, current, t);
                            board.set_to_tiled(current);
                        } else {
                            board.mark(*nextIt);
                            stack.push_back((*nextIt));
                        }
                    }
                }
            }

            complete_coloring_cycle(gen, board, coloring, cycle);
        } else {
            std::cout<<"Using tree solver\n";
            if (!solve_tree_from_root(gen, board, coloring, first_cell)) {
                std::cerr << "Tree structure with root in "<< first_cell<<" is unsolvable.\n";
                exit(1);
            }
        }
    }
};