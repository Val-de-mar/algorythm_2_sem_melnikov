#include <algorithm>
#include <iostream>
#include <vector>
 
 
int main() {
    long long answer = 0;
    int n;
    int C;
    std::cin >> n;
    std::vector<long long> vals_1 (n/2), vals_2((n + 1)/2);
    for (long long & val : vals_1) {
        std::cin >> val;
    }
    for (long long & val : vals_2) {
        std::cin >> val;
    }
    std::cin >> C;
    std::vector<long long> first((1u << (n / 2)), 0), second ((1u << ((n + 1)/2)), 0);
    for (unsigned int i = 0; i < first.size(); ++i) {
        unsigned int place = 1;
        for (int j = 0; j < n / 2; ++j) {
            if (bool(place & i)) {
                first[i] += vals_1[j];
            }
            place = (place << 1);
        }
    }
    std::sort(first.begin(), first.end());
    for (unsigned int i = 0; i < second.size(); ++i) {
        unsigned int place = 1;
        for (int j = 0; j < (n + 1) / 2; ++j) {
            if (bool(place & i)) {
                second[i] += vals_2[j];
            }
            place = (place << 1);
        }
    }
    std::sort(second.begin(), second.end());
    int j = 0;
    int delta = 0;
    for (int i = static_cast<int>(first.size()) - 1; i >= 0; --i) {
        while(j + delta < second.size() && first[i] + second[j + delta] <= C) {
            ++delta;
        }
        answer += (long long)(delta) * (i + 1);
        j += delta;
        delta = 0;
    }
 
    std::cout << answer;
    return 0;
}
