#include <iostream>
#include <list>
#include <vector>
#include <exception>
 
 
class HashToShortInt {
public:
    explicit HashToShortInt(int size) : size_(size) {}
 
    size_t operator()(int val) const {
        return val % size_;
    }

private:
    int size_;
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
    explicit MapHashListed(int size, const HashArgs&... hash_args) : data_(size), hash_(size, hash_args...) {}
 
    explicit MapHashListed(int size) : data_(size), hash_(size) {}
 
    void enableDefaultValue(const Tval& default_value) {
        default_val_ = new Tval(default_value);
        default_val_enable_ = true;
    }

    Tval& operator [] (const Tkey& key) {
        size_t place = hash_(key);
        auto iter = data_[place].begin();
        while(key != iter -> key_ && iter != data_[place].end()) ++iter;
        if (iter == data_[place].end()) {
            if (default_val_enable_) {
                data_[place].push_front(Node(key,*default_val_));
            } else {
                throw std::out_of_range{"Key hasn't been activated by insert and default activation hasn't been activated by enableDefaultValue"};
            }
        } else {
            data_[place].splice(data_[place].begin(), data_[place], iter);
            return data_[place].begin()->val_;
        }
        return data_[place].begin()->val_;
    }
 
    ~MapHashListed() {
        if (default_val_enable_) delete default_val_;
    }
private:
    std::vector<std::list<Node>> data_;
    Hash hash_;
    bool default_val_enable_ = false;
    Tval* default_val_;
};
 
int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    int n;
    std::cin >> n;
    int a, b, max = 5;
    MapHashListed<int, int, HashToShortInt> map(1000003);
    map.enableDefaultValue(-1);
    for (int i = 0; i < n; ++i) {
        std::cin >> a >> b;
        max = std::max(max, std::max(a, b));
        int &a_val = map[a];
        int &b_val = map[b];
        if (a_val == -1) {
            if (b_val == -1) {
                a_val = b;
                b_val = a;
            } else {
                a_val = b_val;
                b_val = a;
            }
        } else {
            if (b_val == -1) {
                b_val = a_val;
                a_val = b;
            } else {
                int t = a_val;
                a_val = b_val;
                b_val = t;
            }
        }
        std::cout << std::abs(a_val - b_val) << '\n';
    }
    return 0;
}
