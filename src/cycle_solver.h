#ifndef LINEARWANG_CYCLE_SOLVER_H
#define LINEARWANG_CYCLE_SOLVER_H

#include "board.h"
#include "coloring.h"
#include "wang.h"

class CellType {
public:
    CellType(coord_type pos, Edge input, Edge output): pos(pos), input(input), output(output), in_color(-1), out_color(-1) {}

    virtual ~CellType() = default;
    virtual bool is_pass() = 0;
    virtual bool is_straight() = 0;
    virtual int propagate(ColorGeneration g, int in_color) = 0;

    coord_type pos;
    Edge input;
    Edge output;
    int in_color;
    int out_color;
};

class Straight: public CellType {
public:
    Straight(int a, int b, coord_type pos, Edge input, Edge output): CellType(pos, input, output), a(a), b(b) {}

    int propagate(ColorGeneration g, int inc) final {
        in_color = inc;
        if (a == b)
            out_color = g.pick_different_color(inc);
        else
            out_color = inc;
        return out_color;
    }

    bool is_pass() final {
        return a != b;
    }

    bool is_straight() final {
        return true;
    }

    int a;
    int b;

};

class Corner: public CellType {
public:
    Corner(int a, int b, coord_type pos, Edge input, Edge output): CellType(pos, input, output), a(a), b(b) {}

    int propagate(ColorGeneration g, int inc) final {
        in_color = inc;
        if (a == in_color)
            out_color = g.pick_different_color(b);
        else
            out_color = b;
        return out_color;
    }

    bool is_pass() final {
        return false;
    }

    bool is_straight() final {
        return false;
    }

    int a;
    int b;
};

std::vector<std::shared_ptr<CellType>> analysis(Coloring& coloring, const std::vector<coord_type>& cycle);
void complete_coloring_cycle(ColorGeneration g, Board& board, Coloring& coloring, const std::vector<coord_type>& cycle);

#endif //LINEARWANG_CYCLE_SOLVER_H
