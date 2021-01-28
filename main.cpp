#include <iostream>
#include <queue>
#include <tuple>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>


enum class OP { KEEP, DELETE, INSERT };
using Step=std::tuple<OP,unsigned int,std::string>;


std::vector<Step> myers_diff(const std::vector<std::string>& a,
                             const std::vector<std::string>& b)
{
    std::vector<Step> steps;
    std::vector<int> V;
    std::vector<std::vector<int>> paths;
    int N = a.size();
    int M = b.size();
    int max = M+N;
    V.resize(2*max+1);
    V[1] = 0;
    int x{0},y{0};
    int ses{-1};
#define TK(v) (v+max)
    for (int d {0}; d<= max; ++d) {
        paths.push_back(V);
        auto min_valid_k = -d + std::max(0, d - M) * 2;
        auto max_valid_k = d - std::max(0, d-N) * 2;
        //std::cerr << "d=" << d << " k_range=(" << min_valid_k << "," << max_valid_k << ")\n";
        for (int k = min_valid_k; k<= max_valid_k; k+=2) {
            // If we are at the bottom diagonal (k == -d) or if we are
            // not on the topmost diagonal (k!=d) and the upper diagonal went further
            // than the lower diagonal (relative to the current one k), INSERT
            if (k == -d || ((k != d )&& (V[TK(k-1)] < V[TK(k+1)]))) {
                x = V[TK(k+1)];
            }
            // Otherwise go downward to the lowest diagonal and advance x one step, right, DELETE
            else {
                x = V[TK(k-1)] + 1;
            }
            y = x - k;
            // Consume original diagonal (if possible)
            while ((x < N) && (y < M) && (a[x] == b[y])) {
                ++x;
                ++y;
            }
            V[TK(k)] = x; // Store the farthest point on this diagonal
            if ((x>=N) && (y>= M)) {
                ses = d;
                goto outside;
            }
        }
    }
outside:
    //std::cerr << "SES " << ses << '\n';
    //std::cerr << "paths.size() = " << paths.size() << '\n';

    x = a.size();
    y = b.size();

    //std::cerr << "Starting from (" << x << "," << y << ")\n";
    for (int d{ses}; d >= 0; --d) {
        int k = x - y;
        const auto& Vd = paths[d]; // Vd[k] is the best x on diagonal k
        auto min_valid_k = -d + std::max(0, d - M) * 2;
        auto max_valid_k = d - std::max(0, d-N) * 2;
        auto half = Vd.size() / 2;
        //auto max_iter = std::max_element(Vd.begin() + half + min_valid_k, Vd.begin() + half + max_valid_k, [](const auto& lh, const auto& rh) {
        auto max_iter = std::max_element(Vd.begin(), Vd.end(), [](const auto& lh, const auto& rh) {
            return lh < rh;
        });
        int best_k = std::distance(Vd.begin(), max_iter) - max;
        //std::cerr << "Best k=" << best_k << " with "<< *max_iter<< "\n";
        auto prev_x = *max_iter;
        auto prev_y = prev_x - best_k;
        std::cerr << "(" << x << "," << y << ") on diagonal k=" << k << " will move to (" << prev_x << "," << prev_y << ") on diagonal prev=" << best_k << '\n';
        if (best_k < k) {
            // Was a rightward move
            //std::cerr << "A (" << x << ',' << y << ") -> (" << prev_x << ',' << prev_y << ')' << '\n';
            steps.insert(steps.begin(), {OP::DELETE, x, {}});
        }
        else if (best_k > k) {
            // Was a downward move
            //std::cerr << "+ (" << x << ',' << y << ") -> (" << prev_x << ',' << prev_y << ')' << '\n';
            steps.insert(steps.begin(), {OP::INSERT, x, b[prev_y]});
        }
        while(a[prev_x] == b[prev_y]) {
            //std::cerr << " @(" << prev_x << "," << prev_y << ") " << a[prev_x] << "==" << b[prev_y] << "\n";
            --prev_x;
            --prev_y;
        }
        //std::cerr << "D (" << x << ',' << y << ") -> (" << prev_x << ',' << prev_y << ')' << '\n';

        x = prev_x;
        y = prev_y;
    }
    std::reverse(steps.begin(), steps.end());
    return steps;
}

/*


std::vector<OP> myers_naive(const std::vector<std::string>& a,
                            const std::vector<std::string>& b)
{
    std::queue<Step> toVisit;
    toVisit.push({0,0,{},0});
    std::vector<OP> bestPath;
    unsigned long bestPathCost{a.size()+b.size()+1};
    while(!toVisit.empty()) {
        auto& [idx_a, idx_b, path, current_cost] = toVisit.front();
        if (current_cost < bestPathCost) {
            // Segui diagonale
            while(a[idx_a] == b[idx_b]) {
                path.push_back(OP::KEEP);
                ++idx_a;
                ++idx_b;
                if (idx_a == a.size() || idx_b == b.size()) {
                    break;
                }
            }
            // Non posso più seguire la diagonale
            // Verifica se abbiamo ancora caratteri nella prima stringa
            if (idx_a == a.size()) { // End of start
                while((idx_b < b.size()) && (current_cost < bestPathCost)) {
                    path.push_back(OP::INSERT);
                    ++current_cost;
                    ++idx_b;
                }
                if (current_cost < bestPathCost) {
                    bestPathCost = current_cost;
                    bestPath = path;
                }
            }
            // Verifica se abbiamo ancora caratteri nella seconda stringa
            else if (idx_b == b.size()) {
                while((idx_a < a.size()) && (current_cost < bestPathCost)) {
                    path.push_back(OP::DELETE);
                    ++current_cost;
                    ++idx_a;
                }
                if (current_cost < bestPathCost) {
                    bestPathCost = current_cost;
                    bestPath = path;
                }
            }
            else {
                auto npath = path;
                path.push_back(OP::DELETE);
                toVisit.push({++idx_a, idx_b, path, current_cost + 1});
                npath.push_back(OP::INSERT);
                toVisit.push({idx_a, idx_b+1, npath, current_cost + 1});

            }
        }
        toVisit.pop();

    }
    return bestPath;
}

void printPath(const std::vector<OP>& ops, const std::vector<std::string>& a,
               const std::vector<std::string>& b)
{
    unsigned int idx_a{0};
    unsigned int idx_b{0};
    for (const auto& p : ops) {
        switch(p) {
        case OP::KEEP:
            std::cerr << a[idx_a] << '\n';
            ++idx_a;
            ++idx_b;
            break;
        case OP::INSERT:
            std::cerr << '+' << idx_a << ',' << b[idx_b] << '\n';
            ++idx_b;
            break;
        case OP::DELETE:
            std::cerr << '-' << '\n';
            ++idx_a;
            break;
        }
    }
}

*/

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
    auto a = readFile("/home/attila/before.json");
    std::cerr << "Read " << a.size() << " lines\n";
    auto b = readFile("/home/attila/after.json");
    std::cerr << "Read " << b.size() << " lines\n";
    auto d = myers_diff(a, b);
    for (const auto& [op, idx, data] : d) {
        if (op == OP::INSERT) {
            std::cerr << idx << '+' << data << '\n';
        }
        else if (op == OP::DELETE) {
            std::cerr << idx << '-' << '\n';
        }
    }

    /*auto a = readFile("/home/attila/before.json");
    std::cerr << "Read " << a.size() << " lines\n";
    auto b = readFile("/home/attila/after.json");
    std::cerr << "Read " << b.size() << " lines\n";
    auto d = myers_diff(a, b);*/
    //printPath(d, a, b);
}
