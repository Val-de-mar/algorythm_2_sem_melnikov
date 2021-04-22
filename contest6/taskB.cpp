#include <iostream>
#include <vector>

int bin_search(const std::vector<int> &vect, int size, int val) {
    int l = 0, r = size;
    while (l < r) {
        if (vect[(l + r) / 2] <= val) {
            r = (l + r) / 2;
        } else {
            l = (l + r) / 2 + 1;
        }
    }
    return l;
}

struct Node {
    Node(int place, int val, Node *prev) : place(place), val(val), prev(prev) {}

    Node() {}


    int place;
    int val;
    Node *prev;
};

void optimise() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
}

int main() {
    optimise();
    int n, scaner, current_place, size = 0;

    std::cin >> n;
    std::vector<int> vec_of_rising_vals(n);
    std::vector<Node *> chain_vect(n), all(n);
    for (int i = 0; i < n; ++i) {
        std::cin >> scaner;
        current_place = bin_search(vec_of_rising_vals, size, scaner);
        vec_of_rising_vals[current_place] = scaner;
        chain_vect[current_place] = new Node(i + 1, scaner,
                                             (current_place > 0 ? chain_vect[current_place - 1] : nullptr));
        all[i] = chain_vect[current_place];
        if (current_place == size) {
            ++size;
        }
    }
    std::cout << size << std::endl;
    auto curr = chain_vect[size - 1];
    for (int i = size; i > 0; --i) {
        vec_of_rising_vals[i - 1] = curr->place;
        curr = curr->prev;
    }
    for (int i = 0; i < size; ++i) {
        std::cout << vec_of_rising_vals[i] << " ";
    }
    for (auto &i : all) {
        delete i;
    }
    return 0;
}
