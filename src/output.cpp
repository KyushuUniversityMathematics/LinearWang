#include <fstream>
#include <iostream>
#include "output.h"
#include "coloring.h"
#include "wang.h"

void print_line(std::ofstream& out, int x1, int y1, int x2, int y2, const std::string& color, int stroke_width) {
    out << "\t<line x1=\"" << x1;
    out << "\" y1=\"" << y1;
    out << "\" x2=\"" << x2;
    out << "\" y2=\"" << y2;
    out << "\" stroke=\"" << color;
    out << "\" stroke-width=\"" << stroke_width;
    out << "\"/>\n";
}

void print_square(std::ofstream& out, int x1, int y1, int x2, int y2, const std::string& color, int stroke_width){
    out << "\t<polygon points=\"" << x1 << "," <<y1<<" ";
    out << x1 << "," << y2<<" ";
    out << x2 << "," << y2<<" ";
    out << x2 << "," << y1;
    out << "\" stroke=\"" << color;
    out << "\" stroke-width=\"" << stroke_width;
    out << "\"/>\n";
}

void print_tile(std::ofstream& out, Coloring& coloring, coord_type c, unsigned unit_size, int max_color){
    tile t = get_tile(coloring, c);
    if (std::count(t.begin(), t.end(), -1) > 0) {
        std::cerr << "Incomplete tile in "<<c<<'\n';
    } else if ((t[0] == t[2]) == (t[1] == t[3])) {
        std::cerr << "Invalid tile in "<<c<<'\n';
    } else {
        int x = c.first * unit_size;
        int y = c.second * unit_size;
        int xx = x + unit_size;
        int yy = y + unit_size;

        int bottom = static_cast<int> (x + (static_cast<float> (t[0]+1) / (max_color+1)) * unit_size);
        int left = static_cast<int> (y + (static_cast<float> (t[1]+1) / (max_color+1)) * unit_size);
        int top = static_cast<int> (x + (static_cast<float> (t[2]+1) / (max_color+1)) * unit_size);
        int right = static_cast<int> (y + (static_cast<float> (t[3]+1) / (max_color+1)) * unit_size);


        if (t[0] == t[2]){
            print_line(out, x, left, top, left, "#000000", 2);
            print_line(out, top, right, xx, right, "#000000", 2);
            print_line(out, top, y, top, yy, "#000000", 2);
        } else {
            print_line(out, x, left, xx, left, "#000000", 2);
            print_line(out, top, y, top, left, "#000000", 2);
            print_line(out, bottom, left, bottom, yy, "#000000", 2);
        }
    }

}

void print_cell(std::ofstream& out, coord_type c, unsigned unit_size){
    int x = c.first * unit_size;
    int y = c.second * unit_size;
    int xx = x + unit_size;
    int yy = y + unit_size;

    print_square(out, x, y, xx, yy, "#000000", 2);
}

void output_tiling(const Board& board, Coloring& coloring, int max_color, unsigned size_unit, const std::string& filename, bool exterior_output){
    std::ofstream ofs;

    ofs.open(filename);

    ofs << "<svg width=\""<<board.width()*size_unit<<"\" height=\""<<board.height()*size_unit<<"\" xmlns=\"http://www.w3.org/2000/svg\">\n";

    if (exterior_output){
        board.outside_vertex_iter([&ofs, &board, &coloring, max_color, size_unit](coord_type v){
            print_tile(ofs, coloring, v, size_unit, max_color);
        });
    }

    board.vertex_iter([&ofs, &board, &coloring, max_color, size_unit](coord_type v){
        print_tile(ofs, coloring, v, size_unit, max_color);
    });

    ofs << "</svg>\n";

    ofs.close();
}

void output_board(const Board& board, unsigned size_unit, const std::string& filename){
    std::ofstream ofs;
    ofs.open(filename);
    ofs << "<svg width=\""<<board.width()*size_unit<<"\" height=\""<<board.height()*size_unit<<"\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    board.vertex_iter([&ofs, &board, size_unit](coord_type v){
        print_cell(ofs, v, size_unit);
    });
    ofs << "</svg>\n";
    ofs.close();
}