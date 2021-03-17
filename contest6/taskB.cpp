#include <iostream>
#include <vector>
 
int bin_search(const std::vector<int>& vect, int size, int val) {
    int l = 0, r = size;
    while (l < r){
        if (vect[(l + r) / 2] <= val) {
            r = (l + r) / 2;
        } else {
            l = (l + r) / 2 + 1;
        }
    }
    return l;
}
 
struct Node {
    int place;
    int val;
    Node* prev;
    Node(int place, int val, Node* prev) : place(place), val(val), prev(prev) {}
    Node(){}
};
 
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n, sc, pl, size = 0;
 
    std::cin >> n;
    std::vector<int> vec(n);
    std::vector<Node*> vecn(n), all(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> sc;
        pl = bin_search(vec, size, sc);
        vec[pl] = sc;
        vecn[pl] = new Node(i + 1, sc, (pl > 0 ? vecn[pl - 1] : nullptr));
        all[i] = vecn[pl];
        if (pl == size) {
            ++size;
        }
    }
    std::cout << size << std::endl;
    auto curr = vecn[size - 1];
    for (int i = size; i > 0; --i) {
        vec[i - 1] = curr -> place;
        curr = curr -> prev;
    }
    for (int i = 0; i < size; ++i) {
        std::cout << vec[i] << " ";
    }
    for (auto & i : all) {
        delete i;
    }
    return 0;
}
