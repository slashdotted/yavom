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

// *******************************************************************
// POINT
// *******************************************************************

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
// *******************************************************************
// MOVE
// *******************************************************************
enum class OP {FWD_TRIM_NOP, BWD_TRIM_NOP, NOP,INSERT, DELETE};
struct Move {
    Move(OP op, const Point& s, const Point& t)
        : m_op{op}, m_s{s}, m_t{t}
    {

    }

    bool apply(std::vector<std::string>& a, std::vector<std::string>& b) const
    {
        switch(m_op) {
        case OP::DELETE: {
            auto count{m_t.x - m_s.x};
            /*std::cerr << m_s.x << ',' << m_s.x + (m_t.x - m_s.x) << 'd' << m_t.y << '\n';
            for (int i{0}; i < count; ++i) {
                std::cerr << '<' << a[m_s.x+i] << '\n';
            }*/
            a.erase(a.begin() + m_s.y, a.begin() + m_s.y + count);
            return true;
        }
        case OP::INSERT: {
            auto count{m_t.y - m_s.y};
            /*std::cerr << m_s.x << 'a' << m_s.y << ',' << m_s.y + count << '\n';
            for (int i{0}; i< count; ++i) {
                std::cerr << '>' << b[m_s.y+i] << '\n';
            }*/
            a.insert(a.begin() + m_s.y, b.begin() + m_s.y, b.begin() + m_s.y + count);
            return true;
        }
        default:
            return false;
        }
    }

    OP m_op;
    Point m_s;
    Point m_t;
};

// *******************************************************************
// AREA
// *******************************************************************
struct Area {
    Area(const std::vector<std::string>& a, const std::vector<std::string>& b)
        : m_a{a}, m_b{b}, m_tl{0,0}, m_br{static_cast<long>(a.size()), static_cast<long>(b.size())}
    {
        assert(m_tl.x <= m_br.x);
        assert(m_tl.y <= m_br.y);
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
        assert(m_tl.x <= m_br.x);
        assert(m_tl.y <= m_br.y);
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

    std::vector<Move> trim_start()
    {
        std::vector<Move> moves;
        Point start{m_tl};
        while(m_tl.x < m_br.x && m_tl.y < m_br.y && m_a[m_tl.x] == m_b[m_tl.y]) {
            ++m_tl.x;
            ++m_tl.y;
        }
        if (m_tl != start) {
            moves.push_back({OP::FWD_TRIM_NOP, start, m_tl});
        }
        return moves;
    }

    std::vector<Move> trim_end()
    {
        std::vector<Move> moves;
        Point end{m_br};
        while(m_br.x > m_tl.x && m_br.y > m_tl.y && m_a[m_br.x-1] == m_b[m_br.y-1]) {
            --m_br.x;
            --m_br.y;
        }
        if (m_br != end) {
            moves.push_back({OP::BWD_TRIM_NOP, m_br, end});
        }
        return moves;
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

// *******************************************************************
// MYERS
// *******************************************************************
#define TK(v) (v+max)
/**
 * @brief Determines the middle point
 * @param area  the search area
 * @return      the absolute middle point
 */
Point myers_middle_move(const Area& area)
{
    auto max{area.M() + area.N()};
    std::vector<long> V_fwd{static_cast<unsigned int>(2*max+1), 0};
    V_fwd.resize(2*max+1);
    V_fwd[1] = 0;
    assert(V_fwd.capacity() == static_cast<unsigned long>(2*max+1));
    long x_fwd{0},y_fwd{0};

    std::vector<long> V_bwd{static_cast<unsigned int>(2*max+1), 0};
    V_bwd.resize(2*max+1);
    V_bwd[1] = 0;
    assert(V_bwd.capacity() == static_cast<unsigned long>(2*max+1));
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
        for (long k = max_valid_k; k>= min_valid_k; k-=2) {
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
                if (area.contains_abs(abfw)) {
                    return abfw;
                } else if (area.contains_abs(abbw)) {
                    return abbw;
                }
            }
        }

        if (at_dest) {
            break;
        }
    }
    assert(false); // This can't be
}

void myers_moves(Area area, std::vector<Move>& result)
{
    auto trim_start{area.trim_start()};
    auto trim_end{area.trim_end()};
    result.insert(result.end(), trim_start.begin(), trim_start.end());
    if (area.N() == 0 && area.M() == 0) {
        auto trim_end{area.trim_end()};
        return;
    } else if (area.N() == 0) {
        if (!result.empty()) {
            auto& last = result.back();
            // Merge with last insert (if possible)
            if (last.m_op == OP::INSERT && last.m_t == area.tl()) {
                last.m_t = area.br();
                result.insert(result.end(), trim_end.begin(), trim_end.end());
                return;
            }
        }
        result.push_back(Move{OP::INSERT, area.tl(), area.br()});
    }
    else if (area.M() == 0) {
        if (!result.empty()) {
            auto& last = result.back();
            // Merge with last delete (if possible)
            if (last.m_op == OP::DELETE && last.m_t == area.tl()) {
                last.m_t = area.br();
                result.insert(result.end(), trim_end.begin(), trim_end.end());
                return;
            }
        }
        result.push_back(Move{OP::DELETE, area.tl(), area.br()});
    }
    else {
        auto middle = myers_middle_move(area);
        myers_moves(Area{area, area.tl(), middle}, result);
        myers_moves(Area{area, middle, area.br()}, result);
    }
    result.insert(result.end(), trim_end.begin(), trim_end.end());
}

bool check_moves(const std::vector<Move>& s, const std::vector<std::string>& a, const std::vector<std::string>& b) {
    const Move* prev{nullptr};
    for (const auto& m : s) {
        std::string ops;
        switch(m.m_op) {
        case OP::BWD_TRIM_NOP:
            ops = "BWD_TRIM_NOP";
            break;
        case OP::FWD_TRIM_NOP:
            ops = "FWD_TRIM_NOP";
            break;
        case OP::NOP:
            ops = "NOP";
            break;
        case OP::DELETE:
            ops = "DELETE";
            break;
        case OP::INSERT:
            ops = "INSERT";
            break;
        }
        std::cerr << ops << m.m_s << " -> " << m.m_t << '\n';
        switch(m.m_op) {
        case OP::BWD_TRIM_NOP: {
            auto start = m.m_s;
            while(start != m.m_t) {
                assert(a[start.x] == b[start.y]);
                start = start.offset(1,1);
            }
            break;
        }
        case OP::FWD_TRIM_NOP: {
            auto start = m.m_s;
            while(start != m.m_t) {
                assert(a[start.x] == b[start.y]);
                start = start.offset(1,1);
            }
            break;
        }
        case OP::NOP: {
            auto start = m.m_s;
            while(start != m.m_t) {
                assert(a[start.x] == b[start.y]);
                start = start.offset(1,1);
            }
            break;
        }
        default:
            break;
        }
        if (prev) {
            assert(prev->m_t == m.m_s);
        } else {
            assert(m.m_s.x == 0 && m.m_s.y == 0);
        }
        prev = &m;
    }
    assert(!prev || (prev->m_t.x == a.size() && prev->m_t.y == b.size()));
    return true;
}

std::vector<Move> myers(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    Area all{a,b};
    std::vector<Move> s;
    myers_moves(all, s);
    assert(check_moves(s, a, b));
    return s;
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
            std::cerr << "Line " << i << " differs:\n";
            std::cerr << "\tA:" << a[i] << '\n';
            std::cerr << "\tB:" << b[i] << '\n';
            r = false;
            return false;
        }
    }
    return r;
}

unsigned int first_mismatching_line(const std::vector<std::string>& a, const std::vector<std::string>& b)
{
    for (auto i{0u}; i<a.size(); ++i) {
        if (a[i] != b[i]) {
            return i;
        }
    }
    return -1;
}


int main()
{
    {
        std::string basePath{"/home/attila/Sviluppo/arraydiff/testdata/"};
        auto files = {"after.small", "before.small", "ben", "ban", "before.json", "after.json", "alpha.txt", "a.txt", "before.small", "beta.txt", "b.txt", "empty", "jsonobj_in_after.json", "jsonobj_in.json", "primo.txt", "secondo.txt", "terzo.txt", "test1", "test2", "x.txt", "y.txt"};
        //auto files = {"after.small", "before.small"};
        for(const auto& fa : files) {
            for (const auto& fb : files) {
                std::cerr << "Diff: " << fa << ' ' << fb << '\n';
                auto a = readFile(basePath+ fa);
                auto b = readFile(basePath+ fb);
                //assert(compare(a,b) == (std::string{fa} == std::string{fb}));
                a.reserve(a.size()+b.size());
                auto patches = myers(a,b);
                auto idx{0};
                for (const auto& p : patches) {
                    if(p.apply(a,b))
                    {

                        /*std::cerr << "Applied patch " << idx << " of " << patches.size() << '\n';
                        std::ofstream pfile;
                        std::stringstream prefix;
                        prefix << idx << '_';
                        pfile.open (prefix.str()+fa);
                        for (const auto& l : a) {
                        pfile << l << '\n';
                        }
                        pfile.close();*/
                    }
                    ++idx;
                    //std::cerr << "First mismatch at line " << first_mismatching_line(a,b) << '\n';
                }
                a.resize(b.size());
                //compare(a,b);
                assert(compare(a,b));
                /*if (std::string{fa} != std::string{fb}) {
                    exit(0);
                }*/
            }

        }

        exit(0);
    }
}
