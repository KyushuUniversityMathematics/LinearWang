#ifndef LINEARWANG_WANG_H
#define LINEARWANG_WANG_H

#include <random>
#include <array>

typedef std::array<int, 4> tile;

class ColorGeneration {
public:
    ColorGeneration(unsigned seed, int bound): rng(new std::mt19937(seed)), b(bound){}

    inline int bound() const { return b; }

    inline int pick_color() {
        return std::uniform_int_distribution<int>(0, bound()-1)(*rng.get());
    }

    inline int pick_different_color(int c) {
        int r = std::uniform_int_distribution<int>(0, bound()-2)(*rng.get());
        if (r < c)
            return r;
        return r+1;
    }

    inline int pick_different_color(int c1, int c2) {
        if (c1 == c2)
            return pick_different_color(c1);
        if (c2 < c1)
            std::swap(c1, c2);
        int r = std::uniform_int_distribution<int>(0, bound()-3)(*rng.get());
        if (r < c1)
            return r;
        r += 1;
        if (r < c2)
            return r;
        return r+1;
    }

    void complete_tile(tile& t);

    double get() {
        return std::uniform_real_distribution<>(0,1)(*rng.get());
    }

private:
    std::shared_ptr<std::mt19937>(rng);
    int b;
};

#endif //LINEARWANG_WANG_H
