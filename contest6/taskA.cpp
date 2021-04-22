#include <iostream>
#include <vector>
#include <algorithm>
 
typedef unsigned int uint;

void print_taxi(int num, const std::vector <std::string> &passengers) {
    std::cout << "Taxi " << num << ": ";
    if (passengers.size() == 1) {
        std::cout << passengers[0] << ".\n";
    } else {
        int i = 0;
        for (; i < passengers.size() - 2; ++i) {
            std::cout << passengers[i] << ", ";
        }
        std::cout << passengers[i] << " and " << passengers[i + 1] << ".\n";
    }
}

unsigned int max_in_gap(const std::vector <std::pair<uint, std::string>> &male_houses_taxis,
                        const std::vector <std::pair<uint, std::string>> &female_houses_taxis, int x1, int x2, int y1,
                        int y2) {
    uint max = 0;
    for (int i = x1 + 1; i <= x2; ++i) {
        max = std::max(max, male_houses_taxis[i].first);
    }
    for (int i = y1 + 1; i <= y2; ++i) {
        max = std::max(max, female_houses_taxis[i].first);
    }
    return max;
}

void update(int n, int m, std::vector <std::vector<std::pair < uint, std::pair < int, int>>>>& dp,
                        std::vector <std::pair<uint, std::string>>& male_houses_taxis,
                        std::vector <std::pair<uint, std::string>>& female_houses_taxis) {
    for (int i = 0; i < n + 1; ++i) {
        for (int j = 0; j < m + 1; ++j) {
            if (i * 3 < j) {
                dp[i][j].first = -1;
                dp[i][j].second = std::make_pair(0, 0);
            } else {
                if (i + j <= 4) {
                    dp[i][j].first = max_in_gap(male_houses_taxis, female_houses_taxis, 0, i, 0, j);
                    dp[i][j].second = std::make_pair(0, 0);
                } else {
                    unsigned int min = -1;
                    for (int k = 1; k <= 4; ++k) {
                        if (i - k >= 0 && j - 4 + k >= 0 && dp[i - k][j - 4 + k].first != uint(-1)) {
                            unsigned int temp = dp[i - k][j - 4 + k].first;
                            temp += max_in_gap(male_houses_taxis, female_houses_taxis, i - k, i, j - 4 + k, j);
                            if (min > temp) {
                                min = temp;
                                dp[i][j].first = min;
                                dp[i][j].second = std::make_pair(i - k, j - 4 + k);
                            }
                        }
                    }
                }
            }
        }
    }
}

void taxiCompanation(int n, int m, std::vector <std::vector<std::pair < uint, std::pair < int, int>>>>& dp,
        std::vector <std::pair<uint, std::string>>& male_houses_taxis,
        std::vector <std::pair<uint, std::string>>& female_houses_taxis) {
    int i = 1;
    std::vector<std::string> taxi;
    std::pair<uint, uint> next = std::make_pair(n, m);
    while (next != std::make_pair(uint(0), uint(0))) {
        taxi.clear();
        for (int j = dp[next.first][next.second].second.first + 1; j <= next.first ; ++j) {
            taxi.push_back(male_houses_taxis[j].second);
        }
        for (int j = dp[next.first][next.second].second.second + 1; j <= next.second ; ++j) {
            taxi.push_back(female_houses_taxis[j].second);
        }
        print_taxi(i, taxi);
        ++ i;
        next = dp[next.first][next.second].second;
    }
}


typedef std::vector<std::vector<std::pair<uint, std::pair<int, int>>>> triplet_matrix;
typedef std::vector<std::pair<uint, std::pair<int, int>>> triplet_line;


int main() {
    int n, m;
    int input;
    std::string name;
    std::cin >> n;
    std::vector<std::pair<uint, std::string>> male_houses_taxis(n + 1);
    for (int i = 1; i < n + 1; ++i) {
        std::cin >> name >> input;
        male_houses_taxis[i] = std::make_pair(input, name);
    }
    std::cin >> m;
    std::vector<std::pair<uint, std::string>> female_houses_taxis(m + 1);
    for (int i = 1; i < m + 1; ++i) {
        std::cin >> name >> input;
        female_houses_taxis[i] = std::make_pair(input, name);
    }
    std::sort(male_houses_taxis.begin(), male_houses_taxis.end());
    std::sort(female_houses_taxis.begin(), female_houses_taxis.end());

    triplet_matrix dp (n + 1, triplet_line(m + 1,std::make_pair(0, std::make_pair(0, 0))));
    update(n, m, dp, male_houses_taxis, female_houses_taxis);
    std::cout << dp[n][m].first << '\n';
    std::cout << (n + m + 3) / 4 << '\n';
    taxiCompanation(n, m, dp, male_houses_taxis, female_houses_taxis);

    return 0;
}
