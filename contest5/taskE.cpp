#include <iostream>
#include <list>
#include <vector>
#include <exception>
#include <fstream>
 
class StringHash{
public:
    int size;
    int module;
    StringHash(int size, int mod = 37) : size(size), module(mod) {}
    size_t operator () (const std::string& val) const {
        unsigned long long ans = 0, mod = 1;
        for (char i : val) {
            ans += i * mod;
            ans %= size;
            mod *= module;
            mod %= size;
        }
        return static_cast<size_t>(ans);
    }
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


    void insert(const Tkey &key, const Tval &val) {
        size_t place = hash_(key);
        auto iter = data_[place].begin();
        while(key != iter -> key_ && iter != data_[place].end()) ++iter;
        if (iter == data_[place].end()) {
            data_[place].push_front(Node(key, val));
        } else {
            data_[place].splice(data_[place].begin(), data_[place], iter);
            data_[place].begin()->val_ = val;
        }
    }

    void erase(const Tkey &key) {
        size_t place = hash_(key);
        auto iter = data_[place].begin();
        while(key != iter -> key_ && iter != data_[place].end()) ++iter;
        if (iter != data_[place].end()) {
            data_[place].erase(iter);
        }
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
    MapHashListed<std::string, std::string, StringHash> map(1000003);
    std::string command, key, val;
    std::ifstream fin;
    fin.open("map.in");
    std::ofstream fout;
    fout.open("map.out");
    while (fin >> command) {
        switch (command[0]) {
            case 'p':
                fin >> key >> val;
                map.insert(key, val);
                break;
            case 'g':
                fin >> key;
                try {
                    fout << map[key] << '\n';
                } catch (...) {
                    fout << "none" << '\n';
                }
                break;
            case 'd':
                fin >> key;
                map.erase(key);
                break;
        }
    }
 
    return 0;
}
