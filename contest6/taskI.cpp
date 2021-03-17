#include <iostream>
#include <vector>
#include <algorithm>
 
int main() {
    int n, val;
    std::cin >> n;
    std::vector<long long> sequence(n), subsequence_num(n), excesses(n, 0);
    std::vector<int> adress(n, -1);
    std::vector<std::pair<int, int>> sorted_seq(n);
    long long summ = 0;
 
    for (int i = 0; i < n; ++i) {
        std::cin >> sequence[i];
        sorted_seq[i] = std::make_pair(sequence[i], i);
    }
 
    std::sort(sorted_seq.begin(), sorted_seq.end());
 
    for (int i = 1; i < n; ++i) {
        if (sorted_seq[i].first == sorted_seq[i - 1].first) {
            adress[sorted_seq[i].second] = sorted_seq[i - 1].second;
        }
    }
    subsequence_num[0] = 1;
    summ = 2;
    for (int i = 1; i < n; ++i) {
        subsequence_num[i] = summ;
        if (adress[i] != -1) {
            excesses[i] = excesses[adress[i]] + subsequence_num[adress[i]];
            subsequence_num[i] += 2 * 1000000007;
            subsequence_num[i] -= excesses[i];
        }
        summ += subsequence_num[i];
        summ %= 1000000007;
        subsequence_num[i] %= 1000000007;
        excesses[i] %= 1000000007;
    }
    std::cout << ((summ - 1) % 1000000007);
    return 0;
}
