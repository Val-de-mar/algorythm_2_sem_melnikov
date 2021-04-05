#include <iostream>
#include <vector>

int main() {
    int n;
    int road;
    std::cin >> n;
    std::vector<std::vector<int>> roads;
    for (int i = 0; i < n; ++i) {
        roads.emplace_back();
        for (int j = 0; j < n; ++j) {
            std::cin >> road;
            roads[i].push_back(road);
        }
    }
    std::vector<std::vector<unsigned int>> dp((1u << n), std::vector<unsigned int>(n));
    std::vector<std::vector<int>> last_city((1u << n), std::vector<int>(n, -1));
    dp[0] = std::vector<unsigned int>(n, 0);
    for (unsigned int i = 1; i < (1u << n); ++i) {
        unsigned int place = 1;
        for (int j = 0; j < n; ++j) {
            unsigned int pre = -1;
            pre /= 2;
            for (int k = 0; k < n; ++k) {
                if (bool(i & place)) {
                    if(pre >= dp[i & (~place)][k] + roads[j][k]) {
                        pre = dp[i & (~place)][k] + roads[j][k];
                        last_city[i][j] = k;
                    }
                }
            }
            dp[i][j] = pre;
            place *= 2;
        }
    }
    unsigned int min = -1;
    unsigned int min_place;
    for (int i = 0; i < n; ++i) {
        if(dp[(1u << n) - 1][i] < min) {
            min = dp[(1u << n) - 1][i];
            min_place = i;
        }
    }
    std::cout << min << std::endl;
    int next_city = static_cast<int>(min_place);
    int city = -1;
    unsigned int subset = (1u << n) - 1;
    while(next_city != city) {
        std::cout << next_city + 1 << " ";

        city = next_city;
        next_city = last_city[subset][next_city];
        subset = (subset & (~(1u <<city)));
    }
    return 0;
}
