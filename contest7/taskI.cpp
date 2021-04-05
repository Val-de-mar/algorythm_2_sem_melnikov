#include <iostream>
#include <vector>


int main() {
    int n;
    std::cin >> n;
    std::vector<std::vector<bool>> edges(n, std::vector<bool>(n));
    {
        char edge;
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cin >> edge;
                if (edge == 'Y') {
                    edges[i][j] = true;
                } else {
                    edges[i][j] = false;
                }
            }
        }
    }
    std::vector<unsigned int> dp(1u << n, 0);
    for (unsigned int i = 0; i < dp.size(); ++i) {
        unsigned int max = 0;
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                bool b1 = bool(i & (1u << j));
                bool b2 = bool(i & (1u << j));
                if(bool(i & (1u << j)) && bool(i & (1u << k)) && edges[k][j]) {
                    max = std::max(max, 1 + dp[i & (~((1u << j) | (1u << k)))]);
                }
            }
        }
        dp[i] = max;
    }
    std::cout << 2 * dp[(1 << n) - 1];
    return 0;
}
