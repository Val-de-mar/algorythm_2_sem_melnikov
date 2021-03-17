#include <iostream>
#include <vector>
#include <algorithm>
 
int main() {
    int n, m;
    std::cin >> n;
    std::vector<int> n_num(n + 1);
    for (int i = 1; i <= n; ++i) {
        std::cin >> n_num[i];
    }
    std::cin >> m;
    std::vector<int> m_num(m + 1);
    for (int i = 1; i <= m; ++i) {
        std::cin >> m_num[i];
    }
    std::vector<std::vector<int>> table(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if(n_num[i] == m_num[j]) {
                table[i][j] = table[i - 1][j - 1] + 1;
            } else {
                table[i][j] = std::max(table[i][j - 1], table [i - 1][j]);
            }
        }
    }
    std::cout << table[n][m];
    return 0;
}
