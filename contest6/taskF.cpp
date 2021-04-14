#include <iostream>
#include <vector>
#include <algorithm>
 
int main() {
    int n, m;
    std::cin >> n;
    std::vector<int> first_sequence(n + 1);
    for (int i = 1; i <= n; ++i) {
        std::cin >> first_sequence[i];
    }
    std::cin >> m;
    std::vector<int> second_sequence(m + 1);
    for (int i = 1; i <= m; ++i) {
        std::cin >> second_sequence[i];
    }
    std::vector<std::vector<int>> table(n + 1, std::vector<int>(m + 1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            if(first_sequence[i] == second_sequence[j]) {
                table[i][j] = table[i - 1][j - 1] + 1;
            } else {
                table[i][j] = std::max(table[i][j - 1], table [i - 1][j]);
            }
        }
    }
    std::cout << table[n][m];
    return 0;
}
