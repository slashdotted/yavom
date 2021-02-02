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
#define LINE_OFFSET 0

struct Point {
    long x;
    long y;

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

    Point offset(long dx, long dy) const noexcept
    {
        return {x+dx, y+dy};
    }
};


std::ostream& operator<<(std::ostream& o, const Point& p)
{
    o << '(' << p.x << ',' << p.y << ')';
    return o;
}


enum class OP {/*NOP, */INSERT, DELETE};

struct Patch {
    OP m_op;
    std::string m_value;
    long m_start;
    long m_count;

    void apply(std::vector<std::string>& a) const
    {
        switch(m_op) {
        case OP::DELETE:
            a.erase(a.begin() + m_start, a.begin() + m_start + m_count);
            break;
        case OP::INSERT:
            a.insert(a.begin() + m_start, m_value);
            break;
        default:
            break;
        }
    }

    void write(std::ostream& o) const
    {
        switch(m_op) {
        case OP::DELETE:
            if (m_count > 1) {
                o << m_start << '-' << m_count;
            }
            else {
                o << m_start << '-';
            }
            break;
        case OP::INSERT:
            o << m_start << '+' << m_value;
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

    Patch make_patch(const std::vector<std::string>& b, long& offset) const
    {
        switch(m_op) {
        case OP::DELETE: {
            assert(m_t.y == m_s.y);
            auto o = offset;
            auto count = (m_t.x - m_s.x);
            offset -= count;
            //std::cerr << "DELETION from " << m_s.x << " to " << m_t.x << " count=" << count << " adjusted " << o + m_s.x << '\n';
            //std::cerr << "Offset is now "<< offset << '\n';
            return Patch{OP::DELETE, "", o + m_s.x, count};
        }
        case OP::INSERT: {
            assert(m_t.x == m_s.x);
            auto o = offset;
            offset += 1;
            //std::cerr << "Offset is now "<< offset << '\n';
            return Patch{OP::INSERT, b.at(m_s.y), o + m_s.x, 0};
        }
        }
        assert(false);
    }

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
        : m_a{a}, m_b{b}, m_tl{0,0}, m_br{static_cast<long>(a.size()), static_cast<long>(b.size())}
    {
        trim();
    }

    /**
     * @brief Constructs a sub-area
     * @param base
     * @param tl
     * @param br
     */
    Area(const Area& base, const Point& tl, const Point& br)
        : m_a{base.m_a}, m_b{base.m_b}, m_tl{tl}, m_br{br}
    {
        assert(contains_abs(tl));
        assert(contains_abs(br));
        trim();
    }

    const std::string& a(long index) const noexcept
    {
        return m_a[m_tl.x + index];
    }

    const std::string& b(long index) const noexcept
    {
        return m_b[m_tl.y + index];
    }

    const std::string& ra(long index) const noexcept
    {
        return m_a[m_br.x -1 - index];
    }

    const std::string& rb(long index) const noexcept
    {
        return m_b[m_br.y -1 - index];
    }

    long N() const noexcept
    {
        assert(m_br.x >= m_tl.x);
        return m_br.x - m_tl.x;
    }

    long M() const noexcept
    {
        assert(m_br.y >= m_tl.y);
        return m_br.y - m_tl.y;
    }

    Point abs_point_r(long rel_x, long rel_y) const noexcept
    {
        return  {m_tl.x + N() - rel_x, m_tl.y + M() - rel_y};
    }

    Point abs_point(long rel_x, long rel_y) const noexcept
    {
        return  {m_tl.x + rel_x, m_tl.y + rel_y};
    }

    Point point_r(const Point& p) const noexcept
    {
        return  {N() - p.x, M() - p.y};
    }

    long rdiagonal(long k) const noexcept
    {
        return (-k + (N() - M()));
    }

    bool contains_abs(const Point& p) const noexcept
    {
        return p.x >= m_tl.x && p.x <= m_br.x && p.y >= m_tl.y && p.y <= m_br.y;
    }

    const Point& tl() const noexcept
    {
        return m_tl;
    }

    const Point& br() const noexcept
    {
        return m_br;
    }

    const std::vector<std::string>& a() const noexcept
    {
        return m_a;
    }
    const std::vector<std::string>& b() const noexcept
    {
        return m_b;
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

bool operator==(const Area& a, const Area b)
{
    return a.tl() == b.tl() && a.br() == b.br();
}


bool operator!=(const Area& a, const Area b)
{
    return !(a==b);
}

#define TK(v) (v+max)
/**
 * @brief Determines the middle diagonal (which can be of length >= 0)
 * @param area  the search area
 * @return      the absolute start and end points of the middle diagonal
 */
std::tuple<Point,Point> myers_middle_move(const Area& area)
{
    auto max{area.M() + area.N()};
    std::vector<long> V_fwd{static_cast<unsigned int>(2*max+1), 0};
    V_fwd.resize(2*max+1);
    V_fwd[1] = 0;
    long x_fwd{0},y_fwd{0};

    std::vector<long> V_bwd{static_cast<unsigned int>(2*max+1), 0};
    V_bwd.resize(2*max+1);
    V_bwd[1] = 0;
    long x_bwd{0},y_bwd{0};


    for (int d {0}; d <= static_cast<long>(max); ++d) {
        auto min_valid_k = -d + std::max(0l, d-static_cast<long>(area.M())) * 2;
        auto max_valid_k = d - std::max(0l, d-static_cast<long>(area.N())) * 2 ;
        // Forward step
        bool at_dest{false};
        for (long k = min_valid_k; k<= max_valid_k; k+=2) {
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
        for (long k = min_valid_k; k<= max_valid_k; k+=2) {
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

        for (long k = min_valid_k; k<= max_valid_k; k+=1) {
            const auto rk = area.rdiagonal(k);
            const auto& bxf = V_fwd[TK(k)];
            auto byf = bxf - k;
            const auto& bxb = V_bwd[TK(rk)];
            auto byb = bxb - rk;
            Point abfw = area.abs_point(bxf,byf);
            Point abbw = area.abs_point_r(bxb, byb);
            if (abfw.x >= abbw.x) {
                if (area.contains_abs(abfw) && area.contains_abs(abbw)) {
                    return {abbw, abfw};
                }
            }
        }

        if (at_dest) {
            break;
        }
    }
    assert(false); // This can't be
}

std::vector<Move> myers_moves(const Area& area)
{
    //std::cerr << "Processing area " << area.tl() << ',' << area.br() << '\n';
    std::vector<Move> result;
    if (area.N() == 0 && area.M() == 0) {
        return {};
    }
    if (area.N() == 0) {
        Point prev{area.tl()};
        //std::cerr << "\t\tINSERT RUN " << area.tl() << " to " << area.br() << '\n';
        for (long y{0}; y < area.M(); ++y) {
            auto next = prev.offset(0,1);
            result.push_back(Move{OP::INSERT, prev, next});
            prev = next;
        }
    }
    else if (area.M() == 0) {
        //std::cerr << "\t\tDELETE RUN " << area.tl() << " to " << area.br() << '\n';
        //std::cerr << area.a()[area.tl().x] << '\n' << area.b()[area.tl().y] << '\n';
        result.push_back(Move{OP::DELETE, area.tl(), area.br()});
    }
    else {
        auto middle = myers_middle_move(area);
        const auto& [top,bottom] = middle;
        if (!top.is_null() || !bottom.is_null()) {
            //std::cerr << "\tProcessing TOP area " << area.tl() << ',' << top << '\n';
            auto top_area = myers_moves(Area{area, area.tl(), top});
            result.insert(result.end(), top_area.begin(), top_area.end());
            //std::cerr << "\tProcessing BOTTOM area " << bottom << ',' << area.br() << '\n';
            auto bottom_area = myers_moves(Area{area, bottom, area.br()});
            result.insert(result.end(), bottom_area.begin(), bottom_area.end());
        }
        else {
            assert(false);
        }
    }
    return result;
}

std::vector<Patch> basic_myers(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    auto s= myers_moves({a,b});
    long offset = LINE_OFFSET;
    std::vector<Patch> patches;
    patches.reserve(s.size());
    std::transform(s.begin(), s.end(), std::back_insert_iterator(patches), [&offset,&b](const auto& p) {
        //std::cerr << p.m_s << " -> " << p.m_t << '\n';
        return p.make_patch(b, offset);
    });
    return patches;
}

std::vector<Patch> myers(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    auto s= myers_moves({a,b});
    std::vector<Patch> patches;
    if (!s.empty()) {
        long offset = LINE_OFFSET;
        std::vector<Move> optimized;
        // Optimize deletion
        Move* last_deletion{nullptr};
        for (size_t i{0}; i<s.size(); ++i) {
            auto& p = s[i];
            if (p.m_op == OP::DELETE) {
                if (!last_deletion) {
                    last_deletion = &p;
                }
                else {
                    if (p.m_s == last_deletion->m_t && p.m_s.y == p.m_t.y) {
                        last_deletion->m_t = p.m_t;
                    }
                    else {
                        optimized.push_back(*last_deletion);
                        last_deletion = &p;
                    }
                }
            }
            else {
                if (last_deletion) {
                    optimized.push_back(*last_deletion);
                    last_deletion = nullptr;
                }
                optimized.push_back(p);
            }
        }
        if (last_deletion) {
            optimized.push_back(*last_deletion);
            last_deletion = nullptr;
        }
        patches.reserve(optimized.size());
        for(const auto& p : optimized) {
            auto tp = p.make_patch(b, offset);
            patches.push_back(tp);
        }
    }
    return patches;
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

bool compare(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    if (a.size() != b.size()) {
        std::cerr << "a.size()=" << a.size() << " b.size()=" << b.size() << '\n';
        return false;
    }
    else {
        std::cerr << "a.size()=" << a.size() << " b.size()=" << b.size() << "...OK!" << '\n';
    }
    bool r{true};

    for (auto i{0u}; i<a.size(); ++i) {
        if (a[i] != b[i]) {
            std::cerr << "Line " << i << " differs:";
            std::cerr << "\tA:" << a[i] << '\n';
            std::cerr << "\tB:" << b[i] << '\n';
            r= false;
        }
    }
    return r;
}


int main()
{
    {
        auto a = readFile("/home/attila/before.json");
        auto b = readFile("/home/attila/after.json");
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.write(std::cerr);
            std::cerr << '\n';
            p.apply(a);
        }
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
        /*for (const auto& l : a) {
            std::cout<< l << '\n';
        }*/
        exit(0);
    }
    /* {
         auto a = readFile("/home/attila/before.json");
         auto b = readFile("/home/attila/after.json");
         std::cerr << "Strip prefix\n";
         int idx{0};
         while(a[idx] == b[idx]) {
             ++idx;
         }
         a.erase(a.begin(), a.begin()+ idx);
         b.erase(b.begin(), b.begin()+ idx);
         std::cerr <<  "Strip suffix\n";
         auto a_idx{a.size()-1};
         auto b_idx{b.size()-1};
         while(a[a_idx] == b[b_idx]) {
             --a_idx;
             --b_idx;
         }
         a.erase(a.begin()+a_idx+1, a.end());
         b.erase(b.begin()+b_idx+1, b.end());
         std::cerr << "Diffing\n";
         std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
         auto patches = myers(a,b);
         for (const auto& p : patches) {
             p.apply(a);
         }
         std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
     }*/
    {
        auto a = readFile("/home/attila/x.txt");
        auto b = readFile("/home/attila/y.txt");
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        auto patches = myers(a,b);
        /*for (const auto& p : patches) {
            p.write(std::cerr);
            std::cerr << '\n';
        };*/
        for (const auto& p : patches) {

            /*std::cout << "****** Before:\n";
            {
                int ln{0};
                for (const auto& c : a) {
                    std::cout << ln++ << ": " <<  c << '\n';
                }
            }
            std::cout << "****** Patch:";
            p.write(std::cout);
            std::cout << '\n';
            */
            p.apply(a);
            /*
                        {
                            int ln{0};
                            for (const auto& c : a) {
                                std::cout << ln++ << ": " <<  c << '\n';
                            }
                        }
                        // std::cout << "******";
                        // std::string wait;
                        //std::getline(std::cin, wait);
              */
        }
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
        //      for (const auto& c : a) {
        //        std::cout << c << '\n';
        //   }
        //exit(0);
    }
    {
        std::cerr << "************ N < M ************\n";
        auto a = readFile("/home/attila/secondo.txt");
        auto b = readFile("/home/attila/primo.txt");
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        std::cerr << "Result before patching: ";
        for (const auto& c : b) {
            std::cerr << c;
        }
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "\nResult after patching:  ";
        for (const auto& c : a) {
            std::cout << c;
        }
        std::cerr << '\n';
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
    }

    {
        std::cerr << "************ N > M ************\n";
        auto a = readFile("/home/attila/primo.txt");
        auto b = readFile("/home/attila/secondo.txt");
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        std::cerr << "Result before patching: ";
        for (const auto& c : b) {
            std::cerr << c;
        }
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "\nResult after patching:  ";
        for (const auto& c : a) {
            std::cout << c;
        }
        std::cerr << '\n';
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
    }
    {
        std::cerr << "************ N == M ************\n";
        auto a = readFile("/home/attila/primo.txt");
        auto b = readFile("/home/attila/terzo.txt");
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        std::cerr << "Result before patching: ";
        for (const auto& c : b) {
            std::cerr << c;
        }
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "\nResult after patching:  ";
        for (const auto& c : a) {
            std::cout << c;
        }
        std::cerr << '\n';
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
    }
    {
        std::cerr << "************ N === M ************\n";
        auto a = readFile("/home/attila/secondo.txt");
        auto b = readFile("/home/attila/secondo.txt");
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        std::cerr << "Result before patching: ";
        for (const auto& c : b) {
            std::cerr << c;
        }
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "\nResult after patching:  ";
        for (const auto& c : a) {
            std::cout << c;
        }
        std::cerr << '\n';
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
    }
    {
        std::cerr << "************ N === empty ************\n";
        auto a = readFile("/home/attila/secondo.txt");
        auto b = std::vector<std::string>();
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        std::cerr << "Result before patching: ";
        for (const auto& c : b) {
            std::cerr << c;
        }
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "\nResult after patching:  ";
        for (const auto& c : a) {
            std::cout << c;
        }
        std::cerr << '\n';
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
    }
    {
        std::cerr << "************ empty === M ************\n";
        auto a = std::vector<std::string>();
        auto b = readFile("/home/attila/secondo.txt");
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        std::cerr << "Result before patching: ";
        for (const auto& c : b) {
            std::cerr << c;
        }
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "\nResult after patching:  ";

        std::cerr << '\n';
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
    }
    {
        std::cerr << "************ empty == empty ************\n";
        auto a = std::vector<std::string>();
        auto b = std::vector<std::string>();
        std::cerr << "Before: equals? " << std::boolalpha << compare(a,b) << '\n';
        std::cerr << "Result before patching: ";
        for (const auto& c : b) {
            std::cerr << c;
        }
        auto patches = myers(a,b);
        for (const auto& p : patches) {
            p.apply(a);
        }
        std::cerr << "\nResult after patching:  ";
        for (const auto& c : a) {
            std::cout << c;
        }
        std::cerr << '\n';
        std::cerr << "After: equals? " << std::boolalpha << compare(a,b) << '\n';
    }
}
