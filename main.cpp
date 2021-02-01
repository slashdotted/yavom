#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <set>
#include <list>
#include <cassert>


#define TK(v) (v+max)

struct Point {
    unsigned int x;
    unsigned int y;
};

bool operator!=(const Point& a, const Point& b)
{
    return a.x != b.x || a.y != b.y;
}

bool operator==(const Point& a, const Point& b)
{
    return a.x == b.x && a.y == b.y;
}

struct Snake {
    Point s;
    Point t;

    bool empty()
    {
        return s.x == t.x && s.y == t.y;
    }
};

void print_point(std::ostream& o, const Point& p)
{
    const auto& [x,y] = p;
    o << '(' << x << ',' << y << ')';
}

bool print_snake(std::ostream& o, const Snake& s)
{
    const auto& [from, to] = s;
    if (from != to) {
        print_point(o, from);
        o << "->";
        print_point(o, to);
        return true;
    }
    return false;
}

#define TK(v) (v+max)
template<typename T>
std::tuple<Point,Point> myers_middle_snake(const Point& origin,
        const Point& limit,
        const T& a,
        const T&  b)
{
    std::cerr << "Finding meeting point in box ("
              << origin.x << ',' << origin.y << ")(" << limit.x <<',' << limit.y << ")\n";
    int M = limit.y - origin.y;
    int N = limit.x - origin.x;

    int max = M+N;

    std::vector<int> V_fwd;
    V_fwd.resize(2*max+1);
    V_fwd[1] = 0;
    int x_fwd{0},y_fwd{0};
    const auto& fwd_it_a = a.cbegin() + origin.x;
    const auto& fwd_it_b = b.cbegin() + origin.y;

    std::vector<int> V_bwd;
    V_bwd.resize(2*max+1);
    V_bwd[1] = 0;
    int x_bwd{0},y_bwd{0};
    auto bwd_it_a = std::make_reverse_iterator(a.cbegin() + limit.x);
    auto bwd_it_b = std::make_reverse_iterator(b.cbegin() + limit.y);

    for (int d {0}; d<= max; ++d) {
        auto min_valid_k = -d + std::max(0, d-M) * 2;
        auto max_valid_k = d - std::max(0, d-N) * 2 ;
        // Forward step
        bool at_dest{false};
        for (int k = min_valid_k; k<= max_valid_k; k+=2) {
            // Move downward or to the right
            if (k == -d || ((k != d )&& (V_fwd[TK(k-1)] < V_fwd[TK(k+1)]))) {
                x_fwd = V_fwd[TK(k+1)];
            }
            else {
                x_fwd = V_fwd[TK(k-1)] + 1;
            }
            y_fwd = x_fwd - k;
            // Follow diagonal as long as possible
            while ((x_fwd < N) && (y_fwd < M) && (*(fwd_it_a+x_fwd) == *(fwd_it_b+y_fwd))) {
                ++x_fwd;
                ++y_fwd;
            }
            // Store best x position on this diagonal
            V_fwd[TK(k)] = x_fwd;
            if (x_fwd >= N and y_fwd >= M) {
                at_dest = true;
                break;
            }
        }

        // Backward step
        for (int k = min_valid_k; k<= max_valid_k; k+=2) {
            // Move downward or to the right
            if (k == -d || ((k != d )&& (V_bwd[TK(k-1)] < V_bwd[TK(k+1)]))) {
                x_bwd = V_bwd[TK(k+1)];
            }
            else {
                x_bwd = V_bwd[TK(k-1)] + 1;
            }
            y_bwd = x_bwd - k;
            // Follow diagonal as long as possible
            while ((x_bwd < N) && (y_bwd < M) && (*(bwd_it_a+x_bwd) == *(bwd_it_b+y_bwd))) {
                ++x_bwd;
                ++y_bwd;
            }
            // Store best position on this diagonal
            V_bwd[TK(k)] = x_bwd;
            if (x_bwd >= N and y_bwd >= M) {
                at_dest = true;
                break;
            }
        }

        if (true) {
            std::cerr << "d=" << d << '\n';
            std::cerr << "Current points so far... forward:";
            for (int k = min_valid_k; k<= max_valid_k; k+=1) {
                auto x = V_fwd[TK(k)];
                auto y = x -k;
                std::cerr << "(" << x << ',' << y << "), " ;
            }

            std::cerr << "\nCurrent points so far... backward:";
            for (int k = min_valid_k; k<= max_valid_k; k+=1) {
                auto x = V_bwd[TK(k)];
                auto y = x -k;
                std::cerr << "(" << N-x << ',' << M-y << "), " ;
            }
            std::cerr << '\n';
            std::string s;
            std::getline(std::cin, s);
        }





        // Compare V_fwd and V_bwd (if the sum on each diagonal is > N we met somewhere)
        if (d >= abs(N-M)) {
            for (int k = min_valid_k; k<= max_valid_k; k+=1) {
                auto k_bwd = k + (N-M); // match forward and backward diagonal
                const auto& current_x_fwd =V_fwd[TK(k)];
                const auto& current_x_bwd = V_bwd[TK(k_bwd)];
                if (current_x_fwd >= (N - current_x_bwd)) {
                    std::cerr << "d=" << d << " on forward diagonal " << k << " forward went to " << current_x_bwd << " while backward went to " << current_x_bwd <<'\n';
                    // X points on the diagonal met or crossed
                    // The end points can be used as midpoints
                    unsigned int s_x_fwd = current_x_fwd;
                    unsigned int s_y_fwd = s_x_fwd - k;
                    unsigned int s_x_bwd = N - current_x_bwd;
                    unsigned int s_y_bwd = M - (current_x_bwd - k_bwd);
                    if ((s_x_fwd <= limit.x && s_y_fwd <= limit.y
                            && s_x_fwd >= origin.x && s_y_fwd >= origin.y)
                            && (s_x_bwd <= limit.x && s_y_bwd <= limit.y
                                && s_x_bwd >= origin.x && s_y_bwd >= origin.y)) {
                        std::cerr << "Found!\n";
                        return {{s_x_bwd, s_y_bwd}, {s_x_fwd, s_y_fwd}};
                    }
                    else {
                        std::cerr << "(" << s_x_bwd << "," << s_y_bwd << ") - (" << s_x_fwd <<',' << s_y_fwd << ") is outside box ("
                                  << origin.x << ',' << origin.y << ")(" << limit.x <<',' << limit.y << ")\n";
                    }
                }
            }
        }
        if (at_dest) {
            break;
        }
    }
    std::cerr << "No middle meeting\n";
    return {{0,0},{0,0}};
}

template<typename T>
std::list<Snake> myers(const Point& o, const Point& l, const T& a, const T&  b)
{
    std::list<Snake> result;
    if (l == o) {
        return {};
    }
    Point origin = o;
    Point limit = l;

    // Consume same prefix
    while (*(a.cbegin()+origin.x) == *(b.cbegin()+origin.y)) {
        //std::cerr << "Value " << *(a.cbegin()+origin.x) << "==" << *(b.cbegin()+origin.y) << '\n';
        ++origin.x;
        ++origin.y;
    }
    assert(*(a.cbegin()+origin.x) != *(b.cbegin()+origin.y));

    // Consume same suffix
    while (*(a.cbegin()+limit.x-1) == *(b.cbegin()+limit.y-1)) {
        //std::cerr << "Value 2: " << *(a.cbegin()+limit.x-1) << "==" << *(b.cbegin()+limit.y-1) << '\n';
        --limit.x;
        --limit.y;
    }
    assert((*(a.cbegin()+limit.x-1) != *(b.cbegin()+limit.y-1)));
    std::cerr << "Processing area from (" << origin.x << ',' << origin.y << ") to (" << limit.x << ',' << limit.y << ") " << '\n';
    if (limit.x == origin.x) {
        // Vertical movement
        std::cerr << "Just a vertical movement!\n";
        for (unsigned int y{origin.y}; y < limit.y-1; ++y) {
            result.insert(result.begin(), Snake{{origin.x,y},{origin.x,y+1}});
        }
    }
    else if (origin.y == limit.y) {
        // Horizontal movement
        std::cerr << "Just an horizontal  movement!\n";
        for (unsigned int x{origin.x}; x < limit.x-1; ++x) {
            result.insert(result.begin(), Snake{{x,origin.y},{x+1, origin.y}});
        }
    }
    else {
        auto middle = myers_middle_snake(origin, limit, a, b);
        const auto& [top,bottom] = middle;
        const auto& [tx,ty] = top;
        const auto& [bx,by] = bottom;
        std::cerr<< "Middle diagonal: " << top.x << ',' << top.y << " to " << bottom.x << ',' << bottom.y << '\n';
        if (tx != 0 || ty !=0 || bx !=0 || by !=0) {
            std::cerr << "\tProcessing top area from (" << origin.x << ',' << origin.y << ") to (" << top.x << ',' << top.y << ") " << '\n';
            std::cerr << "\tProcessing bottom area from (" << bottom.x << ',' << bottom.y << ") to (" << limit.x << ',' << limit.y << ") " << '\n';
            auto top_area = myers(origin, {tx,ty}, a, b);
            auto bottom_area = myers({bx, by}, limit, a, b);
            result.insert(result.begin(), top_area.begin(), top_area.end());
            result.insert(result.end(), bottom_area.begin(), bottom_area.end());
        }
    }
    return result;
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
    if (false ) {
        auto a = readFile("/home/attila/before.json");
        std::cerr << "Read " << a.size() << " lines\n";
        auto b = readFile("/home/attila/after.json");
        std::cerr << "Read " << b.size() << " lines\n";
        auto s= myers({0,0}, {static_cast<unsigned int>(a.size()), static_cast<unsigned int>(b.size())}, a, b);
        for (const auto& p : s) {
            print_snake(std::cerr, p);
            std::cerr << '\n';
        }
    }
    else {
        auto a = readFile("/home/attila/primo.txt");
        std::cerr << "Read " << a.size() << " lines\n";
        auto b = readFile("/home/attila/secondo.txt");
        std::cerr << "Read " << b.size() << " lines\n";
        auto s= myers({0,0}, {static_cast<unsigned int>(a.size()), static_cast<unsigned int>(b.size())}, a, b);
        for (const auto& p : s) {
            if (print_snake(std::cerr, p)) {
                std::cerr << '\n';
            }
        }
    }
}
