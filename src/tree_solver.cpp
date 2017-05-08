#include <iostream>
#include "tree_solver.h"
#include "wang.h"

struct Constraint {
    enum { Any, Strict, Diff } type;
    int color;

    static Constraint star() {
        Constraint s;
        s.type = Any;
        s.color = -1;
        return s;
    }

    static Constraint strict(int c) {
        Constraint s;
        s.type = Strict;
        s.color = c;
        return s;
    }

    static Constraint diff(int c) {
        Constraint s;
        s.type = Diff;
        s.color = c;
        return s;
    }
};

Constraint inv(Constraint c){
    if (c.type == Constraint::Diff || c.type == Constraint::Any)
        return Constraint::star();
    return Constraint::diff(c.color);
}

Constraint propagate(Constraint a, Constraint b, Constraint c){
    if (c.type == Constraint::Any || a.type == Constraint::Any || b.type == Constraint::Any)
        return Constraint::star();
    if (a.type == Constraint::Diff && b.type == Constraint::Diff)
        return Constraint::star();

    if (a.type == Constraint::Strict && b.type == Constraint::Strict){
        if (a.color == b.color)
            return inv(c);
        else
            return c;
    }

    if (a.color == b.color)
        return c;
    else
        return Constraint::star();
}

bool compatible(Constraint a, Constraint b){
    if (a.type == Constraint::Any || b.type == Constraint::Any) return true;
    if (a.type == Constraint::Diff && b.type == Constraint::Diff) return true;

    if (a.type == b.type) {
        return a.color == b.color;
    } else
        return a.color != b.color;
}

Constraint propagate_constraint_from_leaves(
        Board& board
        , Coloring& coloring
        , std::set<Edge, EdgeLess> visited
        , std::map<Edge, Constraint, EdgeLess>& constraints
        , coord_type child){

    std::array<Edge, 3> to_visit {{bottom(child), left(child), right(child)}};

    if (visited.count(top(child))){
        to_visit = {{bottom(child), left(child), right(child)}};
    } else if (visited.count(bottom(child))) {
        to_visit = {{top(child), left(child), right(child)}};
    } else if (visited.count(left(child))) {
        to_visit = {{right(child), top(child), bottom(child)}};
    } else if (visited.count(right(child))) {
        to_visit = {{left(child), top(child), bottom(child)}};
    } else {
        std::cerr << "No previously visited node in propagating constraint for cell "<<child<<'\n';
        exit(1);
    }

    std::array<Constraint, 3> prop_constraints;
    for(size_t index = 0; index < 3 ; ++index) {
        int c = get_color(coloring, to_visit[index]);
        if (c > -1){
            prop_constraints[index] = Constraint::strict(c);
        } else {
            visited.insert(to_visit[index]);
            auto next = first(to_visit[index]);
            if (next == child)
                next = second(to_visit[index]);
            prop_constraints[index] = propagate_constraint_from_leaves(board, coloring, visited, constraints, next);
            constraints[to_visit[index]] = prop_constraints[index];
        }
    }

    return propagate(prop_constraints[1], prop_constraints[2], prop_constraints[0]);
}

int solve_constraint(ColorGeneration g, Constraint c){
    if (c.type == Constraint::Any)
        return g.pick_color();
    if (c.type == Constraint::Diff)
        return g.pick_different_color(c.color);
    return c.color;
}

int solve_dual_constraints(ColorGeneration g, Constraint c1, Constraint c2){
    // The two constraints are assumed compatible.
    if (c1.type == Constraint::Any)
        return solve_constraint(g, c2);
    if (c2.type == Constraint::Any)
        return solve_constraint(g, c1);

    if (c1.type == Constraint::Strict)
        return c1.color;
    if (c2.type == Constraint::Strict)
        return c2.color;

    return g.pick_different_color(c1.color, c2.color);
}

std::array<int, 4> solve_constraints(ColorGeneration g, std::array<Constraint, 4> constraints){
    int a = 0, b = 1, c = 2, d = 3;
    std::array<int, 4> solution;
    for(size_t index = 0 ; index < 4 ; ++index){
        if (constraints[index].type == Constraint::Strict)
            solution[index] = constraints[index].color;
        else
            solution[index] = -1;
    }

    if (solution[a] == -1) std::swap(a, c);
    if (solution[b] == -1) std::swap(b, d);

    if (solution[a] == -1 && solution[b] > -1) { std::swap(a, b); std::swap(c,d); }

    if (solution[b] == -1 && solution[c] > -1) {
        if (solution[a] != solution[c]){
            int col = solve_dual_constraints(g, constraints[b], constraints[d]);
            solution[b] = col;
            solution[d] = col;
        } else {
            solution[b] = solve_constraint(g, constraints[b]);
            solution[d] = solve_dual_constraints(g, constraints[d], Constraint::diff(solution[b]));
        }
        return solution;
    }

    if (solution[c] == -1 && solution[d] > -1) { std::swap(a, b); std::swap(c,d); }

    if (solution[d] > -1) return solution;

    if((solution[c] > -1 && solution[a] == solution[c]) || (solution[c] == -1 && compatible(constraints[a], constraints[c]) && (g.get() > 0.5 || !compatible(constraints[b], constraints[d])))) {
        solution[a] = solve_dual_constraints(g, constraints[a], constraints[c]);
        solution[c] = solution[a];
        solution[b] = solve_constraint(g, constraints[b]);
        solution[d] = solve_dual_constraints(g, constraints[d], Constraint::diff(solution[b]));
    } else {
        solution[b] = solve_dual_constraints(g, constraints[b], constraints[d]);
        solution[d] = solution[b];
        solution[a] = solve_constraint(g, constraints[a]);
        solution[c] = solve_dual_constraints(g, constraints[c], Constraint::diff(solution[a]));
    }
    return solution;
};

void propagate_solution(ColorGeneration g, Board& board, Coloring& coloring, std::map<Edge, Constraint, EdgeLess>& constraints, coord_type root){
    std::array<Edge, 4> to_visit {{ top(root), left(root), bottom(root), right(root)}};

    std::array<Constraint, 4> prop_constraints;
    for(size_t index = 0; index < 4 ; ++index) {
        int c = get_color(coloring, to_visit[index]);
        if (c > -1){
            prop_constraints[index] = Constraint::strict(c);
        } else {
            prop_constraints[index] = constraints.at(to_visit[index]);
        }
    }

    std::array<int, 4> solution = solve_constraints(g, prop_constraints);
    for(size_t index = 0; index < 4 ; ++index) {
        int c = get_color(coloring, to_visit[index]);
        if (c == -1){
            set_color(coloring, to_visit[index], solution[index]);
            auto next = first(to_visit[index]);
            if (next == root)
                next = second(to_visit[index]);
            propagate_solution(g, board, coloring, constraints, next);
        }
    }
    board.set_to_tiled(root);
}

bool solve_tree_from_root(ColorGeneration g, Board& board, Coloring& coloring, coord_type root){

    std::set<Edge, EdgeLess> visited;
    std::map<Edge, Constraint, EdgeLess> constraints;

    std::array<Edge, 4> to_visit {{top(root), left(root), bottom(root), right(root)}};
    std::array<Constraint, 4> prop_constraints;
    for(size_t index = 0; index < 4 ; ++index) {
        int c = get_color(coloring, to_visit[index]);
        if (c > -1){
            prop_constraints[index] = Constraint::strict(c);
        } else {
            visited.insert(to_visit[index]);
            auto next = first(to_visit[index]);
            if (next == root)
                next = second(to_visit[index]);
            prop_constraints[index] = propagate_constraint_from_leaves(board, coloring, visited, constraints, next);
            constraints[to_visit[index]] = prop_constraints[index];
        }
    }

    if (compatible(prop_constraints[0], propagate(prop_constraints[1], prop_constraints[3], prop_constraints[2]))) {
        propagate_solution(g, board, coloring, constraints, root);
        return true;
    } else
        return false;
}