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
#include "diff.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>

using namespace orgsyscall::yavom;

auto readFile(const std::string& filePath) -> std::vector<std::string>
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

template<typename T>
auto compare(const std::vector<T>& a, const std::vector<T>& b) -> bool
{
    if (a.size() != b.size()) {
        return false;
    }
    for (auto i{0U}; i<a.size(); ++i) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

auto main() -> int
{
    {
        {
            auto a = std::vector<std::string>{"A", "B", "C", "D", "E", "F", "A"};
            auto b = std::vector<std::string>{"C", "B", "A", "D", "F", "E"};
            auto moves = myers_unfilled(a,b, -1);
            std::cerr << moves.size() << " moves...";
            myers_fill(b, moves);
            myers_strip_moves(moves);
            std::cerr << " filled...";
            for (const auto& m : moves) {
                apply_move(m, a);
            }
            if (!compare(a,b)) {
                std::cerr << " fail!\n";
            }
            else {
                std::cerr << " success!\n";
            }
            
        }
        {
            auto a = std::vector<std::string>{"A", "W", "E", "S", "O", "M", "O"};
            auto b = std::vector<std::string>{"S", "T", "R", "A", "N", "G", "E", "S", "O", "M", "O"};
            auto moves = myers_unfilled(a,b, -1);
            std::cerr << moves.size() << " moves...";
            myers_fill(b, moves);
            myers_strip_moves(moves);
            std::cerr << " filled...";
            for (const auto& m : moves) {
                apply_move(m, a);
            }
            if (!compare(a,b)) {
                std::cerr << " fail!\n";
            }
            else {
                std::cerr << " success!\n";
            }
        }
        {
            for (auto s{2}; s <= 8; ++s) {
                auto vsize = pow(2, s);
                std::cerr << "Comparing vectors of size " << vsize << "...";
                std::vector<int_fast64_t> a;
                a.reserve(vsize);
                for(size_t x{0}; x < a.capacity(); ++x) {
                    a.push_back(x);
                }
                std::vector<int_fast64_t> b;
                b.reserve(vsize);
                for(size_t x{0}; x < a.capacity(); ++x) {
                    b.push_back(-x);
                }
                auto moves = myers(a,b, -1);
                for (const auto& m : moves) {
                    apply_move(m, a);
                }
                if (!compare(a,b)) {
                    std::cerr << " fail!\n";
                }
                else {
                    std::cerr << " success!\n";
                }
            }
        }
        {
            std::string basePath{"./testdata/"};
            auto files = {"alpha", "ban", "ben", "beta", "delta", "empty", "first", "gamma", "huge", "huge2", "large1", "large2", "second", "test1", "test2", "third", "x", "y"};
            for(const auto& fa : files) {
                for (const auto& fb : files) {
                    std::cerr << "Comparing (two steps) " << fa << " with " << fb << "... ";
                    auto a = readFile(basePath+ fa);
                    auto b = readFile(basePath+ fb);
                    auto moves = myers_unfilled(a,b, -1);
                    std::cerr << moves.size() << " moves...";
                    myers_fill(b, moves);
                    myers_strip_moves(moves);
                    std::cerr << " filled...";
                    for (const auto& m : moves) {
                        apply_move(m, a);
                    }
                    if (!compare(a,b)) {
                        std::cerr << " fail!\n";
                    }
                    else {
                        std::cerr << " success!\n";
                    }
                }
            }
        }
        {
            std::string basePath{"./testdata/"};
            auto files = {"alpha", "ban", "ben", "beta", "delta", "empty", "first", "gamma", "huge", "huge2", "large1", "large2", "second", "test1", "test2", "third", "x", "y"};
            for(const auto& fa : files) {
                for (const auto& fb : files) {
                    std::cerr << "Comparing (two steps) " << fa << " with " << fb << "... ";
                    auto a = readFile(basePath+ fa);
                    auto b = readFile(basePath+ fb);
                    auto moves = myers_unfilled(a,b, -1);
                    std::cerr << moves.size() << " moves...";
                    myers_fill(b, moves);
                    std::cerr << " filled...";
                    for (const auto& m : moves) {
                        apply_move(m, a);
                    }
                    if (!compare(a,b)) {
                        std::cerr << " fail!\n";
                    }
                    else {
                        std::cerr << " success!\n";
                    }
                }
            }
        }
        {
            std::string basePath{"./testdata/"};
            auto files = {"alpha", "ban", "ben", "beta", "delta", "empty", "first", "gamma", "huge", "huge2", "large1", "large2", "second", "test1", "test2", "third", "x", "y"};
            for(const auto& fa : files) {
                for (const auto& fb : files) {
                    std::cerr << "Comparing (single step) " << fa << " with " << fb << "...";
                    auto a = readFile(basePath+ fa);
                    auto b = readFile(basePath+ fb);
                    auto moves = myers(a,b, -1);
                    for (const auto& m : moves) {
                        apply_move(m, a);
                    }
                    if (!compare(a,b)) {
                        std::cerr << " fail!\n";
                    }
                    else {
                        std::cerr << " success!\n";
                    }
                }
            }
        }
        {
            std::string basePath{"./testdata/"};
            auto files = {"alpha", "ban", "ben", "beta", "delta", "empty", "first", "gamma", "huge", "huge2", "large1", "large2", "second", "test1", "test2", "third", "x", "y"};
            for(const auto& fa : files) {
                for (const auto& fb : files) {
                    std::cerr << "Comparing (two steps) " << fa << " with " << fb << "... ";
                    auto a = readFile(basePath+ fa);
                    auto b = readFile(basePath+ fb);
                    auto moves = myers_unfilled(a,b, 1'000'000);
                    std::cerr << moves.size() << " moves...";
                    myers_fill(b, moves);
                    myers_strip_moves(moves);
                    std::cerr << " filled...";
                    for (const auto& m : moves) {
                        apply_move(m, a);
                    }
                    if (!compare(a,b)) {
                        std::cerr << " fail!\n";
                    }
                    else {
                        std::cerr << " success!\n";
                    }
                }
            }
        }
        {
            std::string basePath{"./testdata/"};
            auto files = {"alpha", "ban", "ben", "beta", "delta", "empty", "first", "gamma", "huge", "huge2", "large1", "large2", "second", "test1", "test2", "third", "x", "y"};
            for(const auto& fa : files) {
                for (const auto& fb : files) {
                    std::cerr << "Comparing (two steps) " << fa << " with " << fb << "... ";
                    auto a = readFile(basePath+ fa);
                    auto b = readFile(basePath+ fb);
                    auto moves = myers_unfilled(a,b, 1'000'000);
                    std::cerr << moves.size() << " moves...";
                    myers_fill(b, moves);
                    std::cerr << " filled...";
                    for (const auto& m : moves) {
                        apply_move(m, a);
                    }
                    if (!compare(a,b)) {
                        std::cerr << " fail!\n";
                    }
                    else {
                        std::cerr << " success!\n";
                    }
                }
            }
        }
        {
            std::string basePath{"./testdata/"};
            auto files = {"alpha", "ban", "ben", "beta", "delta", "empty", "first", "gamma", "huge", "huge2", "large1", "large2", "second", "test1", "test2", "third", "x", "y"};
            for(const auto& fa : files) {
                for (const auto& fb : files) {
                    std::cerr << "Comparing (single step) " << fa << " with " << fb << "...";
                    auto a = readFile(basePath+ fa);
                    auto b = readFile(basePath+ fb);
                    auto moves = myers(a,b, 1'000'000);
                    for (const auto& m : moves) {
                        apply_move(m, a);
                    }
                    if (!compare(a,b)) {
                        std::cerr << " fail!\n";
                    }
                    else {
                        std::cerr << " success!\n";
                    }
                }
            }
        }
        exit(0);
    }
}
