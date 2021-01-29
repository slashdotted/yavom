#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>


enum class OP { KEEP, DELETE, INSERT };
using Step=std::tuple<OP,unsigned int,std::string>;


#define TK(v) (v+max)

using Point=std::tuple<unsigned int, unsigned int>;
using Snake=std::tuple<Point /*A*/,
      Point /*B*/,
      Point /*C*/,
      Point /*D*/>;
void print_point(std::ostream& o, const Point& p)
{
    const auto& [x,y] = p;
    o << '(' << x << ',' << y << ')';
}

void print_snake(std::ostream& o, const Snake& s)
{
    const auto& [A, B, C, D] = s;

    print_point(o, A);
    o << "->";
    print_point(o, B);
    o << "->";
    print_point(o, C);
    o << "->";
    print_point(o, D);
    o << '\n';
}

void print_move(const Point& f, const Point& t, const std::string& b)
{
    const auto& [fx, fy] = f;
    const auto& [tx, ty] = t;

}

void print_snake(std::ostream& o, const Snake& s, const std::string& b)
{
    const auto& [A, B, C, D] = s;
    if (A != B) {
        print_move(A,B, b);
    }
    if(C != D) {
        print_move(C,D, b);
    }
}

struct SnakeLessThan : public std::binary_function<Snake, Snake, bool> {
    bool operator()(const Snake& lhs, const Snake& rhs) const
    {
        const auto& [lA, lB, lC, lD] = lhs;
        const auto& [rA, rB, rC, rD] = rhs;
        const auto& [lBx, lBy] = lB;
        const auto& [rBx, rBy] = rB;
        const auto& [lCx, lCy] = lC;
        const auto& [rCx, rCy] = rC;
        return (lBx < rBx) &&  (lBy < rBy) && (lCx < rCx) && (lCy < rCy);
    }
};


template<bool reverse>
Snake make_snake(const Point& start, const Point& middle, const Point& end, const Point& origin, int N, int M)
{
    auto translate_coordinate = [origin](const Point& p) -> Point {
        const auto& [x,y] = p;
        const auto& [ox,oy] = origin;
        return {ox+x, oy+y};
    };
    if constexpr (reverse) {
        auto reverse_coordinate = [N,M](const Point& p) -> Point {
            const auto& [x,y] = p;
            return {N-x, M-y};
        };
        auto D = translate_coordinate(reverse_coordinate(start));
        auto C = translate_coordinate(reverse_coordinate(middle));
        auto B = translate_coordinate(reverse_coordinate(end));
        return Snake{B,B,C,D};
    }
    else {
        auto A = translate_coordinate(start);
        auto B = translate_coordinate(middle);
        auto C = translate_coordinate(end);
        return Snake{A,B,C,C};
    }
}

inline bool in_area(int x, int y, int N, int M)
{
    return x <= N && y <= M;
}



/**
 * a_begin is the starting point in the initial string
 * b_begin is the starting point in the final string
 * N is the width of the box
 * M is the height of the box
 * origin is the top,left (x,y) coordinate (in normal NOT REVERSED!) of the box
 */
template<template <typename ...> typename C, typename K, typename T>
std::set<Snake,SnakeLessThan> boxed_myers_snakes(const T& a_begin,
        int N,
        const T&  b_begin,
        int M,
        const std::set<Snake,SnakeLessThan>& other_snakes = {},
        std::tuple<int,int> origin = {0,0})
{
    constexpr bool reverse{std::is_same<T,typename C<K>::reverse_iterator>::value
                           || std::is_same<T,typename C<K>::const_reverse_iterator>::value};
    std::set<Snake,SnakeLessThan> snakes;
    std::vector<int> V;
    //int max = M+N;
    int max = (M+N)/2+1;
    V.resize(2*max+1);
    V[1] = 0;
    int x{0},y{0};
    int ses{-1};
    std::cerr << "Area size " << N << 'x' << M << '\n';
#define TK(v) (v+max)
    for (int d {0}; d<= max; ++d) {
        auto min_valid_k = -d + std::max(0, d-M) * 2;
        auto max_valid_k = d - std::max(0, d-N) * 2;
        for (int k = min_valid_k; k<= max_valid_k; k+=2) {
            int px = V[TK(k)];
            int py = px -k;
            // Move downward or to the right
            if (k == -d || ((k != d )&& (V[TK(k-1)] < V[TK(k+1)]))) {
                x = V[TK(k+1)];
            }
            else {
                x = V[TK(k-1)] + 1;
            }
            y = x - k;
            int mx = x;
            int my = y;
            // Follow diagonal as long as possible
            while ((x < N) && (y < M) && (*(a_begin+x) == *(b_begin+y))) {
                ++x;
                ++y;
            }
            // Store best position on this diagonal
            V[TK(k)] = x;
            // Store snake if it's inside the search area
            if (in_area(px, py, N, M) && in_area(mx, my, N, M) && in_area(x, y, N, M)) {
                // std::cerr << "Found snake";
                auto snake = make_snake<reverse>({px, py}, {mx, my}, {x, y}, origin, N, M);
                // print_snake(std::cerr, snake);
                if constexpr (reverse) {
                    auto fwd_snake = std::find_if(other_snakes.find(snake),
                    other_snakes.end(), [snake](const auto& o) {
                        const auto& [lA, lB, lC, lD] = snake;
                        const auto& [rA, rB, rC, rD] = o;
                        return lB == rB && lC == rC;
                    });
                    if (fwd_snake != other_snakes.end()) {
                        const auto& [fA, fB, fC, fD] = *fwd_snake;
                        return {{fA, fB, fC, std::get<3>(snake)}};
                    }
                }
                else {
                    snakes.insert(snake);
                }
            }
            if ((x>=N) && (y>= M)) {
                ses = d;
                goto outside;
            }
        }
    }
outside:
    return snakes;
}

using Area=std::tuple<int /* x0 */, int /* y0 */, int /* x1 */, int /* y1 */>;

std::vector<Snake> solve_area(const std::vector<std::string>& a,
                              const std::vector<std::string>& b,
                              Area area = {-1,0,0,0})
{
    std::vector<Snake> solution;
    Snake middle;
    const auto& [x0, y0, x1, y1] = area;
    if (x0 < 0) {
        return solve_area(a, b, {0,0,a.size(), b.size()});
    }
    int N = x1-x0;
    int M = y1-y0;
    if (N== 0 && M==0) {
        return {};
    }
    else if (N == 0) {
        for (int y{y0}; y<y1; ++y) {
            solution.push_back({{x0,y},{x0,y+1},{x0,y+1},{x0,y+1}});
        }
        return solution;
    }
    else if (M == 0) {
        for (int x{x0}; x<x1; ++x) {
            solution.push_back({{x,y0},{x+1,y0},{x+1,y0},{x+1,y0}});
        }
        return solution;
    }
    else {
        std::cerr << "Forward Processing area from " << x0 << ',' << y0 << " to " << x1 << ',' << y1 << '\n';
        auto fsnakes = boxed_myers_snakes<std::vector, std::string>(
                           a.cbegin()+x0, N,
                           b.cbegin()+y0, M);
        if (fsnakes.empty()) {
            throw "No snakes in forward";
        }
        std::cerr << "Backward Processing area from " << x0 << ',' << y0 << " to " << x1 << ',' << y1 << '\n';
        auto bsnakes = boxed_myers_snakes<std::vector, std::string>(
                           std::make_reverse_iterator(a.cbegin()+x1), N,
                           std::make_reverse_iterator(b.cbegin()+y1), M, fsnakes);
        if (bsnakes.empty()) {
            throw "No matching snakes in backward";
        }
        // There should be a matching snake
        middle = *bsnakes.begin();
        std::cerr << "Middle snake: ";
        print_snake(std::cerr, middle);
    }

    const auto& [A,B,C,D] = middle;
    const auto& [tx,ty] = A;
    const auto& [bx,by] = D;
    //std::cerr << "\tDelegating bottom area from " << bx << ',' << by << " to " << x1 << ',' << y1 << '\n';
    auto bottom = solve_area(a, b, {
        bx, by, x1,y1
    });
    //std::cerr << bottom.size() << std::endl;
    solution.insert( solution.begin(), bottom.begin(), bottom.end() );
    solution.insert(solution.begin(), middle);
    //std::cerr << "\tDelegating top area from " << x0 << ',' << y0 << " to " << tx << ',' << ty << '\n';
    auto top = solve_area(a, b, {
        x0, y0, tx, ty
    });
    solution.insert( solution.begin(), top.begin(), top.end() );

    return solution;
}

// ****************************************************************************3


std::vector<std::string> readFile(const std::string& filePath)
{
    std::vector<std::string> data;
    std::ifstream inf{filePath};
    if (!inf) {
        std::cerr << "Cannot open " << filePath << '\n';
    }
    else {
        std::string line;
        while (std::getline(inf, line)) {
            data.push_back(line);
        }
    }
    return data;
}


int main()
{
    if (true ) {
        auto a = readFile("/home/attila/before.json");
        std::cerr << "Read " << a.size() << " lines\n";
        auto b = readFile("/home/attila/after.json");
        std::cerr << "Read " << b.size() << " lines\n";
        auto r = solve_area(a,b);
        std::cerr << "Solution:\n";
        for (const auto& s : r) {
            print_snake(std::cerr, s);
        }
    }
    else {
        auto a = readFile("/home/attila/primo.txt");
        std::cerr << "Read " << a.size() << " lines\n";
        auto b = readFile("/home/attila/secondo.txt");
        std::cerr << "Read " << b.size() << " lines\n";
        auto r = solve_area(a,b);
        std::cerr << "Solution:\n";
        for (const auto& s : r) {
            print_snake(std::cerr, s);
        }
    }
}
