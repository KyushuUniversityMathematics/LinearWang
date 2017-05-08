#include "wang.h"

void ColorGeneration::complete_tile(tile& t){
    int a = 0, b=1, c=2, d=3;
    if (t[a] == -1) std::swap(a, c);
    if (t[b] == -1) std::swap(b, d);

    if (t[a] == -1 && t[b] > -1) { std::swap(a, b); std::swap(c,d); }

    if (t[b] == -1 && t[c] > -1) {
        t[b] = pick_color();
        if (t[a] == t[c])
            t[d] = pick_different_color(t[b]);
        else
            t[d] = t[b];
        return;
    }

    if (t[c] == -1 && t[d] > -1) { std::swap(a, b); std::swap(c,d); }


    if (t[a] == -1) t[a] = pick_color();
    if (t[b] == -1) t[b] = pick_color();

    if (t[c] == -1) {
        if(std::uniform_real_distribution<>(0,1)(*rng.get()) > 0.5)
            t[c] = t[a];
        else {
            t[c] = pick_different_color(t[a]);
        }
    }

    if (t[d] == -1) {
        if (t[a] != t[c])
            t[d] = t[b];
        else {
            t[d] = pick_different_color(t[b]);
        }
    }
}