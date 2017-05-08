#ifndef LINEARWANG_COLORING_H
#define LINEARWANG_COLORING_H

#include "board.h"
#include <map>
#include <array>

typedef std::map<Edge, int, EdgeLess> Coloring;

inline int get_color(Coloring& c, Edge e) {
    int result;
    try{
        result = c.at(e);
    } catch (std::out_of_range) {
        result = -1;
    }
    return result;
}

inline void set_color(Coloring& c, Edge e, int color) {
    c[e] = color;
}

inline std::array<int, 4> get_tile(Coloring& c, coord_type current){
    std::array<int, 4> t {{
            get_color(c, top(current)),
            get_color(c, left(current)),
            get_color(c, bottom(current)),
            get_color(c, right(current))
    }};
    return t;
}

inline void set_tile(Coloring& c, coord_type current, std::array<int, 4> t){
    set_color(c, top(current), t[0]);
    set_color(c, left(current), t[1]);
    set_color(c, bottom(current), t[2]);
    set_color(c, right(current), t[3]);
}

#endif //LINEARWANG_COLORING_H
