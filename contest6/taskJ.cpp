#include <iostream>
#include <vector>

int main() {
    int S, n;
    std::cin >> S >> n;
    std::vector<int> gold(n + 1);
    std::vector<std::vector<int>> ans(n + 1, std::vector<int>(S + 1, 0));

    for (int i = 1; i <= n; ++i) {
        std::cin >> gold[i];
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= S; ++j) {
            ans[i][j] = std::max(ans[i - 1][j], (j >= gold[i] ? (gold[i] + ans[i - 1][j - gold[i]]) : (0)));
        }
    }

    std::cout << ans[n][S];

    return 0;
}
