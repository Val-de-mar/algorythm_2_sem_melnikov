#include <vector>
#include <memory>

template<typename ...Args>
bool check(Args... args) {
    return false;
}

template<>
bool check(char x) {
    return x == '5';
}

template<size_t chunkSize>
class FixedAllocator {
    std::vector<void *> allocated;
public:

    FixedAllocator() = default;

    FixedAllocator(const FixedAllocator &) : FixedAllocator() {}

    FixedAllocator &operator=(const FixedAllocator &) {
        return *this;
    }

    void *allocate() {
        allocated.push_back(::operator new(chunkSize));
        return allocated.back();
    }

    void deallocate(void*, int) {
        //::operator delete(ptr, size);
    }

    bool operator==(const FixedAllocator &) const {
        return false;
    }

    bool operator!=(const FixedAllocator &other) const {
        return !(*this == other);
    }

    ~FixedAllocator() {
        for (size_t i = 0; i < allocated.size(); ++i) {
            ::operator delete (allocated[i], chunkSize);
        }
    }
};


template<typename T>
class FastAllocator {
    const static int Size = 3000;
    FixedAllocator<Size * sizeof(T)> alloc;
    T *cur_;
    int place = Size;
public:

    T *allocate(int n) {
        if (n <= Size) {
            if (place + n <= Size) {
                T *ans = cur_;
                ans += place;
                place += n;
                return ans;
            } else {
                if (place != Size) {
                    T *ans = cur_;
                    ans += place;
                    //alloc.deallocate(static_cast<void *>(ans), (Size - place) * sizeof(T));
                }
                place = 0;
                cur_ = static_cast<T *>(alloc.allocate());
                place += n;
                return cur_;
            }
        } else {
            return static_cast<T *>(::operator new(n * sizeof(T)));
        }
    }

    void deallocate(T *ptr, int n) {
        if (n < Size) {
            alloc.deallocate(static_cast<void *>(ptr), n * sizeof(T));
        } else {
            ::operator delete (static_cast<void *>(ptr), n * sizeof(T));
        }
    }

    template<typename ...Args>
    void construct(T *ptr, Args &&... args) {
        new(ptr) T(std::forward<Args>(args)...);
    }

    void destroy(T *ptr) {
        ptr->~T();
    }

    typedef T value_type;
    typedef FastAllocator<T> allocator_type;
    typedef T *pointer;
    typedef typename std::pointer_traits<pointer>::template rebind<const value_type> const_pointer;
    typedef typename std::pointer_traits<pointer>::template rebind<void> void_pointer;
    typedef typename std::pointer_traits<pointer>::template rebind<const void> const_void_pointer;
    typedef typename std::pointer_traits<pointer>::difference_type difference_type;
    typedef typename std::make_unsigned<difference_type>::type size_type;
    template<class U>
    struct rebind {
        typedef FastAllocator<U> other;
    };

    explicit operator FastAllocator<typename std::remove_pointer<T>::type>() const {
        return FastAllocator<typename std::remove_pointer<T>::type>();
    }

    explicit operator FastAllocator<T *>() const {
        return FastAllocator<T *>();
    }

};

template<typename T, typename Allocator = FastAllocator<T>>
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

    void empl_back() {
        ++size_;
        auto added = ATraits::allocate(alloc_, 1);
        ATraits::construct(alloc_, added);

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

        NodeData *getNodePtr() {
            return dynamic_cast<NodeData *>(current_);
        }

        typedef std::bidirectional_iterator_tag iterator_category;
        typedef CommonIterator<isConst> iterator_type;
        typedef Type value_type;
        typedef std::ptrdiff_t difference_type;
        typedef Type *pointer;
        typedef Type &reference;

        explicit operator CommonIterator<true>() const {
            CommonIterator<true> ans(current_);
            return ans;
        }

    private:

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

    const_iterator cbegin() {
        return const_iterator (first->next_);
    }

    const_iterator cend() {
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

        //NodeData(const NodeData& other) = default;
        ~NodeData() = default;

        using Type = decltype(val_);
    };

    ~List() {
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


    using FullNodeAlloc = typename Allocator::template rebind<NodeData>::other;
    using SubNodeAlloc = typename Allocator::template rebind<Node>::other;
    using ATraits = std::allocator_traits<FullNodeAlloc>;
    using STraits = std::allocator_traits<SubNodeAlloc>;
    FullNodeAlloc alloc_;
    SubNodeAlloc sub_alloc_;
    Allocator simple_alloc_;

    mutable Node *first;
    mutable Node *last;
    size_t size_ = 0;
private:

};

template<typename T, typename Allocator>
template<bool isConst>
List<T, Allocator>::CommonIterator<isConst>::CommonIterator(const List::CommonIterator<isConst> &other): current_(
        other.current_) {}

