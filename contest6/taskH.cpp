#include <iostream>
#include <vector>
#include <map>
#include <set>
 
int main() {
    int n, m;
    std::cin >> n >> m;
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));
    std::vector<int> a(n, 0), b(m, 0);
 
    for (int & element : a) {
        std::cin >> element;
    }
    for (int & element : b) {
        std::cin >> element;
    }
    a.insert(a.begin(), 0);
    b.insert(b.begin(), 0);
    for (int i = 1; i < dp.size(); ++i) {
        int previous_longest_sub_length = 0;
        for (int j = 1; j < dp[i].size(); ++j) {
            if (a[i] == b[j]) {
                dp[i][j] = 1 + previous_longest_sub_length;
            } else {
                dp[i][j] = dp[i - 1][j];
            }
            if (a[i] > b[j]) {
                previous_longest_sub_length = std::max(previous_longest_sub_length, dp[i - 1][j]);
            }
        }
    }
    int ans = 0;
    for (int i = 1; i < dp[0].size(); ++i) {
        ans = std::max(dp[n][i], ans);
    }
    std::cout << ans;
    return 0;
}
