/* Copyright 2021 Amos Brocco - contact@amosbrocco.ch
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef DIFF_H
#define DIFF_H
#include <tuple>
#include <vector>
#include <algorithm>
#include <cassert>
#include <chrono>

#define TK(v) (v+max)

// Yet Another Variation of Myers
// for generic containers (for example std::vector)
namespace syscall {
namespace yavom {

enum class OP {INSERT, DELETE};
using Point=std::tuple<long,long>;

template<typename K>
using Move=std::tuple<OP,Point,Point,std::vector<K>>;

template<template<typename, typename ... > typename C, typename K, typename ... Args>
void apply_move(const Move<K>& m, C<K,Args...>& a)
{
    const auto& [m_op, m_s, m_t, v] = m;
    switch(m_op) {
    case OP::DELETE: {
        auto count{std::get<0>(m_t) - std::get<0>(m_s)};
        a.erase(a.begin() + std::get<1>(m_s), a.begin() + std::get<1>(m_s) + count);
    }
    case OP::INSERT: {
        a.insert(a.begin() + std::get<1>(m_s), v.begin(), v.end());
    }
    }
}

template<template<typename, typename ... > typename C, typename K, typename ... Args>
struct Area {
    Area(const C<K,Args...>& a, const C<K,Args...>& b)
        : m_a{a}, m_b{b}, m_tl{0,0}, m_br{static_cast<long>(a.size()), static_cast<long>(b.size())}
    {
        assert(std::get<0>(m_tl) <= std::get<0>(m_br));
        assert(std::get<1>(m_tl) <= std::get<1>(m_br));
        trim();
    }

    Area(const Area& base, Point tl, Point br)
        : m_a{base.m_a}, m_b{base.m_b}, m_tl{tl}, m_br{br}
    {
        assert(contains_abs(tl));
        assert(contains_abs(br));
        assert(std::get<0>(m_tl) <= std::get<0>(m_br));
        assert(std::get<1>(m_tl) <= std::get<1>(m_br));
        trim();
    }

    const K& a(long index) const noexcept
    {
        return m_a[std::get<0>(m_tl) + index];
    }

    const K& b(long index) const noexcept
    {
        return m_b[std::get<1>(m_tl) + index];
    }

    const K& ra(long index) const noexcept
    {
        return m_a[std::get<0>(m_br) -1 - index];
    }

    const K& rb(long index) const noexcept
    {
        return m_b[std::get<1>(m_br) -1 - index];
    }

    long N() const noexcept
    {
        return m_N;
    }

    long M() const noexcept
    {
        return m_M;
    }

    long cN() const noexcept
    {
        assert(std::get<0>(m_br) >= std::get<0>(m_tl));
        return std::get<0>(m_br) - std::get<0>(m_tl);
    }

    long cM() const noexcept
    {
        assert(std::get<1>(m_br) >= std::get<1>(m_tl));
        return std::get<1>(m_br) - std::get<1>(m_tl);
    }

    Point abs_point_r(long rel_x, long rel_y) const noexcept
    {
        return  {std::get<0>(m_tl) + N() - rel_x, std::get<1>(m_tl) + M() - rel_y};
    }

    Point abs_point(long rel_x, long rel_y) const noexcept
    {
        return  {std::get<0>(m_tl) + rel_x, std::get<1>(m_tl) + rel_y};
    }

    Point point_r(Point p) const noexcept
    {
        return  {N() - std::get<0>(p), M() - std::get<1>(p)};
    }

    long rdiagonal(long k) const noexcept
    {
        return (-k + (N() - M()));
    }

    bool contains_abs(Point p) const noexcept
    {
        return std::get<0>(p) >= std::get<0>(m_tl) && std::get<0>(p) <= std::get<0>(m_br) && std::get<1>(p) >= std::get<1>(m_tl) && std::get<1>(p) <= std::get<1>(m_br);
    }

    const Point& tl() const noexcept
    {
        return m_tl;
    }

    const Point& br() const noexcept
    {
        return m_br;
    }

    const C<K,Args...>& a() const noexcept
    {
        return m_a;
    }
    const C<K,Args...>& b() const noexcept
    {
        return m_b;
    }

    void trim() noexcept
    {
        while(std::get<0>(m_tl) < std::get<0>(m_br) && std::get<1>(m_tl) < std::get<1>(m_br) && m_a[std::get<0>(m_tl)] == m_b[std::get<1>(m_tl)]) {
            ++std::get<0>(m_tl);
            ++std::get<1>(m_tl);
        }
        while(std::get<0>(m_br) > std::get<0>(m_tl) && std::get<1>(m_br) > std::get<1>(m_tl) && m_a[std::get<0>(m_br)-1] == m_b[std::get<1>(m_br)-1]) {
            --std::get<0>(m_br);
            --std::get<1>(m_br);
        }
        m_N = cN();
        m_M = cM();
    }

    const C<K,Args...>& m_a;
    const C<K,Args...>& m_b;
    Point m_tl{0,0};
    Point m_br{0,0};
    long m_N{0};
    long m_M{0};
};

template<template<typename, typename ... > typename C, typename K, typename ... Args>
std::tuple<Point,Point> myers_middle_move(const Area<C,K>& area, long ms_per_step)
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

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int d {0}; d <= static_cast<long>(max); ++d) {
        auto min_valid_k = -d + std::max(0l, d-static_cast<long>(area.M())) * 2;
        auto max_valid_k = d - std::max(0l, d-static_cast<long>(area.N())) * 2 ;
        // Forward step
        bool at_dest{false};
        long px{0};
        for (long k = min_valid_k; k<= max_valid_k; k+=2) {
            // Move downward or to the right
            if (k == -d || ((k != d )&& (V_fwd[TK(k-1)] < V_fwd[TK(k+1)]))) {
                px = x_fwd = V_fwd[TK(k+1)];
            }
            else {
                px = V_fwd[TK(k-1)];
                x_fwd = px + 1;
            }
            y_fwd = x_fwd - k;
            // Follow diagonal as long as possible
            while ((x_fwd < area.N()) && (y_fwd < area.M()) && (area.a(x_fwd) == area.b(y_fwd))) {
                ++x_fwd;
                ++y_fwd;
            }
            // Store best x position on this diagonal
            V_fwd[TK(k)] = x_fwd;

            // Check if we crossed the backward move
            if (d > 0) {
                const auto rk = area.rdiagonal(k);
                if (x_fwd >= (area.N() - V_bwd[TK(rk)])) {
                    Point top = area.abs_point(px, px -k);
                    if (area.contains_abs(top)) {
                        Point bottom = area.abs_point(x_fwd,y_fwd);
                        if (area.contains_abs(bottom)) {
                            return {top,bottom};
                        }
                    }
                }
            }

            if (x_fwd >= area.N() and y_fwd >= area.M()) {
                at_dest = true;
                break;
            }
        }

        // Backward step
        for (long k = min_valid_k; k<= max_valid_k; k+=2) {
            // Move downward or to the right
            if (k == -d || ((k != d )&& (V_bwd[TK(k-1)] < V_bwd[TK(k+1)]))) {
                px = x_bwd = V_bwd[TK(k+1)];
            }
            else {
                px = V_bwd[TK(k-1)];
                x_bwd = px + 1;
            }
            y_bwd = x_bwd - k;
            // Follow diagonal as long as possible
            while ((x_bwd < area.N()) && (y_bwd < area.M()) && (area.ra(x_bwd) == area.rb(y_bwd))) {
                ++x_bwd;
                ++y_bwd;
            }
            // Store best position on this diagonal
            V_bwd[TK(k)] = x_bwd;

            // Check if we crossed the forward move
            if (d > 0) {
                const auto rk = area.rdiagonal(k);
                if (x_bwd >= (area.N() - V_fwd[TK(rk)])) {
                    Point top = area.abs_point_r(x_bwd,y_bwd);
                    if (area.contains_abs(top)) {
                        Point bottom = area.abs_point_r(px,px-k);
                        if (area.contains_abs(bottom)) {
                            return {top,bottom};
                        }
                    }
                }
            }

            if (x_bwd >= area.N() and y_bwd >= area.M()) {
                at_dest = true;
                break;
            }
        }

        if (at_dest) {
            break;
        }

        if (ms_per_step > 0) {
            auto end_time = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count() > ms_per_step) {
                auto n = std::max(1L, area.N() / 2);
                auto m = std::max(1L, area.M() / 2);
                const auto& [tlx, tly] = area.tl();
                return {area.tl(), {tlx+n, tly+m}};
            }
        }
    }
    assert(false); // This can't be
    return {};
}

template<template<typename, typename ... > typename C, typename K, typename ... Args>
void myers_moves(Area<C,K> area, std::vector<Move<K>>& result, long ms_per_step)
{
    if (area.N() == 0 && area.M() == 0) {
        return;
    }
    else if (area.N() == 0) {
        if (!result.empty()) {
            auto& last = result.back();
            // Merge with last insert (if possible)
            if (std::get<0>(last) == OP::INSERT && std::get<2>(last) == area.tl()) {
                std::get<2>(last) = area.br();
                return;
            }
        }
        result.push_back(Move<K> {OP::INSERT, area.tl(), area.br(), {}});
    }
    else if (area.M() == 0) {
        if (!result.empty()) {
            auto& last = result.back();
            // Merge with last delete (if possible)
            if (std::get<0>(last) == OP::DELETE && std::get<2>(last) == area.tl()) {
                std::get<2>(last) = area.br();
                return;
            }
        }
        result.push_back(Move<K> {OP::DELETE, area.tl(), area.br(), {}});
    }
    else {
        auto middle = myers_middle_move(area, ms_per_step);
        const auto& [top, bottom] = middle;
        myers_moves(Area{area, area.tl(), top}, result, ms_per_step);
        myers_moves(Area{area, top, bottom}, result, ms_per_step);
        myers_moves(Area{area, bottom, area.br()}, result, ms_per_step);
    }
}

template<template<typename, typename ... > typename C, typename K, typename ... Args>
std::vector<Move<K>> myers(const C<K,Args...>& a, const C<K,Args...>& b, long ms_per_step = -1)
{
    const auto& longest = a.size() > b.size() ? a : b;
    const auto& shortest = a.size() > b.size() ? b : a;
    bool reversed = (&longest == &a);
    Area<C,K> all{shortest,longest};
    std::vector<Move<K>> s;
    myers_moves(all, s, ms_per_step);
    std::for_each (s.begin(), s.end(), [&longest,&reversed](auto& m) {
        auto& [m_op, m_s, m_t, v] = m;
        switch(m_op) {
        case OP::INSERT: {
            if (reversed) {
                m_op = OP::DELETE;
            }
            else {
                auto count{std::get<1>(m_t) - std::get<1>(m_s)};
                v.insert(v.begin(), longest.begin() + std::get<1>(m_s), longest.begin() + std::get<1>(m_s) + count);
            }
            break;
        }
        case OP::DELETE: {
            if (reversed) {
                m_op = OP::INSERT;
                auto count{std::get<1>(m_t) - std::get<1>(m_s)};
                v.insert(v.begin(), longest.begin() + std::get<1>(m_s), longest.begin() + std::get<1>(m_s) + count);
            }
            break;
        }
        }
    });
    return s;
}
}
}

#endif // DIFF_H
