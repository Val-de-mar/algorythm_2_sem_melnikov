#include <iostream>
#include <vector>


template<unsigned long long N>
class Finite {
    long long value = 0;
    long long finite(long long val) const {
        val %= (long long)(N);
        return val >= 0 ? val : val + N;
    }
    Finite reverse() const {
        long long ans = 1;
        long long pow_val = value;
        unsigned long long use = N - 2;
        while(use > 0) {
            bool add = (use % 2);
            if (add) {
                ans *= pow_val;
            }
            pow_val *= pow_val;
            ans %= N;
            pow_val %= N;
            use /= 2;
        }
        return Finite<N>(ans);
    }

public:
    Finite() {}
    Finite(long long val) : value(finite(val)) {}
    Finite(const Finite<N>& other) = default;


    Finite<N>& operator = (const Finite<N>& other) {
        value = finite(other.value);
        return *this;
    }
    Finite<N>& operator += (const Finite<N>& other) {
        value += other.value;
        value = finite(value);
        return *this;
    }
    Finite<N>& operator -= (const Finite<N>& other) {
        value -= other.value;
        value = finite(value);
        return *this;
    }
    Finite<N>& operator *= (const Finite<N>& other) {
        long long val = value;
        val *= other.value;
        value = val % N;
        return *this;
    }
    Finite<N>& operator /= (const Finite<N>& other) {
        *this *= other.reverse();
        return *this;
    }
    Finite<N>& operator ++() {
        ++value;
        value = ((value == N) ? 0 : value);
        return *this;
    }
    const Finite<N> operator ++(int) {
        auto ans = *this;
        ++value;
        value = ((value == N) ? 0 : value);
        return ans;
    }
    Finite<N>& operator --() {
        --value;
        value = ((value == -1) ? N - 1 : value);
        return *this;
    }
    const Finite<N> operator --(int) {
        auto ans = *this;
        --value;
        value = ((value == -1) ? N - 1 : value);
        return ans;
    }


    Finite<N> operator + (const Finite<N>& other) const {
        auto ans = *this;
        return ans += other;
    }
    Finite<N> operator - (const Finite<N>& other) const {
        auto ans = *this;
        return ans -= other;
    }
    Finite<N> operator * (const Finite<N>& other) const {
        auto ans = *this;
        return ans *= other;
    }
    Finite<N> operator / (const Finite<N>& other) const {
        auto ans = *this;
        ans /= other;
        return ans;
    }

    Finite<N> operator - () {
        return (Finite<N>(0) - *this);
    }

    bool operator == (const Finite<N>& other) const {
        return value == other.value;
    }
    bool operator != (const Finite<N>& other) const {
        return value != other.value;
    }

    explicit operator int() {
        return value;
    }

    [[nodiscard]] long long val() const {
        return value;
    }
};

template <unsigned long long N>
std::ostream& operator << (std::ostream& output, const Finite<N>& finite) {
    output << finite.val();
    return output;
}


typedef Finite<999999937> fin;



template<typename T>
class SmallMatrix {
    size_t n, m;
    std::vector<std::vector<T>> data;
public:
    SmallMatrix(const SmallMatrix& another) : n(another.n), m(another.m), data(another.data) {}

    SmallMatrix(size_t n, size_t m, T default_val) : n(n), m(m), data(n, std::vector<T>(m, default_val)) {}

    SmallMatrix(size_t n, size_t m) : n(n), m(m), data(n, std::vector<T>(m)) {}

    explicit SmallMatrix(const std::vector<std::vector<T>>& value) : n(value.size()), m(n > 0 ? value[0].size() : 0), data(value) {}

    SmallMatrix<T> &operator*=(const SmallMatrix<T> &another);

    std::vector<T> &operator[](size_t place);

    size_t size_x() {return n;}
    size_t size_y() {return m;}
};

template<typename T>
SmallMatrix<T> &SmallMatrix<T>::operator*=(const SmallMatrix<T> &another) {
    if (m != another.n) throw std::invalid_argument("the matrix of this size cannot be multiplied");
    SmallMatrix<T> result(n, m, T(0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < another.m; ++j) {
            for (int k = 0; k < m; ++k) {
                result[i][j] += data[i][k] * another.data[k][j];
            }
        }
    }
    std::swap(data, result.data);
    return *this;
}

template<typename T>
std::vector<T> &SmallMatrix<T>::operator[](size_t place) {
    return data[place];
}


template<typename T>
T power(const T& value, unsigned long long power) {
    --power;
    T ans(value), in_pow_2(value);
    while (power > 0) {
        if (bool(power & 1u)) {
            ans *= in_pow_2;
        }
        in_pow_2 *= in_pow_2;
        power = (power >> 1);
    }
    return ans;
}


int main() {

    fin one = fin(1), null = fin(0);
    std::vector<std::vector<fin>> dp = {std::vector<fin>({one,one,one,one,one}), std::vector<fin>({one,one,one,one,one}), std::vector<fin>({one,one,one,null,null}), std::vector<fin>({one,one,one,one,one}), std::vector<fin>({one,one,one,null,null})};

    SmallMatrix<fin> mtr1(dp);
    while(true){
        long long n;
        std::cin >> n;
        if (n == 0) break;
        if (n == 1) std::cout << 5 << '\n';
        else {
            --n;
            SmallMatrix ans(power(mtr1, n));
            fin answer = 0;
            for (int i = 0; i < ans.size_x(); ++i) {
                for (int j = 0; j < ans.size_y(); ++j) {
                    answer += ans[i][j];
                }
            }
            std::cout << answer << '\n';
        }
    }

    return 0;
}
