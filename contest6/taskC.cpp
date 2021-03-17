#include <iostream>
#include <vector>
 
int main() {
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> answers(m + 1, std::vector<int>(n + 1, 0));
    std::vector<std::vector<std::pair<int, int>>> place(m + 1, std::vector<std::pair<int, int>>(n + 1));
    std::vector<int> ctrs(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        std::cin >> ctrs[i];
    }
 
    for (int i = 1; i < answers[0].size(); ++i) {
        answers[0][i] = 1000000000;
    }
    for (int i = 1; i < answers.size(); ++i) {
        for (int j = 1; j < answers[i].size(); ++j) {
            int min_v = answers[i - 1][j - 1];
            bool delta = false;
            int post = j, summ = 0;
            place[i][j].first = j - 1;
            place[i][j].second = j;
            for (int k = 1; k < j; ++k) {
                if (delta) {
                    --post;
                }
                summ = summ + std::abs(ctrs[j - k] - ctrs[post]);
                delta = !delta;
                if(summ + answers[i - 1][j - k - 1] < min_v) place[i][j] = std::make_pair(j - k - 1, post);
                min_v = std::min(min_v, summ + answers[i - 1][j - k - 1]);
            }
            answers[i][j] = min_v;
        }
    }
    std::cout << answers[m][n]<<"\n";
    std::vector<int> ans(m);
    int next_m = m, next_n = n;
    for (int i = 1; i <= m; ++i) {
        ans[m - i] = place[next_m][next_n].second;
        next_n = place[next_m][next_n].first;
        --next_m;
    }
    for (int an : ans) {
        std::cout << ctrs[an] << " ";
    }
    return 0;
}

