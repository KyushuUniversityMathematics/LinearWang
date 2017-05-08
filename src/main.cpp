#include <iostream>
#include <fstream>
#include "board.h"
#include "general.h"
#include "wang.h"
#include "output.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char* argv[]) {

    std::vector<std::string> arguments(argv + 1, argv + argc);

    bool exterior_output = true;

    auto flagIt = std::find(arguments.begin(), arguments.end(), "-ne");
    if (flagIt != arguments.end()){
        exterior_output = false;
        arguments.erase(flagIt);
    }

    if (arguments.size() != 1){
        std::cout<<"Usage:\n";
        std::cout<<"\t"<<argv[0]<<" [-ne] MASK\n";
        std::cout<<"where MASK is a png image.\n";
        std::cout<<"Use the flag \"-ne\" to remove the exterior in the output.\n";
        return 1;
    }

    int n;
    int width, height;
    unsigned char *data = stbi_load(arguments[0].c_str(), &width, &height, &n, 0);

    if (!data) {
        std::cout<<"Error while opening mask "<<arguments[0].c_str()<<std::endl;
        return 1;
    }

    Board b(width, height);

    for(int j = height-1; j >= 0; --j){
        for (int i = 0 ; i < width ; ++i){
            unsigned char c = data[(j * width + i)*n+n-1];
            if (c >= 1) {
                b.add_cell(i, j);
            }
        }
    }

    stbi_image_free(data);

    ColorGeneration gen(1234, 3);

    Coloring c;

    b.edge_iter([&b, &c](Edge e) { if (b.is_boundary_edge(e)) set_color(c, e, 0); });

    b.outside_vertex_iter([&b, &c](coord_type v){
        set_color(c, left(v), 1);
        set_color(c, right(v), 1);
        if (v.second % 2 == 0){
            set_color(c, top(v), 0);
            set_color(c, bottom(v), 2);
        } else {
            set_color(c, top(v), 2);
            set_color(c, bottom(v), 0);
        }
    });


    complete_coloring(gen, b, c);

    output_tiling(b, c, 3, 20, "out.svg", exterior_output);
    return 0;
}