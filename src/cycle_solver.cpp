#include <iostream>
#include "cycle_solver.h"

std::vector<std::shared_ptr<CellType>> analysis(Coloring& coloring, const std::vector<coord_type>& cycle) {
    std::vector<std::shared_ptr<CellType>> types;

    coord_type previous = cycle.back();

    for (auto c: cycle){
        tile t = get_tile(coloring, c);
        auto n = std::count(t.begin(), t.end(), -1);
        if (n != 2){
            std::cerr << "Error in the cycle coloring: n"<< c <<" = "<<n<<"\n";
            exit(1);
        }

        Edge input = edge_between(previous, c);
        Edge output = input;

        if (t[0] != -1 && t[2] != -1) {
            if (input == left(c))
                output = right(c);
            else
                output = left(c);
            types.push_back(std::shared_ptr<CellType>(new Straight(t[0], t[2], c, input, output)));
        } else if (t[1] != -1 && t[3] != -1) {
            if (input == top(c))
                output = bottom(c);
            else
                output = top(c);
            types.push_back(std::shared_ptr<CellType>(new Straight(t[1], t[3], c, input, output)));
        } else if (t[0] != -1 && t[1] != -1) {
            if (input == bottom(c)){
                types.push_back(std::shared_ptr<CellType>(new Corner(t[0], t[1], c, input, right(c))));
            } else {
                types.push_back(std::shared_ptr<CellType>(new Corner(t[1], t[0], c, input, bottom(c))));
            }
        } else if (t[1] != -1 && t[2] != -1) {
            if (input == top(c)){
                types.push_back(std::shared_ptr<CellType>(new Corner(t[2], t[1], c, input, right(c))));
            } else {
                types.push_back(std::shared_ptr<CellType>(new Corner(t[1], t[2], c, input, top(c))));
            }
        } else if (t[2] != -1 && t[3] != -1) {
            if (input == top(c)) {
                types.push_back(std::shared_ptr<CellType>(new Corner(t[2], t[3], c, input, left(c))));
            } else {
                types.push_back(std::shared_ptr<CellType>(new Corner(t[3], t[2], c, input, top(c))));
            }
        } else if (t[3] != -1 && t[0] != -1) {
            if (input == bottom(c)) {
                types.push_back(std::shared_ptr<CellType>(new Corner(t[0], t[3], c, input, left(c))));
            } else {
                types.push_back(std::shared_ptr<CellType>(new Corner(t[3], t[0], c, input, bottom(c))));
            }
        } else {
            std::cerr << "Error in the cycle analysis\n";
            exit(1);
        }
        previous = c;
    }

    return types;
}

void complete_coloring_cycle(ColorGeneration g, Board& board, Coloring& coloring, const std::vector<coord_type>& cycle){
    auto types = analysis(coloring, cycle);

    if (types.size() != cycle.size()){
        std::cerr<<"Error in analysing the cycle: size mismatch.\n";
        exit(1);
    }

    std::vector<std::shared_ptr<CellType>> primitive;
    std::copy_if(
            types.begin(),
            types.end(),
            std::back_inserter(primitive),
            [](std::shared_ptr<CellType> c) { return !c->is_pass(); }
    );

    auto sit = std::find_if(primitive.begin(), primitive.end(), [](std::shared_ptr<CellType> c) { return c->is_straight();});

    if (sit != primitive.end()){
        std::rotate(primitive.begin(), sit, primitive.end());
        if (primitive[1]->is_straight()) {
            auto first = dynamic_cast<Straight*>(primitive[0].get());
            auto second = dynamic_cast<Straight*>(primitive[1].get());
            int c = g.pick_color();
            second->out_color = c;
            int output = c;

            for(size_t index = 2; index < primitive.size(); ++index){
                output = primitive[index]->propagate(g, output);
            }
            int diff = g.pick_different_color(output, c);
            first->in_color = output;
            first->out_color = diff;
            second->in_color = diff;

        } else {
            auto first = dynamic_cast<Straight*>(primitive[0].get());
            auto second = dynamic_cast<Corner*>(primitive[1].get());
            int c = second->b;
            second->out_color = c;
            int output = c;

            for(size_t index = 2; index < primitive.size(); ++index){
                output = primitive[index]->propagate(g, output);
            }
            int diff = g.pick_different_color(output, second->a);
            first->in_color = output;
            first->out_color = diff;
            second->in_color = diff;
        }
    } else {
        auto current = primitive.end()-1;
        auto next = primitive.begin();
        for (; next != primitive.end(); ++next){
            auto f = dynamic_cast<Corner*>((*current).get());
            auto s = dynamic_cast<Corner*>((*next).get());
            if (f->b != s->a) break;
            current = next;
        }

        if (next != primitive.end()){
            std::rotate(primitive.begin(), current, primitive.end());
            auto first = dynamic_cast<Corner*>(primitive[0].get());
            auto second = dynamic_cast<Corner*>(primitive[1].get());
            int c = second->b;
            second->out_color = c;
            int output = c;

            for(size_t index = 2; index < primitive.size(); ++index){
                output = primitive[index]->propagate(g, output);
            }
            int diff;
            if (output == first->a)
                diff = g.pick_different_color(first->b, second->a);
            else
                diff = first->b;

            first->in_color = output;
            first->out_color = diff;
            second->in_color = diff;
        } else {
            for(size_t index = 0 ; index < primitive.size(); index += 2){
                auto first = dynamic_cast<Corner*>(primitive[index].get());
                auto second = dynamic_cast<Corner*>(primitive[index+1].get());
                first->in_color = first->a;
                first->out_color = g.pick_different_color(first->b);
                second->in_color = first->out_color;
                second->out_color = second->b;
            }
        }
    }

    auto current = types.back();
    for (auto c: types){
        if (c->is_pass()){
            auto first = dynamic_cast<Straight*>(c.get());
            first->in_color = current->out_color;
            first->out_color = first->in_color;
        }
        current = c;
    }

    for(auto c: types){
        set_color(coloring, c->input, c->in_color);
        set_color(coloring, c->output, c->out_color);
        board.set_to_tiled(c->pos);
    }


}