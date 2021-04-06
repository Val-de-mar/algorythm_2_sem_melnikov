#include <vector>

//typedef int T;

template<typename T>
class Deque {
private:
    struct BucketInit {
        BucketInit(size_t size, std::allocator<T> &allocator) : size_(size), alloc(allocator) {}

        const size_t size_;
        std::allocator<T> &alloc;
    };

    struct FictiveBucketInit {
        FictiveBucketInit(size_t size, std::allocator<T> &allocator) : size_(size), alloc(allocator) {}

        const size_t size_;
        std::allocator<T> &alloc;
    };

    class Bucket {
        const size_t size_;
        std::allocator<T> alloc;
        T *ptr_;
        T *left = ptr_ + size_;
        T *right = ptr_ + size_;
    public:
        Bucket(size_t bucket_size, std::allocator<T> &allocator) : size_(bucket_size), alloc(allocator),
                                                                   ptr_(alloc.allocate(size_)) {
            ptr_ = alloc.allocate(size_);
            left = ptr_ + size_;
            right = left;
            //std::cout << "allocated ptr_ " << this << std::endl;
        }

        Bucket(const BucketInit &bucket) : size_(bucket.size_), alloc(bucket.alloc), ptr_(nullptr) {
            ptr_ = alloc.allocate(size_);
            left = ptr_ + size_;
            right = left;
            //std::cout << "allocated ptr_ " << this << std::endl;
        }

        Bucket(const Bucket &other) : size_(other.size_), alloc(other.alloc) {
            ptr_ = alloc.allocate(size_);
            left = ptr_ + (other.left - other.ptr_);
            right = ptr_ + (other.right - other.ptr_);
            for (T *i = left; i < right; ++i) {
                alloc.construct(i, other[(i - ptr_)]);
            }
        }

        Bucket(FictiveBucketInit other) : size_(other.size_), alloc(other.alloc), ptr_(nullptr), left(nullptr),
                                          right(nullptr) {}

        Bucket() : size_(0), alloc(), ptr_(nullptr), left(nullptr), right(nullptr) {}

        Bucket(Bucket &&other) noexcept: size_(other.size_), alloc(other.alloc), ptr_(other.ptr_), left(other.left),
                                         right(other.right) {
            other.ptr_ = nullptr;
            other.left = nullptr;
            other.right = nullptr;
        }

        void sameSizes(const Bucket &other) {
            left = ptr_ + (other.left - other.ptr_);
            right = ptr_ + (other.right - other.ptr_);
        }

        void swap(Bucket &other) {
            std::swap(ptr_, other.ptr_);
            std::swap(left, other.left);
            std::swap(right, other.right);
        }

        Bucket &operator=(Bucket &&other) noexcept {
            std::swap(ptr_, other.ptr_);
            std::swap(left, other.left);
            std::swap(right, other.right);
            return *this;
        }

        Bucket &operator=(const Bucket &other) noexcept {
            if (ptr_ != nullptr) {
                for (T *i = left; i < right; ++i) {
                    alloc.destroy(i);
                }
            }
            left = ptr_ + (other.left - other.ptr_);
            right = ptr_ + (other.right - other.ptr_);
            for (T *i = left; i < right; ++i) {
                alloc.construct(i, other[(i - ptr_)]);
            }
            return *this;
        }

        T *pointer(size_t place) {
            return ptr_ + place;
        }

        T &operator[](size_t k) {
            //std::cout << *(ptr_ + k) << std::endl;
            return *(ptr_ + k);
        }

        const T &operator[](size_t k) const {
            return *(ptr_ + k);
        }

        void place(int begin, int end) {
            left = ptr_ + begin;
            right = ptr_ + end;
            T *pointer_ = ptr_ + begin;
            try {
                for (; pointer_ < right; ++pointer_) {
                    alloc.construct(pointer_);
                }
            } catch (...) {
                --pointer_;
                {
                    while (pointer_ >= left) {
                        alloc.destroy(pointer_);
                        --pointer_;
                    }
                    //std::cout << "\n place \t"<< ptr_ << std::endl;
                }
                throw;
            }
        }

        void set_edges(int place) {
            left = ptr_ + place;
            right = ptr_ + place;
        }

        void place(int begin, int end, const T &value) {
            left = ptr_ + begin;
            right = ptr_ + end;
            T *pointer_ = ptr_ + begin;
            try {
                for (; pointer_ < right; ++pointer_) {
                    alloc.construct(pointer_, value);
                }
            } catch (...) {
                --pointer_;
                {
                    while (pointer_ >= left) {
                        alloc.destroy(pointer_);
                        --pointer_;
                    }
                    //std::cout << "\n place_val \t"<< ptr_ << std::endl;
                }
                throw;
            }

//            for (T* i = left; i < right; ++i) {
//                std::cout << *i << "\t";
//            }
//            std::cout << std::endl;
//            std::cout << right - left << "\n";
        }

        ~Bucket() {
            //std::cout << "\n ~Bucket \t"<< ptr_ << std::endl;
            if (ptr_ != nullptr) {
                for (T *i = left; i < right; ++i) {
                    alloc.destroy(i);
                }
                alloc.deallocate(ptr_, size_);
            }
        }

        T *begin() {
            return left;
        }

        T *end() {
            return right;
        }

        T *rightWall() {
            return ptr_ + size_;
        }

        T *leftWall() {
            return ptr_;
        }

        [[nodiscard]] const T *begin() const {
            return left;
        }

        [[nodiscard]] const T *end() const {
            return right;
        }

        [[nodiscard]] const T *rightWall() const {
            return ptr_ + size_;
        }

        [[nodiscard]] const T *leftWall() const {
            return ptr_;
        }

        void push_back(const T &val) {
            alloc.construct(right, val);
            ++right;
        };

        void pop_back() {
            alloc.destroy(right - 1);
            --right;
        }

        void push_front(const T &val) {
            alloc.construct(left - 1, val);
            --left;
        }

        void pop_front() {
            alloc.destroy(left);
            ++left;
        }
    };

    [[nodiscard]] std::pair<size_t, size_t> bucketAndPlace(int place) const {
        place += left_place_;
        size_t current_bucket = left_bucket_;
//        place -= (bucket_size_ - left_place_);
//        ++current_bucket;
        if (place >= 0) {
            current_bucket += (place / bucket_size_);
            place %= bucket_size_;
        } else {
            current_bucket += (place / bucket_size_);
            place %= bucket_size_;
            if (place < 0) {
                --current_bucket;
                place += bucket_size_;
            }
        }
        auto ans = std::make_pair(current_bucket, place);
        return ans;
    }


    [[nodiscard]] int numberFromBucketAndPtr(size_t bucket, const T *ptr) const {
        return ((int(bucket) - int(left_bucket_)) * bucket_size_) + (ptr - data_[bucket].leftWall()) - left_place_;
    }

    size_t bucket_size_ = 10;
    std::allocator<T> alloc;
    size_t left_place_;
    size_t right_place_;
    size_t left_bucket_;
    size_t right_bucket_;
    std::vector<Bucket> data_;

public:
    class iterator {
        Deque &deque_;
        size_t place_of_bucket_;
        T *left_;
        T *right_;
        T *pointer_;
    public:
        iterator(Deque &deque, size_t place_of_bucket, T *left, T *right, T *pointer) : deque_(deque),
                                                                                        place_of_bucket_(
                                                                                                place_of_bucket),
                                                                                        left_(left),
                                                                                        right_(right),
                                                                                        pointer_(
                                                                                                pointer) {}

        iterator(const iterator &other) : deque_(other.deque_), place_of_bucket_(other.place_of_bucket_),
                                          left_(other.left_), right_(other.right_), pointer_(other.pointer_) {}

        T &operator*() {
            return *pointer_;
        }

        T *operator->() {
            return pointer_;
        }

        iterator &operator++() {
            *this += 1;
            return *this;
        }

        iterator &operator--() {
            *this -= 1;
            return *this;
        }

        iterator &operator+=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) + delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        iterator &operator-=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) - delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        iterator operator+(int delta) const {
            iterator ans = *this;
            ans += delta;
            return ans;
        }

        iterator operator-(int delta) const {
            iterator ans = *this;
            ans -= delta;
            return ans;
        }

        iterator &operator=(const iterator &other) {
            place_of_bucket_ = other.place_of_bucket_;
            left_ = other.left_;
            right_ = other.right_;
            pointer_ = other.pointer_;
            return *this;
        }

        int operator-(const iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 - x2);
        }

        bool operator>(const iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 > x2);
        }

        bool operator<(const iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 < x2);
        }

        bool operator>=(const iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 >= x2);
        }

        bool operator<=(const iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 <= x2);
        }


        bool operator==(const iterator &other) const {
            return pointer_ == other.pointer_;
        }

        bool operator!=(const iterator &other) const {
            return !(*this == other);
        }


        T *pointer() {
            return pointer_;
        }

        const T *pointer() const {
            return const_cast<const T *>(pointer_);
        }

        void debug() {
            std::cout << deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
        }

        friend Deque;
    };


    class reverse_iterator {
        Deque &deque_;
        size_t place_of_bucket_;
        T *left_;
        T *right_;
        T *pointer_;
    public:
        reverse_iterator(Deque &deque, size_t place_of_bucket, T *left, T *right, T *pointer) : deque_(deque),
                                                                                                place_of_bucket_(
                                                                                                        place_of_bucket),
                                                                                                left_(left),
                                                                                                right_(right),
                                                                                                pointer_(
                                                                                                        pointer) {}

        reverse_iterator(const reverse_iterator &other) : deque_(other.deque_),
                                                          place_of_bucket_(other.place_of_bucket_),
                                                          left_(other.left_), right_(other.right_),
                                                          pointer_(other.pointer_) {}

        T &operator*() {
            return *pointer_;
        }

        T *operator->() {
            return pointer_;
        }

        reverse_iterator &operator++() {
            *this += 1;
            return *this;
        }

        reverse_iterator &operator--() {
            *this -= 1;
            return *this;
        }

        reverse_iterator &operator-=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) + delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        reverse_iterator &operator+=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) - delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        reverse_iterator operator+(int delta) const {
            reverse_iterator ans = *this;
            ans += delta;
            return ans;
        }

        reverse_iterator operator-(int delta) const {
            reverse_iterator ans = *this;
            ans -= delta;
            return ans;
        }

        reverse_iterator &operator=(const reverse_iterator &other) {
            place_of_bucket_ = other.place_of_bucket_;
            left_ = other.left_;
            right_ = other.right_;
            pointer_ = other.pointer_;
            return *this;
        }

        int operator-(const reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 - x2);
        }

        bool operator>(const reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 > x2);
        }

        bool operator<(const reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 < x2);
        }

        bool operator>=(const reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 >= x2);
        }

        bool operator<=(const reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 <= x2);
        }


        bool operator==(const reverse_iterator &other) const {
            return pointer_ == other.pointer_;
        }

        bool operator!=(const reverse_iterator &other) const {
            return !(*this == other);
        }


        T *pointer() {
            return pointer_;
        }

        const T *pointer() const {
            return const_cast<const T *>(pointer_);
        }

        void debug() {
            std::cout << deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
        }

        friend Deque;
    };


    class const_iterator {
        const Deque &deque_;
        size_t place_of_bucket_;
        const T *left_;
        const T *right_;
        const T *pointer_;
    public:
        const_iterator(const Deque &deque, size_t place_of_bucket, const T *left, const T *right, const T *pointer) : deque_(deque),
                                                                                              place_of_bucket_(
                                                                                                      place_of_bucket),
                                                                                              left_(left),
                                                                                              right_(right),
                                                                                              pointer_(
                                                                                                      pointer) {}

        const_iterator(const const_iterator &other) : deque_(other.deque_), place_of_bucket_(other.place_of_bucket_),
                                                      left_(other.left_), right_(other.right_),
                                                      pointer_(other.pointer_) {}

        const_iterator(const iterator &other) : deque_(other.deque_), place_of_bucket_(other.place_of_bucket_),
                                                left_(other.left_), right_(other.right_), pointer_(other.pointer_) {}


        const T &operator*() {
            return *pointer_;
        }

        const T *operator->() {
            return pointer_;
        }

        const_iterator &operator++() {
            *this += 1;
            return *this;
        }

        const_iterator &operator--() {
            *this -= 1;
            return *this;
        }

        const_iterator &operator+=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) + delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        const_iterator &operator-=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) - delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        const_iterator operator+(int delta) const {
            const_iterator ans = *this;
            ans += delta;
            return ans;
        }

        const_iterator operator-(int delta) const {
            const_iterator ans = *this;
            ans -= delta;
            return ans;
        }

        const_iterator &operator=(const const_iterator &other) {
            place_of_bucket_ = other.place_of_bucket_;
            left_ = other.left_;
            right_ = other.right_;
            pointer_ = other.pointer_;
            return *this;
        }

        int operator-(const const_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 - x2);
        }

        bool operator>(const const_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 > x2);
        }

        bool operator<(const const_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 < x2);
        }

        bool operator>=(const const_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 >= x2);
        }

        bool operator<=(const const_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 <= x2);
        }


        bool operator==(const const_iterator &other) const {
            return pointer_ == other.pointer_;
        }

        bool operator!=(const const_iterator &other) const {
            return !(*this == other);
        }


        const T *pointer() {
            return pointer_;
        }

        const T *pointer() const {
            return const_cast<const T *>(pointer_);
        }

        void debug() {
            std::cout << deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
        }

        friend Deque;
    };


    class const_reverse_iterator {
        const Deque &deque_;
        size_t place_of_bucket_;
        const T *left_;
        const T *right_;
        const T *pointer_;
    public:
        const_reverse_iterator(const Deque &deque, size_t place_of_bucket, const T *left, const T *right, const T *pointer) : deque_(deque),
                                                                                                      place_of_bucket_(
                                                                                                              place_of_bucket),
                                                                                                      left_(left),
                                                                                                      right_(right),
                                                                                                      pointer_(
                                                                                                              pointer) {}

        const_reverse_iterator(const const_reverse_iterator &other) : deque_(other.deque_),
                                                                      place_of_bucket_(other.place_of_bucket_),
                                                                      left_(other.left_), right_(other.right_),
                                                                      pointer_(other.pointer_) {}

        const_reverse_iterator(const reverse_iterator &other) : deque_(other.deque_),
                                                                place_of_bucket_(other.place_of_bucket_),
                                                                left_(other.left_), right_(other.right_),
                                                                pointer_(other.pointer_) {}

        const T &operator*() {
            return *pointer_;
        }

        const T *operator->() {
            return pointer_;
        }

        const_reverse_iterator &operator++() {
            *this += 1;
            return *this;
        }

        const_reverse_iterator &operator--() {
            *this -= 1;
            return *this;
        }

        const_reverse_iterator &operator-=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) + delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        const_reverse_iterator &operator+=(int delta) {
            int new_place = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_) - delta;
            auto new_coordinates = deque_.bucketAndPlace(new_place);
            place_of_bucket_ = new_coordinates.first;
            left_ = deque_.data_[place_of_bucket_].begin();
            right_ = deque_.data_[place_of_bucket_].end();
            pointer_ = deque_.data_[place_of_bucket_].leftWall() + new_coordinates.second;
            return *this;
        }

        const_reverse_iterator operator+(int delta) const {
            const_reverse_iterator ans = *this;
            ans += delta;
            return ans;
        }

        const_reverse_iterator operator-(int delta) const {
            const_reverse_iterator ans = *this;
            ans -= delta;
            return ans;
        }

        const_reverse_iterator &operator=(const const_reverse_iterator &other) {
            place_of_bucket_ = other.place_of_bucket_;
            left_ = other.left_;
            right_ = other.right_;
            pointer_ = other.pointer_;
            return *this;
        }

        int operator-(const const_reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 - x2);
        }

        bool operator>(const const_reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 > x2);
        }

        bool operator<(const const_reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 < x2);
        }

        bool operator>=(const const_reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 >= x2);
        }

        bool operator<=(const const_reverse_iterator &other) const {
            int x1 = deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
            int x2 = deque_.numberFromBucketAndPtr(other.place_of_bucket_, other.pointer_);
            return (x1 <= x2);
        }


        bool operator==(const const_reverse_iterator &other) const {
            return pointer_ == other.pointer_;
        }

        bool operator!=(const const_reverse_iterator &other) const {
            return !(*this == other);
        }


        const T *pointer() {
            return pointer_;
        }

        const T *pointer() const {
            return const_cast<const T *>(pointer_);
        }

        void debug() {
            std::cout << deque_.numberFromBucketAndPtr(place_of_bucket_, pointer_);
        }

        friend Deque;
    };


    Deque(int size) : alloc(std::allocator<T>()), left_place_(0),
                          right_place_(0), left_bucket_(0),
                          right_bucket_(0), data_((size + 1) / bucket_size_ + 1, BucketInit(bucket_size_, alloc)) {
        left_bucket_ = 0;
        left_place_ = 1;
        right_bucket_ = 0;
        right_place_ = 1;
        {
            int elements_num = std::min(size, static_cast<int>(bucket_size_ - 1));
            data_[0].place(1, elements_num + 1);
            size -= bucket_size_ - 1;
            right_place_ = 1 + elements_num;
            if (right_place_ == bucket_size_) {
                right_place_ = 0;
                ++right_bucket_;
            }
        }
        for (size_t i = 1; size > 0 && i < data_.size(); ++i) {
            int elements_num = std::min(size, static_cast<int>(bucket_size_));
            data_[i].place(0, elements_num);
            size -= bucket_size_;
            right_bucket_ = i;
            right_place_ = elements_num;
            if (right_place_ == bucket_size_) {
                right_place_ = 0;
                ++right_bucket_;
            }
        }

    }

    Deque() : alloc(std::allocator<T>()), left_place_(0),
                          right_place_(0), left_bucket_(0),
                          right_bucket_(0), data_(1, BucketInit(bucket_size_, alloc)) {
        left_bucket_ = 0;
        left_place_ = 1;
        right_bucket_ = 0;
        right_place_ = 1;
        data_[0].set_edges(1);

    }

    Deque(int size, const T &value) : alloc(std::allocator<T>()), left_place_(0),
                                      right_place_(0), left_bucket_(0),
                                      right_bucket_(0),
                                      data_((size + 1) / bucket_size_ + 1, BucketInit(bucket_size_, alloc)) {
        left_bucket_ = 0;
        left_place_ = 1;
        right_bucket_ = 0;
        right_place_ = 1;
        {
            int elements_num = std::min(size, static_cast<int>(bucket_size_ - 1));
            data_[0].place(1, elements_num + 1, value);
            size -= bucket_size_ - 1;
            right_place_ = 1 + elements_num;
            if (right_place_ == bucket_size_) {
                right_place_ = 0;
                ++right_bucket_;
            }
        }
        for (size_t i = 1; size > 0 && i < data_.size(); ++i) {
            int elements_num = std::min(size, static_cast<int>(bucket_size_));
            data_[i].place(0, elements_num, value);
            size -= bucket_size_;
            right_bucket_ = i;
            right_place_ = elements_num;
            if (right_place_ == bucket_size_) {
                right_place_ = 0;
                ++right_bucket_;
            }
        }

    }

    Deque(const Deque &other) : bucket_size_(other.bucket_size_), alloc(other.alloc), left_place_(other.left_place_),
                                right_place_(other.right_place_), left_bucket_(other.left_bucket_),
                                right_bucket_(other.right_bucket_), data_(other.data_) {}

    Deque &operator=(const Deque &other) {
        data_.clear();
        data_.resize(other.data_.size(), FictiveBucketInit(bucket_size_, alloc));
        for (size_t i = 0; i < other.data_.size(); ++i) {
            data_[i] = other.data_[i];
        }
        left_bucket_ = other.left_bucket_;
        left_place_ = other.left_place_;
        right_bucket_ = other.right_bucket_;
        right_place_ = other.right_place_;
        return *this;
    }

    Bucket &bucket_access(size_t place) {
        return data_[place];
    }

    T &operator[](size_t place) {
        if (place + left_place_ < bucket_size_) {
            T &ans = data_[left_bucket_][left_place_ + place];
            return ans;
        }
        size_t current_bucket = left_bucket_;
        place -= (bucket_size_ - left_place_);
        ++current_bucket;
        current_bucket += (place / bucket_size_);
        place %= bucket_size_;
        return data_[current_bucket][place];
    }

    const T &operator[](size_t place) const {
        if (place + left_place_ < bucket_size_) {
            const T &ans = data_[left_bucket_][left_place_ + place];
            return ans;
        }
        size_t current_bucket = left_bucket_;
        place -= (bucket_size_ - left_place_);
        ++current_bucket;
        current_bucket += (place / bucket_size_);
        place %= bucket_size_;
        return data_[current_bucket][place];
    }

    T &at(size_t place) {
        if (place >= size()) {
            throw std::out_of_range("out of range in deque");
        }
        return (*this)[place];
    }

    const T &at(size_t place) const {
        if (place >= size()) {
            throw std::out_of_range("out of range in deque");
        }
        return (*this)[place];
    }

    void push_back(const T &val) {
        try {
            data_[right_bucket_].push_back(val);
        } catch (...) {
            throw;
        }
        ++right_place_;
        if (right_place_ == bucket_size_) {
            right_place_ = 0;
            ++right_bucket_;
            if (right_bucket_ >= data_.size()) {
                data_.emplace_back(bucket_size_, alloc);
                data_[data_.size() - 1].place(0, 0, val);
            }
        }
    }

    void pop_back() {
        try {
            data_[right_bucket_].pop_back();
        } catch (...) {
            throw;
        }
        --right_place_;
        if (right_place_ == 0) {
            right_place_ = bucket_size_;
            --right_bucket_;
        }
    }

    void push_front(const T &val) {
        if (left_place_ == 0) {
            if (left_bucket_ == 0) {
                std::vector<Bucket> data_new(data_.size() * 2, FictiveBucketInit(bucket_size_, alloc));
                for (size_t i = 0; i < data_.size(); ++i) {
                    data_new[data_.size() + i] = std::move(data_[i]);
                }
                for (size_t i = 0; i < data_.size() - 1; ++i) {
                    data_new[i] = Bucket(bucket_size_, alloc);
                }
                left_bucket_ += data_.size();
                right_bucket_ += data_.size();
                --left_bucket_;
                left_place_ = bucket_size_ - 1;
                size_t prev_size = data_.size();
                data_ = std::move(data_new);
                try {
                    data_[prev_size - 1].place(bucket_size_ - 1, bucket_size_, val);
                } catch (...) {
                    ++left_bucket_;
                    left_place_ = 0;
                    throw;
                }

            } else {
                try {
                    data_[left_bucket_ - 1].push_front(val);
                    --left_bucket_;
                    left_place_ = bucket_size_ - 1;
                } catch (...) {
                    throw;
                }
            }
        } else {
            try {
                data_[left_bucket_].push_front(val);
                --left_place_;
            } catch (...) {
                throw;
            }
        }
    }

    void pop_front() {
        try {
            data_[left_bucket_].pop_front();
        } catch (...) {
            throw;
        }
        ++left_place_;
        if (left_place_ == bucket_size_) {
            left_place_ = 0;
            ++left_bucket_;
        }
    }

    iterator begin() {
        Bucket &left_bucket = data_[left_bucket_];
        iterator ans(*this, left_bucket_, left_bucket.begin(), left_bucket.end(), left_bucket.begin());
        return ans;
    }

    iterator end() {
        Bucket &right_bucket = data_[right_bucket_];
        iterator ans(*this, right_bucket_, right_bucket.begin(), right_bucket.end(), right_bucket.end());
        return ans;
    }

    const_iterator begin() const {
        const Bucket &left_bucket = data_[left_bucket_];
        const_iterator ans(*this, left_bucket_, left_bucket.begin(), left_bucket.end(), left_bucket.begin());
        return ans;
    }

    const_iterator end() const {
        const Bucket &right_bucket = data_[right_bucket_];
        const_iterator ans(*this, right_bucket_, right_bucket.begin(), right_bucket.end(), right_bucket.end());
        return ans;
    }

    reverse_iterator rbegin() {
        Bucket &right_bucket = data_[right_bucket_];
        iterator pre_ans(*this, right_bucket_, right_bucket.begin(), right_bucket.end(), right_bucket.end());
        --pre_ans;
        reverse_iterator ans(*this, pre_ans.place_of_bucket_, pre_ans.left_, pre_ans.right_, pre_ans.pointer_);
        return ans;
    }

    reverse_iterator rend() {
        Bucket &left_bucket = data_[left_bucket_];
        iterator pre_ans(*this, left_bucket_, left_bucket.begin(), left_bucket.end(), left_bucket.begin());
        --pre_ans;
        reverse_iterator ans(*this, pre_ans.place_of_bucket_, pre_ans.left_, pre_ans.right_, pre_ans.pointer_);
        return ans;
    }

    const_iterator cbegin() const {
        const Bucket &left_bucket = data_[left_bucket_];
        const_iterator ans(*this, left_bucket_, left_bucket.begin(), left_bucket.end(), left_bucket.begin());
        return ans;
    }

    const_iterator cend() const {
        const Bucket &right_bucket = data_[right_bucket_];
        const_iterator ans(*this, right_bucket_, right_bucket.begin(), right_bucket.end(), right_bucket.end());
        return ans;
    }

    const_reverse_iterator crbegin() const {
        const Bucket &right_bucket = data_[right_bucket_];
        const_iterator pre_ans(*this, right_bucket_, right_bucket.begin(), right_bucket.end(), right_bucket.end());
        --pre_ans;
        const_reverse_iterator ans(*this, pre_ans.place_of_bucket_, pre_ans.left_, pre_ans.right_, pre_ans.pointer_);
        return ans;
    }

    const_reverse_iterator crend() const {
        const Bucket &left_bucket = data_[left_bucket_];
        const_iterator pre_ans(*this, left_bucket_, left_bucket.begin(), left_bucket.end(), left_bucket.begin());
        --pre_ans;
        const_reverse_iterator ans(*this, pre_ans.place_of_bucket_, pre_ans.left_, pre_ans.right_, pre_ans.pointer_);
        return ans;
    }

    void insert(const iterator &place, const T &val) {
        push_back(val);
        std::vector<Bucket> buckets_new(data_.size() - place.place_of_bucket_, BucketInit(bucket_size_, alloc));
        size_t delta = place.place_of_bucket_;
        {
            int start = data_[delta].begin() - data_[delta].leftWall();
            int changing_place = place.pointer() - data_[delta].leftWall();
            int finish = data_[delta].end() - data_[delta].leftWall();
            for (int j = start; j < changing_place; ++j) {
                alloc.construct(buckets_new[0].pointer(j), std::move(data_[delta][j]));
            }
            alloc.construct(buckets_new[0].pointer(changing_place),
                            (right_place_ > 0) ? data_[right_bucket_][right_place_ - 1] : data_[right_bucket_ - 1][
                                    bucket_size_ - 1]);
            for (int j = changing_place; j < finish - 1; ++j) {
                alloc.construct(buckets_new[0].pointer(j + 1), std::move(data_[delta][j]));
            }
            buckets_new[0].sameSizes(data_[0 + delta]);
        }
        for (size_t i = 1; i < data_.size() - delta; ++i) {
            int start = data_[i + delta].begin() - data_[i + delta].leftWall();
            int finish = data_[i + delta].end() - data_[i + delta].leftWall();
            for (int j = start; j < (finish - 1); ++j) {
                alloc.construct(buckets_new[i].pointer(j + 1), std::move(data_[delta + i][j]));
            }
            buckets_new[i].sameSizes(data_[i + delta]);
        }
        int i = 1;

        while (i + delta < data_.size() && data_[delta + i].end() != data_[delta + i].begin()) {
            alloc.construct(buckets_new[i].pointer(0), std::move(data_[delta + i - 1][bucket_size_ - 1]));
            ++i;
        }


        int limit = data_.size() - delta;
        for (int j = 0; j < limit; ++j) {
            data_[j + delta] = std::move(buckets_new[j]);
        }
    }

    void erase(const iterator &place) {
        std::vector<Bucket> buckets_new(data_.size() - place.place_of_bucket_, BucketInit(bucket_size_, alloc));
        size_t delta = place.place_of_bucket_;
        int changing_place = place.pointer() - data_[delta].leftWall();
        {
            int start = data_[delta].begin() - data_[delta].leftWall();
            int finish = data_[delta].end() - data_[delta].leftWall();
            for (int j = start; j < changing_place; ++j) {
                alloc.construct(buckets_new[0].pointer(j), std::move(data_[delta][j]));
            }
            for (int j = (changing_place + 1); j < finish; ++j) {
                alloc.construct(buckets_new[0].pointer(j - 1), std::move(data_[delta][j]));
            }
            buckets_new[0].sameSizes(data_[0 + delta]);
        }
        for (size_t i = 1; i < data_.size() - delta; ++i) {
            int start = data_[i + delta].begin() - data_[i + delta].leftWall();
            int finish = data_[i + delta].end() - data_[i + delta].leftWall();
            for (int j = start + 1; j < finish; ++j) {
                alloc.construct(buckets_new[i].pointer(j - 1), std::move(data_[delta + i][j]));
            }
            buckets_new[i].sameSizes(data_[i + delta]);
        }
        int i = 1;

        while (i + delta < data_.size() && data_[delta + i].end() != data_[delta + i].begin()) {
            alloc.construct(buckets_new[i - 1].pointer(bucket_size_ - 1), std::move(data_[delta + i][0]));
            ++i;
        }

        alloc.construct((right_place_ > 0) ? buckets_new[right_bucket_ - delta].pointer(right_place_ - 1) : buckets_new[
                                right_bucket_ - delta - 1].pointer(bucket_size_ - 1),
                        std::move(data_[place.place_of_bucket_][changing_place]));

        try {
            int limit = data_.size() - delta;
            for (int j = 0; j < limit; ++j) {
                data_[j + delta].swap(buckets_new[j]);
            }
            pop_back();
        } catch (...) {
            int limit = data_.size() - delta;
            for (int j = 0; j < limit; ++j) {
                data_[j + delta].swap(buckets_new[j]);
            }
        }
    }

    size_t size() const {
        return (((right_bucket_ - left_bucket_) * bucket_size_ + right_place_) - left_place_);
    }

    void help_print() {
        std::cout << left_bucket_ << "\t" << left_place_ << "\n";
        std::cout << right_bucket_ << "\t" << right_place_ << "\n";
    }


    friend class Deque::iterator;
};
