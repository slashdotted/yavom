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
    for (auto i{0u}; i<a.size(); ++i) {
        if (a[i] != b[i]) {
            std::cerr << "Line " << i << " differs:\n";
            std::cerr << "\tA:" << a[i] << '\n';
            std::cerr << "\tB:" << b[i] << '\n';
            return false;
        }
    }
    return true;
}

int main()
{
    {
        {
            std::string basePath{"./testdata/"};
            auto files = {"after.small", "before.small", "ben", "ban", "alpha.txt", "a.txt", "before.small", "beta.txt", "b.txt", "empty", "jsonobj_in_after.json", "jsonobj_in.json", "primo.txt", "secondo.txt", "terzo.txt", "test1", "test2", "x.txt", "y.txt"};
            for(const auto& fa : files) {
                for (const auto& fb : files) {
                    std::cerr << "Comparing " << fa << " with " << fb << "...";
                    auto a = readFile(basePath+ fa);
                    auto b = readFile(basePath+ fb);
                    a.reserve(a.size()+b.size());
                    auto moves = myers(a,b);
                    std::cerr << moves.size() << " moves\n";
                    for (const auto& m : moves) {
                        apply_move(m, a);
                    }
                    compare(a,b);
                }
            }
        }
        exit(0);
    }
}
