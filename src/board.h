#ifndef LINEARWANG_BOARD_H
#define LINEARWANG_BOARD_H

#include <exception>
#include <utility>
#include <vector>
#include <map>
#include <cstdlib>
#include <set>

typedef std::pair<int, int> coord_type;

std::ostream& operator<<(std::ostream& os, const coord_type& c);

enum class Orientation { H, V };

struct Edge{
    Edge(Orientation o, int i, int j): o(o), i(i), j(j) {}
    Orientation o;
    int i;
    int j;
};

inline bool operator==(const Edge& e1, const Edge& e2){
    return e1.o == e2.o && e1.i == e2.i && e1.j == e2.j;
}

struct EdgeLess {
    bool operator() (const Edge& lhs, const Edge& rhs) const
    {
        if(lhs.o == Orientation::H && rhs.o == Orientation::V) return true;
        if(lhs.o == rhs.o){
            if (lhs.i < rhs.i) return true;
            if (lhs.i == rhs.i) return (lhs.j < rhs.j);
        }
        return false;
    }
};

inline Edge top(coord_type c){
    return Edge(Orientation::H, c.first, c.second);
}

inline Edge left(coord_type c){
    return Edge(Orientation::V, c.first-1, c.second);
}

inline Edge bottom(coord_type c){
    return Edge(Orientation::H, c.first, c.second-1);
}

inline Edge right(coord_type c){
    return Edge(Orientation::V, c.first, c.second);
}

inline std::vector<Edge> adjacent_edges(coord_type c){
    return std::vector<Edge>({top(c), left(c), bottom(c), right(c)});
}

inline coord_type first(Edge e) {
    return std::make_pair(e.i, e.j);
}

inline coord_type second(Edge e) {
    switch(e.o){
        case Orientation::H:
            return std::make_pair(e.i, e.j+1);
        case Orientation::V:
            return std::make_pair(e.i+1, e.j);
    }
}

class NoEdge: public std::exception {};

inline Edge edge_between(coord_type cin, coord_type cout){
    if (std::abs(cin.first-cout.first) + std::abs(cin.second - cout.second) != 1)
        throw(NoEdge());

    if (cin.first + 1 == cout.first){
        return Edge(Orientation::V, cin.first, cin.second);
    } else if (cin.first - 1 == cout.first){
        return Edge(Orientation::V, cout.first, cout.second);
    } else if (cin.second + 1 == cout.second){
        return Edge(Orientation::H, cin.first, cin.second);
    } else {
        return Edge(Orientation::H, cout.first, cout.second);
    }
}

class Board {
public:
    Board(size_t width, size_t height)
            : m_width(width)
            , m_height(height)
            , m_cells(width * height, 0) {}

    size_t to_index(coord_type c) const {
        return m_width * static_cast<size_t>(c.second) + static_cast<size_t>(c.first);
    }

    std::pair<int, int> from_index(size_t index) const {
        int i = static_cast<int>(index % m_width);
        int j = static_cast<int>(index / m_width);
        return std::make_pair(i, j);
    };

    bool in_boundaries(coord_type c) const {
        return (c.first >= 0 && c.first < static_cast<int>(m_width) && c.second >= 0 && c.second < static_cast<int>(m_height));
    }

    void add_cell(coord_type c){
        if (in_boundaries(c)){
            m_cells[to_index(c)] = 1;
        }
    }

    void add_cell(int i, int j){
        add_cell(std::make_pair(i, j));
    }

    bool in_polygon(coord_type c) const {
        if (in_boundaries(c))
            return m_cells[to_index(c)] > 0;
        return false;
    }

    bool in_polygon(int i, int j) const {
        return in_polygon(std::make_pair(i, j));
    }

    std::vector<coord_type> neighbors(coord_type c){
        std::vector<coord_type> n;
        auto topCell = std::make_pair(c.first, c.second+1);
        auto leftCell = std::make_pair(c.first-1, c.second);
        auto bottomCell = std::make_pair(c.first, c.second-1);
        auto rightCell = std::make_pair(c.first+1, c.second);

        if (in_polygon(topCell)) n.push_back(topCell);
        if (in_polygon(leftCell)) n.push_back(leftCell);
        if (in_polygon(bottomCell)) n.push_back(bottomCell);
        if (in_polygon(rightCell)) n.push_back(rightCell);
        return n;
    }



    bool is_boundary_edge(Edge e) const {
        auto f = first(e);
        auto s = second(e);
        return (in_polygon(f) != in_polygon(s));
    }

    bool is_interior_edge(Edge e) const {
        return in_polygon(first(e)) && in_polygon(second(e));
    }

    void mark(coord_type c){
        if (in_polygon(c) && m_cells[to_index(c)]%2 == 1)
            m_cells[to_index(c)] += 1;
    }

    bool is_marked(coord_type c) const {
        if (in_polygon(c))
            return m_cells[to_index(c)] % 2 == 0;
        return false;
    }

    void clean_marks() {
        for(size_t index = 0; index < m_width * m_height; ++index){
            if (m_cells[index] > 0 && m_cells[index] % 2 == 0)
                m_cells[index] -= 1;
        }
    }

    void set_to_tiled(coord_type c) {
        if(in_polygon(c) && m_cells[to_index(c)] < 3){
            m_cells[to_index(c)] += 2;
        }
    }

    void vertex_iter(std::function<void(coord_type)> f) const {
        for(size_t index = 0; index < m_width * m_height; ++index){
            if (m_cells[index] >= 1) f(from_index(index));
        }
    }

    void outside_vertex_iter(std::function<void(coord_type)> f) const {
        for(size_t index = 0; index < m_width * m_height; ++index){
            if (m_cells[index] == 0) f(from_index(index));
        }
    }

    void edge_iter(std::function<void(Edge)> f){
        std::set<Edge, EdgeLess> visited;
        for(size_t index = 0; index < m_width * m_height; ++index){
            if (m_cells[index] == 1){
                for (auto e: adjacent_edges(from_index(index))){
                    if (visited.count(e) == 0){
                        f(e);
                        visited.insert(e);
                    }
                }
            }
        }
    }

    class EmptyBoard: public std::exception {};

    coord_type find_a_cell() {
        for (size_t index = 0; index < m_width * m_height; ++index){
            if (m_cells[index] == 1)
                return from_index(index);
        }
        throw (EmptyBoard());
    }

    size_t width() const { return m_width; }
    size_t height() const { return m_height; }

private:
    size_t m_width;
    size_t m_height;
    std::vector<int> m_cells;

};

#endif //LINEARWANG_BOARD_H
