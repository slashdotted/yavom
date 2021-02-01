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
#include <sstream>


#define TK(v) (v+max)

struct Point {
    unsigned int x;
    unsigned int y;

    bool operator!=(const Point& b) const noexcept
    {
        return x != b.x || y != b.y;
    }

    bool operator==(const Point& b) const noexcept
    {
        return !operator!=(b);
    }

    bool is_null() const noexcept
    {
        return x == 0 && y == 0;
    }

    Point offset(int dx, int dy) const noexcept
    {
        return {x+dx, y+dy};
    }
};


std::ostream& operator<<(std::ostream& o, const Point& p)
{
    o << '(' << p.x << ',' << p.y << ')';
    return o;
}


enum class OP {NOP, INSERT, DELETE};

struct Patch {
    OP m_op;
    std::string m_value;
    unsigned int m_start;
    unsigned int m_end;

    void apply(std::vector<std::string>& a) const
    {
        switch(m_op) {
        case OP::DELETE:
            for (auto i{m_start}; i< m_end; ++i) {
                a.erase(a.begin() + i);
            }
            break;
        case OP::INSERT:
            a.insert(a.begin() + m_start, m_value);
            break;
        default:
            break;
        }
    }
};

struct Move {
    Move(OP op, const Point& s, const Point& t)
        : m_op{op}, m_s{s}, m_t{t}
    {

    }

    bool nop() const
    {
        return m_s.x == m_t.x && m_s.y == m_t.y;
    }

    void write_op(std::ostream& o, const std::vector<std::string>& b, int& offset) const
    {
        switch(m_op) {
        case OP::DELETE:
            o << m_s << "->" << m_t << " ";
            o << offset + m_s.x << '-';
            if ((m_t.x - m_s.x) > 1) {
                o << offset + m_t.x;
            }
            o << '\n';
            offset -= (m_t.x - m_s.x);
            break;
        case OP::INSERT:
            o << m_s << "->" << m_t << " ";
            o << offset + m_s.x << '+' << b.at(m_s.y);
            o << '\n';
            offset += 1;
            break;
        default:
            break;
        }
    }

    Patch make_patch(const std::vector<std::string>& b, int& offset) const
    {
        switch(m_op) {
        case OP::DELETE: {
            auto o = offset;
            offset -= (m_t.x - m_s.x);
            return Patch{OP::DELETE, "", o + m_s.x, o + m_t.x};
        }
        case OP::INSERT: {
            auto o = offset;
            offset += 1;
            return Patch{OP::INSERT, b.at(m_s.y), o + m_s.x, 0};
        }
        default:
            break;
        }
        return Patch{OP::NOP, "", m_s.x, m_t.x};
    }


private:
    OP m_op;
    Point m_s;
    Point m_t;
};

void print_point(std::ostream& o, const Point& p)
{
    const auto& [x,y] = p;
    o << '(' << x << ',' << y << ')';
}

struct Area {
    Area(const std::vector<std::string>& a, const std::vector<std::string>& b)
        : m_a{a}, m_b{b}, m_tl{0,0}, m_br{static_cast<unsigned int>(a.size()), static_cast<unsigned int>(b.size())}
    {
        trim();
    }

    Area(const Area& base, const Point& tl, const Point& br)
        : m_a{base.m_a}, m_b{base.m_b}, m_tl{tl}, m_br{br}
    {
        trim();
    }

    const std::string& a(unsigned int index) const noexcept
    {
        return m_a[m_tl.x + index];
    }

    const std::string& b(unsigned int index) const noexcept
    {
        return m_b[m_tl.y + index];
    }

    const std::string& ra(unsigned int index) const noexcept
    {
        return m_a[m_br.x -1 - index];
    }

    const std::string& rb(unsigned int index) const noexcept
    {
        return m_b[m_br.y -1 - index];
    }

    int N() const noexcept
    {
        return m_br.x - m_tl.x;
    }

    int M() const noexcept
    {
        return m_br.y - m_tl.y;
    }

    Point abs_point_r(const Point& p) const noexcept
    {
        return  {m_tl.x + N() - p.x, m_tl.y + M() - p.y};
    }

    Point abs_point(const Point& p) const noexcept
    {
        return  {m_tl.x + p.x, m_tl.y + p.y};
    }

    Point point_r(const Point& p) const noexcept
    {
        return  {N() - p.x, M() - p.y};
    }


    int rdiagonal(int k) const noexcept
    {
        return (N()-M())-k;
    }

    bool contains_abs(const Point& p) const noexcept
    {
        return p.x >= m_tl.x && p.x <= m_br.x && p.y >= m_tl.y && p.y <= m_br.y;
    }

    bool abs_contains(const Point& p) const noexcept
    {
        return p.x >= 0 && p.x <= N() && p.y >= 0 && p.y <= M();
    }

    const Point& tl() const noexcept
    {
        return m_tl;
    }

    const Point& br() const noexcept
    {
        return m_br;
    }

private:
    void trim()
    {
        while(m_tl.x < m_br.x && m_tl.y < m_br.y && m_a[m_tl.x] == m_b[m_tl.y]) {
            ++m_tl.x;
            ++m_tl.y;
        }
        while(m_br.x > m_tl.x && m_br.y > m_tl.y && m_a[m_br.x-1] == m_b[m_br.y-1]) {
            --m_br.x;
            --m_br.y;
        }
    }

    const std::vector<std::string>& m_a;
    const std::vector<std::string>& m_b;
    Point m_tl{0,0};
    Point m_br{0,0};
};




#define TK(v) (v+max)
std::tuple<Point,Point> myers_middle_move(const Area& area)
{
    int max{area.M() + area.N()};

    std::vector<unsigned int> V_fwd;
    V_fwd.resize(2*max+1);
    V_fwd[1] = 0;
    int x_fwd{0},y_fwd{0};

    std::vector<unsigned int> V_bwd;
    V_bwd.resize(2*max+1);
    V_bwd[1] = 0;
    int x_bwd{0},y_bwd{0};


    for (int d {0}; d <= max; ++d) {
        auto min_valid_k = -d + std::max(0, d-area.M()) * 2;
        auto max_valid_k = d - std::max(0, d-area.N()) * 2 ;
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
            while ((x_fwd < area.N()) && (y_fwd < area.M()) && (area.a(x_fwd) == area.b(y_fwd))) {
                ++x_fwd;
                ++y_fwd;
            }
            // Store best x position on this diagonal
            V_fwd[TK(k)] = x_fwd;
            if (x_fwd >= area.N() and y_fwd >= area.M()) {
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
            while ((x_bwd < area.N()) && (y_bwd < area.M()) && (area.ra(x_bwd) == area.rb(y_bwd))) {
                ++x_bwd;
                ++y_bwd;
            }
            // Store best position on this diagonal
            V_bwd[TK(k)] = x_bwd;
            if (x_bwd >= area.N() and y_bwd >= area.M()) {
                at_dest = true;
                break;
            }
        }

        /*  if (true) {
              std::cerr << "d=" << d << '\n';
              std::cerr << "Current points so far... forward:";
              for (int k = min_valid_k; k<= max_valid_k; k+=1) {
                  auto x = V_fwd[TK(k)];
                  auto y = x -k;
                  Point rp = area.abs_point({x,y});
                  std::cerr <<  "(" << rp.x << ',' << rp.y << "), "  ;
              }

              std::cerr << "\nCurrent points so far... backward:";
              for (int k = min_valid_k; k<= max_valid_k; k+=1) {
                  auto x = V_bwd[TK(k)];
                  auto y = x -k;
                  Point rp = area.abs_point_r({x,y});
                  std::cerr << "(" << rp.x << ',' << rp.y << "), " ;
              }
              std::cerr << '\n';
              std::string s;
              std::getline(std::cin, s);
          }*/





        // Compare V_fwd and V_bwd (if the sum on each diagonal is > N we met somewhere)
        if (d >= abs(area.N() - area.M())) {
            for (int k = min_valid_k; k<= max_valid_k; k+=1) {
                const auto rk = area.rdiagonal(k);
                const auto& bxf = V_fwd[TK(k)];
                auto byf = bxf - k;
                const auto& bxb = V_bwd[TK(rk)];
                auto byb = bxb - rk;
                Point abfw = area.abs_point({bxf,byf});
                Point abbw = area.abs_point_r({bxb, byb});

                if (abfw.x >= abbw.x) {
                    //std::cerr << "d=" << d << " on forward diagonal " << k << " forward went to " << abfw << " while backward went to " << abbw <<'\n';
                    if (area.contains_abs(abfw) && area.contains_abs(abbw)) {
                        // std::cerr << "Found!\n" << abbw << " to " << abfw << '\n';
                        return {abbw, abfw};
                    }
                }
            }
        }
        if (at_dest) {
            break;
        }
    }
    assert(false); // This can't be
}

std::list<Move> myers(const Area& area)
{
    std::list<Move> result;

    if (area.N() == 0) {
        //std::cerr << "Special case: from " << area.tl() << " to " << area.br() << " N=0 (insertions only)\n";
        Point next{area.tl()};
        for (unsigned int y{0}; y < area.M(); ++y) {
            next = next.offset(0,1);
        }
        result.insert(result.begin(), Move{OP::INSERT, area.tl(), next});
    }
    else if (area.M() == 0) {
        //std::cerr << "Special case: from " << area.tl() << " to " << area.br() << " M=0 (deletions only)\n";
        Point next{area.tl()};
        for (unsigned int x{0}; x < area.N(); ++x) {
            next = next.offset(1,0);
        }
        result.insert(result.begin(), Move{OP::DELETE, area.tl(), next});
    }
    else {
        //std::cerr << "Area from " << area.tl() << " to " << area.br() << '\n';
        auto middle = myers_middle_move(area);
        const auto& [top,bottom] = middle;
        if (!top.is_null() || !bottom.is_null()) {
            //std::cerr << "Middle points " << top << " -> " << bottom << '\n';
            //std::cerr << "\tProcessing top from " << area.tl() << " to " << top << '\n';
            //std::cerr << "\tProcessing bottom from " << bottom << " to " << area.br() << '\n';
            auto top_area = myers(Area{area, area.tl(), top});
            auto bottom_area = myers(Area{area, bottom, area.br()});
            result.insert(result.begin(), top_area.begin(), top_area.end());
            result.insert(result.end(), bottom_area.begin(), bottom_area.end());
        }
        else {
            assert(false);
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
    if (false) {
        auto a = readFile("/home/attila/before.json");
        std::cerr << "Read " << a.size() << " lines\n";
        auto b = readFile("/home/attila/after.json");
        std::cerr << "Read " << b.size() << " lines\n";
        auto s= myers(Area{a, b});

        int offset = 1;
        std::vector<Patch> patches;
        for (const auto& p : s) {
            patches.push_back(p.make_patch(b, offset));
        }
        for (const auto& p : patches) {
            p.apply(a);
        }
    }
    else {
        auto a = readFile("/home/attila/primo.txt");
        std::cerr << "Read " << a.size() << " lines\n";
        for (const auto& c : a) {
            std::cerr << c << '\n';
        }
        auto b = readFile("/home/attila/secondo.txt");
        std::cerr << "Read " << b.size() << " lines\n";
        for (const auto& c : b) {
            std::cerr << c << '\n';
        }
        auto s= myers(Area{a, b});

        int offset = 0;
        std::vector<Patch> patches;
        for (const auto& p : s) {
            patches.push_back(p.make_patch(b, offset));
        }
        std::cerr << "Applying patches:\n";
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "Result after patching:\n";
        for (const auto& c : a) {
            std::cerr << c << '\n';
        }
    }
}
