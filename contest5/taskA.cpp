#include <iostream>
#include <utility>
#include <vector>
#include <list>

typedef unsigned int uint;



class StringHash{
public:
    StringHash(int size, int mod = 37) : size_(size), module_(mod) {}
    size_t operator () (const std::string& val) const {
        unsigned long long ans = 0, mod = 1;
        for (char i : val) {
            ans += i * mod;
            ans %= size_;
            mod *= module_;
            mod %= size_;
        }
        return static_cast<size_t>(ans);
    }

private:
    int size_;
    int module_;
};


template<typename Tkey, typename Tval, typename Hash>
class MapHashListed {
public:
    class Node{
        public:
        Tkey key_;
        Tval val_;

        Node(Tkey  key, Tval val) : key_(std::move(key)), val_(std::move(val)) {}

        bool operator < (const Node& another) const {
            if (key_ != another.key_) {
                return key_ < another.key_;
            } else {
                return val_ < another.val_;
            }
        }
        bool operator > (const Node& another) const {
            if (key_ != another.key_) {
                return key_ > another.key_;
            } else {
                return val_ > another.val_;
            }
        }
        bool operator <= (const Node& another) const {
            if (key_ != another.key_) {
                return key_ <= another.key_;
            } else {
                return val_ <= another.val_;
            }
        }
        bool operator >= (const Node& another) const {
            if (key_ != another.key_) {
                return key_ >= another.key_;
            } else {
                return val_ >= another.val_;
            }
        }
    };
    template <typename ...HashArgs>
    explicit MapHashListed(int size, HashArgs... hash_args) : data_(size), hash_(size, hash_args...) {}

    explicit MapHashListed(int size) : data_(size), hash_(size) {}


    Tval plus(const Tkey &key, const Tval &val) {
        size_t place = hash_(key);
        auto iter = data_[place].begin();
        while(key != iter -> key_ && iter != data_[place].end()) ++iter;
        if (iter == data_[place].end()) {
            data_[place].push_front(Node(key, val));
        } else {
            data_[place].splice(data_[place].begin(), data_[place], iter);
            data_[place].begin()->val_ += val;
        }
        return data_[place].begin() -> val_;
    }

private:
    std::vector<std::list<Node>> data_;
    Hash hash_;
};

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cout.tie(nullptr);
    std::cin.tie(nullptr);
    MapHashListed<std::string, int, StringHash> map(1000003);
    std::string S;
    int delta;
    while (std::cin >> S) {
        std::cin >> delta;
        std::cout << map.plus(S, delta) << std::endl;
    }

    return 0;
}