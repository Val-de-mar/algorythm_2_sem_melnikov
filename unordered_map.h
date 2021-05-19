#include <vector>
#include <iostream>
#include <tuple>



template<typename T, typename Allocator = std::allocator<T>>
class List {
public:
    class Node;

    class NodeData;

    List(const Allocator &allocator = Allocator()) : alloc_(), sub_alloc_(), simple_alloc_(allocator) {
        first = STraits::allocate(sub_alloc_, 1);
        last = STraits::allocate(sub_alloc_, 1);
        STraits::construct(sub_alloc_, first);
        STraits::construct(sub_alloc_, last);
        first->next_ = last;
        last->prev_ = first;
    }

    List(size_t count, const T &value, const Allocator &alloc = Allocator()) : List(alloc) {
        for (size_t i = 0; i < count; ++i) {
            push_back(value);
        }
    }

    List(size_t count, const Allocator &alloc = Allocator()) : List(alloc) {
        for (size_t i = 0; i < count; ++i) {
            empl_back();
        }
    }

    List(const List &other) : List(
            std::allocator_traits<Allocator>::select_on_container_copy_construction(other.get_allocator())) {
        for (const auto &i:other) {
            push_back(i);
        }
    }

    List(List &&other) : List(other.get_allocator()) {
        *this = std::move(other);
    }

    List &operator=(const List &other) {
        Node *iter0 = first->next_;
        while (iter0 != last) {
            iter0 = iter0->next_;
            ATraits::destroy(alloc_, dynamic_cast<NodeData *>(iter0->prev_));
            ATraits::deallocate(alloc_, dynamic_cast<NodeData *>(iter0->prev_), 1);

        }
        last->prev_ = first;
        first->next_ = last;
        size_ = 0;
        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
            simple_alloc_ = other.get_allocator();
        }

        for (const auto &i:other) {
            push_back(i);
        }
        return *this;
    }

    List &operator=(List &&other) {
//        size_ = 0;
//        Node *iter = first->next_;
//        while (iter != last) {
//            iter = iter->next_;
//            ATraits::destroy(alloc_, dynamic_cast<NodeData *>(iter->prev_));
//            ATraits::deallocate(alloc_, dynamic_cast<NodeData *>(iter->prev_), 1);
//        }
//        STraits::destroy(sub_alloc_, first);
//        STraits::deallocate(sub_alloc_, first, 1);
//        STraits::destroy(sub_alloc_, last);
//        STraits::deallocate(sub_alloc_, last, 1);
//        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
//            simple_alloc_ = other.get_allocator();
//            alloc_ = other.alloc_;
//            sub_alloc_ = other.sub_alloc_;
//        }
//        first = other.first;
//        last = other.last;
//        List *new_list = new List(other.simple_alloc_);
//        other.first = new_list->first;
//        other.last = new_list->last;
//        new_list->last = nullptr;
//        new_list -> first = nullptr;
//        size_ = other.size_;
//        other.size_ = 0;
//        delete new_list;
        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
            std::swap(alloc_, other.alloc_);
            std::swap(sub_alloc_, other.sub_alloc_);
            std::swap(simple_alloc_, other.simple_alloc_);
        }

        std::swap(first, other.first);
        std::swap(last, other.last);
        std::swap(size_, other.size_);
        return *this;
    }

    const Allocator &get_allocator() const {
        return simple_alloc_;
    }

    bool empty() {
        return size_ == 0;
    }

    void push_back(const T &value) {
        ++size_;
        auto added = ATraits::allocate(alloc_, 1);
        ATraits::construct(alloc_, added, value);

        added->next_ = last;
        added->prev_ = last->prev_;
        last->prev_->next_ = added;
        last->prev_ = added;
    }

    template<class... Args>
    void emplace_back(Args &&... args) {
        ++size_;
        auto added = ATraits::allocate(alloc_, 1);
        ATraits::construct(alloc_, added, std::forward<Args>(args)...);

        added->next_ = last;
        added->prev_ = last->prev_;
        last->prev_->next_ = added;
        last->prev_ = added;
    }


    void push_front(const T &value) {
        ++size_;
        auto added = ATraits::allocate(alloc_, 1);
        ATraits::construct(alloc_, added, value);

        added->next_ = first->next_;
        added->prev_ = first;
        first->next_->prev_ = added;
        first->next_ = added;

    }

    void pop_back() {
        auto removed = dynamic_cast<NodeData *>(last->prev_);
        --size_;
        last->prev_ = removed->prev_;
        last->prev_->next_ = last;
        ATraits::destroy(alloc_, removed);
        ATraits::deallocate(alloc_, removed, 1);
    }

    void pop_front() {
        auto *removed = dynamic_cast<NodeData *>(first->next_);
        --size_;
        first->next_ = removed->next_;
        first->next_->prev_ = first;
        ATraits::destroy(alloc_, removed);
        ATraits::deallocate(alloc_, removed, 1);
    }

    template<bool isConst>
    class CommonIterator {
        using NodeType = typename std::conditional<isConst, const Node, Node>::type;
        using NodeDataType = typename std::conditional<isConst, const NodeData, NodeData>::type;
        using Type = typename std::conditional<isConst, const T, T>::type;
        mutable Node *current_;
    public:

        explicit CommonIterator(Node *node) : current_(node) {}

        CommonIterator<isConst>(const CommonIterator<isConst> &other);

        //explicit CommonIterator<true>(const CommonIterator<false>& other): current_(other.current_) {}
        CommonIterator &operator=(CommonIterator<isConst> other) {
            current_ = other.current_;
            return *this;
        }

        CommonIterator<isConst> &operator++() {
            current_ = current_->next_;
            return *this;
        }

        CommonIterator<isConst> &operator--() {
            current_ = current_->prev_;
            return *this;
        }

        CommonIterator<isConst> operator++(int) {
            CommonIterator<isConst> ans(*this);
            ++*this;
            return ans;
        }

        CommonIterator<isConst> operator--(int) {
            CommonIterator<isConst> ans(*this);
            --*this;
            return ans;
        }

        Type &operator*() {
            NodeDataType *data = dynamic_cast<NodeDataType *>(current_);
            return data->val_;
        }

        Type *operator->() {
            NodeDataType *data = dynamic_cast<NodeDataType *>(current_);
            return &(data->val_);
        }

        bool operator==(const CommonIterator<isConst> other) {
            return current_ == other.current_;
        }

        bool operator!=(const CommonIterator<isConst> other) {
            return !(*this == other);
        }


        typedef std::bidirectional_iterator_tag iterator_category;
        typedef CommonIterator<isConst> iterator_type;
        typedef Type value_type;
        typedef std::ptrdiff_t difference_type;
        typedef Type *pointer;
        typedef Type &reference;

        operator CommonIterator<true>() const {
            CommonIterator<true> ans(current_);
            return ans;
        }

    private:
        NodeData *getNodePtr() {
            return dynamic_cast<NodeData *>(current_);
        }

        template<class T_, class Allocator_>
        friend
        class List;
    };


    using iterator = CommonIterator<false>;
    using const_iterator = CommonIterator<true>;
    using reverse_iterator = std::reverse_iterator<CommonIterator<false>>;
    using const_reverse_iterator = std::reverse_iterator<CommonIterator<true>>;

    iterator begin() {
        iterator ans(first->next_);
        return ans;
    }

    iterator end() {
        return iterator(last);
    }

    const_iterator begin() const {
        return const_iterator(first->next_);
    }

    const_iterator end() const {
        return const_iterator(last);
    }

    const_iterator cbegin() const {
        return const_iterator(first->next_);
    }

    const_iterator cend() const {
        return const_iterator(last);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(end());
    }

    reverse_iterator rend() {
        return reverse_iterator(begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }

    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() {
        return const_reverse_iterator(cend());
    }

    const_reverse_iterator crend() {
        return const_reverse_iterator(cbegin());
    }

    void insert(iterator iter, const T &value) {
        insert(const_iterator(iter), value);
    }

    void insert(const_iterator iter, const T &value) {
        ++size_;
        auto added = ATraits::allocate(alloc_, 1);
        ATraits::construct(alloc_, added, value);
        NodeData *cur = iter.getNodePtr();
        added->next_ = cur;
        added->prev_ = cur->prev_;
        cur->prev_->next_ = added;
        cur->prev_ = added;
    }

    void erase(iterator iter) {
        --size_;
        NodeData *cur = iter.getNodePtr();
        cur->prev_->next_ = cur->next_;
        cur->next_->prev_ = cur->prev_;
        ATraits::destroy(alloc_, cur);
        ATraits::deallocate(alloc_, cur, 1);
    }

    void erase(const_iterator iter) {
        --size_;
        NodeData *cur = iter.getNodePtr();
        cur->prev_->next_ = cur->next_;
        cur->next_->prev_ = cur->prev_;
        ATraits::destroy(alloc_, cur);
        ATraits::deallocate(alloc_, cur, 1);
    }

    size_t size() const {
        return size_;
    };

    void splice(const_iterator pos_it, List<T, Allocator> &list, const_iterator beg_it, const_iterator end_it,
                bool recalc_size = true) {
        Node *after = pos_it.current_;
        Node *before = (--pos_it).current_;
        Node *beg = beg_it.getNodePtr();
        Node *end = (--end_it).getNodePtr();
        beg->prev_->next_ = end->next_;
        end->next_->prev_ = beg->prev_;
        before->next_ = beg;
        beg->prev_ = before;
        after->prev_ = end;
        end->next_ = after;
        if (recalc_size) {
            int i = 1;
            auto *ptr = beg;
            while (ptr != end) {
                ++i;
                ptr = ptr->next_;
            }
            list.size_ -= i;
            size_ += i;
        }
    }

    struct Node {
        Node *prev_ = nullptr;
        Node *next_ = nullptr;

        Node() = default;

        Node(Node *prev, Node *next) : prev_(prev), next_(next) {}

        virtual ~Node() = default;
    };

    struct NodeData : public Node {
        T val_;

        NodeData() = default;

        NodeData(const T &val, NodeData *prev = nullptr, NodeData *next = nullptr) : Node(prev, next), val_(val) {}

        template<class... Args>
        NodeData(Args &&... args): Node(nullptr, nullptr), val_(std::forward<Args>(args)...) {}

//        NodeData(T&& val): Node(nullptr, nullptr), val_(std::forward<T>(val)){}

        //NodeData(const NodeData& other) = default;
        ~NodeData() = default;

        using Type = decltype(val_);
    };

    ~List() {
        if (first != nullptr || last != nullptr) {
            size_ = 0;
            Node *iter = first->next_;
            while (iter != last) {
                iter = iter->next_;
                ATraits::destroy(alloc_, dynamic_cast<NodeData *>(iter->prev_));
                ATraits::deallocate(alloc_, dynamic_cast<NodeData *>(iter->prev_), 1);
            }
            STraits::destroy(sub_alloc_, first);
            STraits::deallocate(sub_alloc_, first, 1);
            STraits::destroy(sub_alloc_, last);
            STraits::deallocate(sub_alloc_, last, 1);
        }
    }


    using FullNodeAlloc = typename Allocator::template rebind<NodeData>::other;
    using SubNodeAlloc = typename Allocator::template rebind<Node>::other;
    using ATraits = std::allocator_traits<FullNodeAlloc>;
    using STraits = std::allocator_traits<SubNodeAlloc>;

private:
    FullNodeAlloc alloc_;
    SubNodeAlloc sub_alloc_;
    Allocator simple_alloc_;

    mutable Node *first;
    mutable Node *last;
    size_t size_ = 0;

    void empl_back() {
        ++size_;
        auto added = ATraits::allocate(alloc_, 1);
        ATraits::construct(alloc_, added);

        added->next_ = last;
        added->prev_ = last->prev_;
        last->prev_->next_ = added;
        last->prev_ = added;

    }
};

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::CommonIterator<isConst>::CommonIterator(const List::CommonIterator<isConst> &other): current_(
        other.current_) {}

template<class Key, class Value, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>, class Alloc = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
    using ListT = List<std::pair<const Key, Value>, Alloc>;
    typedef size_t SizeT;
    using AHelper = typename Alloc::template rebind<std::pair<Key, Value>>::other;
    using ATraits = std::allocator_traits<AHelper>;
public:
    typedef typename ListT::iterator iterator;
    typedef typename ListT::const_iterator const_iterator;
    typedef std::pair<const Key, Value> NodeType;

    UnorderedMap(const Alloc &allocator = Alloc()) : list(allocator),
                                                     table_(10, std::make_pair(list.end(), list.end())), hash(),
                                                     equal(), max_load_factor_(1.5),
                                                     constructor(nullptr){}

    UnorderedMap(const UnorderedMap &other) : list(other.list),
                                              table_(other.table_.size(), std::make_pair(list.end(), list.end())),
                                              hash(other.hash), equal(other.equal),
                                              max_load_factor_(other.max_load_factor_),
                                              constructor(nullptr){
        SizeT previous, current;
        previous = table_.size();
        auto size = table_.size();
        for (auto i = other.list.begin(); i != other.list.end(); ++i) {
            current = hash(i->first) % size;
            if (current != previous) {
                list.push_back(*i);
                if (previous != size) {
                    table_[current].first = --list.end();
                    table_[previous].second = (table_[current].first);
                    --table_[previous].second;
                }
            }
            previous = current;
        }
        if (previous != size) {
            auto it = list.end();
            table_[previous].second = std::move(--it);
        }
    }

    UnorderedMap(UnorderedMap &&other) : list(std::move(other.list)), table_(std::move(other.table_)), hash(other.hash),
                                         equal(other.equal),
                                         max_load_factor_(other.max_load_factor_), constructor(nullptr) {

    };

    UnorderedMap &operator=(UnorderedMap &&other) {
        std::swap(list, other.list);
        std::swap(table_, other.table_);
        std::swap(hash, other.hash);
        std::swap(equal, other.equal);
        std::swap(max_load_factor_, other.max_load_factor_);
        return *this;
    }

    UnorderedMap &operator=(const UnorderedMap &other) {
        UnorderedMap map_new = other;
        *(this) = std::move(other);
        return *this;
    }


    template<class... Args>
    std::pair<typename ListT::iterator, bool> emplace(Args &&... args) {
        if (static_cast<float>(list.size() + 1) / table_.size() > max_load_factor_) {
            reserve(list.size() + 1);
        }
        if (constructor == nullptr) {
            constructor = ATraits::allocate(alloc_, 1);
        }
        ATraits::construct(alloc_, constructor, std::forward<Args>(args)...);
        list.emplace_back(std::move(*constructor));
        ATraits::destroy(alloc_, constructor);
        auto iter = list.end();
        --iter;
        SizeT place = hash(iter->first) % table_.size();

        if (table_[place].first != list.end()) {
            auto end = table_[place].second;
            ++end;
            for (auto i = table_[place].first; i != end; ++i) {
                if (equal(i->first, iter->first)) {
                    list.pop_back();
                    return {i, false};
                }
            }
            list.splice(table_[place].first, list, --list.end(), list.end(), false);
            return std::make_pair(--(table_[place].first), true);
        } else {
            --table_[place].first;
            --table_[place].second;
            return std::make_pair(table_[place].first, true);
        }

    }

    std::pair<typename ListT::iterator, bool> insert(const std::pair<Key, Value> &node) {
        SizeT place = hash(node.first) % table_.size();

        iterator elem = find_in(place, node.first);
        if (elem != list.end()) {
            if (load_factor() > max_load_factor_) {
                reserve(list.size());
            }
            return {elem, false};
        }
        if (table_[place].first != list.end()) {
            list.emplace_back(node);
            list.splice(table_[place].first, list, --list.end(), list.end(), false);
            if (load_factor() > max_load_factor_) {
                reserve(list.size());
            }
            return std::make_pair(--(table_[place].first), true);
        } else {
            --table_[place].first;
            --table_[place].second;
            if (load_factor() > max_load_factor_) {
                reserve(list.size());
            }
            return std::make_pair(table_[place].first, true);
        }
    }

    std::pair<typename ListT::iterator, bool> insert(std::pair<Key, Value> &&node) {
        Key key = std::move(node.first);
        Value val = std::move(node.second);
        SizeT place = hash(key) % table_.size();

        iterator elem = find_in(place, key);
        if (elem != list.end()) {
            return {elem, false};
            if (load_factor() > max_load_factor_) {
                reserve(list.size());
            }
        }
        list.emplace_back(std::move(key), std::move(val));
        if (table_[place].first != list.end()) {
            list.splice(table_[place].first, list, --list.end(), list.end(), false);
            if (load_factor() > max_load_factor_) {
                reserve(list.size());
            }
            return std::make_pair(--(table_[place].first), true);
        } else {
            --table_[place].first;
            --table_[place].second;
            if (load_factor() > max_load_factor_) {
                reserve(list.size());
            }
            return std::make_pair(table_[place].first, true);
        }
    }

    Value &operator[](const Key &key) {
        auto iter = find_in(hash(key) % table_.size(), key);
        if (iter == list.end()) {
            auto &ans = (insert(std::move(std::make_pair(key, Value()))).first)->second;
            if (load_factor() > max_load_factor_) {
                reserve(list.size());
            }
            return ans;
        } else {
            return iter->second;
        }
    }

    Value &at(const Key &key) {
        auto iter = find_in(hash(key) % table_.size(), key);
        if (iter == list.end()) {
            throw std::out_of_range("there is no value for this key");
        } else {
            return iter->second;
        }
    }

    const Value &at(const Key &key) const {
        auto iter = find_in(hash(key) % table_.size(), key);
        if (iter == list.end()) {
            throw std::out_of_range("there is no value for this key");
        } else {
            return iter->second;
        }
    }

    template<typename InputIter>
    void insert(InputIter begin, InputIter end) {
        static_assert(std::is_convertible_v<typename InputIter::value_type &, const NodeType &>,
                      "iterators correctness");
        for (auto i = begin; i != end; ++i) {
            insert(*i);
        }
        if (load_factor() > max_load_factor_) {
            reserve(list.size());
        }
    }

    void erase(iterator iter) {
        SizeT place = hash(iter->first) % table_.size();
        if (table_[place].first == table_[place].second) {
            table_[place].first = list.end();
            table_[place].second = list.end();
            list.erase(iter);
            return;
        }
        if (table_[place].first == iter) {
            ++table_[place].first;
            list.erase(iter);
            return;
        }
        if (table_[place].second == iter) {
            --table_[place].second;
            list.erase(iter);
            return;
        }
    }

    void erase(iterator begin, iterator end) {
        for (auto i = begin; i != end; ++i) {
            erase(i);
        }
    }

    iterator find(const Key &key) {
        return find_in(hash(key) % table_.size(), key);
    }

    const_iterator find(const Key &key) const {
        return find_in(hash(key) % table_.size(), key);
    }

    void rehash(size_t size) {
        //std::cerr << list.size() << '\t' << table_.size() << '\t' << clock() << '\n';
        ListT list_new;
        std::vector<std::pair<iterator, iterator>> table_new(size, std::make_pair(list_new.end(), list_new.end()));
        SizeT place;
        while (list.size() > 0) {
            auto iter = list.begin();
            auto end = iter;
            ++end;
            place = hash(iter->first);
            list_new.splice(table_new[place % size].first, list, iter, end);
            if (table_new[place % size].first == list_new.end()) {
                --table_new[place % size].first;
                --table_new[place % size].second;
            } else {
                --table_new[place % size].first;
            }
        }
        table_ = std::move(table_new);
        list = std::move(list_new);
        //std::cerr << list.size() << '\t' << table_.size() << '\t' << clock() << '\n';
    }

    size_t size() {
        return list.size();
    }

    float load_factor() {
        return static_cast<float>(size()) / table_.size();
    }

    [[nodiscard]] float max_load_factor() const {
        return max_load_factor_;
    }

    void max_load_factor(float value) {
        max_load_factor_ = value;
        if (load_factor() > max_load_factor_) {
            rehash(static_cast<size_t>(list.size() * max_load_factor_ * 2) + 1);
        }
    }

    void reserve(size_t size) {
        if (static_cast<float>(size) / table_.size() > max_load_factor_) {
            rehash(static_cast<size_t>(size * max_load_factor_ * 2) + 1);
        }
    }

    iterator begin() {
        return list.begin();
    }

    iterator end() {
        return list.end();
    }

    const_iterator begin() const {
        return list.begin();
    }

    const_iterator end() const {
        return list.end();
    }

    const_iterator cbegin() const {
        return list.cbegin();
    }

    const_iterator cend() const {
        return list.cend();
    }

    ~UnorderedMap() {
        if (constructor != nullptr) {
            ATraits::deallocate(alloc_, constructor, 1);
        }
    }

private:

    iterator find_in(SizeT place, const Key &key) {
        auto end = table_[place].second;
        if (end == list.end()) {
            return list.end();
        }
        ++end;

        for (auto i = table_[place].first; i != end; ++i) {
            if (equal(i->first, key)) {
                return i;
            }
        }
        return list.end();
    }

    const_iterator find_in(SizeT place, const Key &key) const {
        auto end = table_[place].second;
        if (end == list.end()) {
            return list.end();
        }
        ++end;

        for (auto i = table_[place].first; i != end; ++i) {
            if (equal(i->first, key)) {
                return i;
            }
        }
        return list.end();
    }


    ListT list;
    std::vector<std::pair<typename ListT::iterator, typename ListT::iterator>> table_;
    Hash hash;
    Equal equal;
    float max_load_factor_;
    AHelper alloc_;
    std::pair<Key, Value> *constructor = nullptr;
};
