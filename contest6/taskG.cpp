
#include <iostream>
#include <string>
#include<vector>
#include <tuple>
 
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cin.tie(nullptr);
    std::string s1, s2;
    std::cin >> s1 >> s2;
    std::vector<std::vector<bool>> move_s2(s2.size() , std::vector<bool>(s1.size(), false)), move_s1(s2.size(), std::vector<bool>(s1.size(), false));
    std::vector<int> previous_line(s1.size(), 0), line(s1.size(), 0), first_column(s2.size());
    previous_line[0] = (s1[0] == s2[0]) ? 1 : 0;
    for (int j = 1; j < s1.size(); ++j) {
        previous_line[j] = std::max(previous_line[j - 1], static_cast<int>(s1[j] == s2[0]));
    }
    first_column[0] = (s1[0] == s2[0]) ? 1 : 0;
    for (int i = 1; i < s2.size(); ++i) {
        first_column[0] = std::max(first_column[0], static_cast<int>(s1[0] == s2[i]));
    }
 
    for (int j = 0; j < s1.size(); ++j) {
        move_s1[0][j] = true;
        move_s2[0][j] = (s1[j] == s2[0]);
    }
    for (int i = 1; i < s2.size(); ++i) {
        move_s2[i][0] = true;
        move_s1[i][0] = (s1[0] == s2[i]);
    }
    for (int i = 1; i < s2.size(); ++i) {
        line[0] = first_column[i];
        for (int j = 1; j < s1.size(); ++j) {
            if (s1[j]== s2[i]) {
                move_s2[i][j] = true;
                move_s1[i][j] = true;
                line[j] = previous_line [j - 1] + 1;
            } else {
                ((previous_line[j] > line[j - 1]) ? move_s2 : move_s1)[i][j] = true;
                line[j] = std::max(line[j - 1], previous_line[j]);
            }
        }
        std::swap(line, previous_line);
    }
    int i = s2.size() - 1, j = s1.size() - 1;
    std::string rans;
    while(i >= 0 && j >= 0) {
        if (s2[i] == s1[j]) rans.push_back(s2[i]);
        if (move_s2[i][j]) {
            if (move_s1[i][j]) {
                --j;
                --i;
            } else {
                --i;
            }
        } else {
            if (move_s1[i][j]) {
                --j;
            }
        }
 
    }
 
    for (int k = rans.size(); k > 0 ; --k) {
        std::cout<<rans[k - 1];
    }
    return 0;
}
