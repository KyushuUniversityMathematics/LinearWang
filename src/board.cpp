#include "board.h"
#include <iostream>

std::ostream& operator<<(std::ostream& os, const coord_type& c){
    return os << '(' << c.first << ';' << c.second << ')';
}