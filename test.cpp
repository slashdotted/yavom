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

using namespace syscall::yavom;

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

auto compare(const std::vector<std::string>& a, const std::vector<std::string>& b) -> bool
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
