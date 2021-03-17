#include <iostream>
#include <vector>
#include <cmath>
 
int main() {
    int n, k;
    std::cin >> n >> k;
    if(k == 0) {
        if(n == 1) {
            std::cout << 0;
        } else {
            std::cout << -1;
        }
        return 0;
    }
    k = std::min(k, int(2 * log(n) + 2));
    std::vector<std::vector<int>> dp(k + 1, std::vector<int>(n + 1, 0));
 
    for (int i = 1; i < dp[1].size(); ++i) {
        dp[1][i] = i - 1;
    }
    for (int j = 1; j < dp.size(); ++j) {
        dp[j][1] = 0;
    }
    for (int i = 2; i < dp.size(); ++i) {
        int place = 1;
        for (int j = 2; j < dp[i].size(); ++j) {
            while(place < j && std::max(dp[i][j - place - 1], dp[i - 1][place + 1]) <= std::max(dp[i][j - place], dp[i - 1][place])) {
                ++place;
            }
            dp[i][j] = 1 + std::max(dp[i][j - place], dp[i - 1][place]);
        }
    }
    std::cout << dp[k][n];
    return 0;
}
