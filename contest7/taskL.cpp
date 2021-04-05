#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

typedef unsigned int uint;




class BigInteger {
    const static unsigned int base_decimal_size = 4;
    const static unsigned int base = 10000;

    void clearNulls () {
        for (size_t i = data.size() - 1; i > 0 && data[i] == 0; --i) {
            data.pop_back();
        }
    }

    void resign () {
        clearNulls();
        if (data[data.size() - 1] < 0) {
            positive = !positive;
            for (size_t i = 0; i < data.size(); ++i) {
                data[i] = -data[i];
            }
        }
        for (size_t i = 0; i < data.size() - 1;) {
            if (data[i] < 0) {
                --(data[i + 1]);
                data[i] += base;
            } else {
                ++i;
            }
        }
        clearNulls();
        if (data.size() <= 1 && data[0] == 0){
            positive = true;
        }
        if (data.size() == 1 && data[0] == 0) {
            positive = true;
        }
    }

    bool is_abs_more(const BigInteger & val) {
        if(data.size() > val.data.size()) {
            return true;
        } else if(data.size() < val.data.size()) {
            return false;
        }
        for(size_t i = data.size() - 1; i > 0; --i) {
            if(data[i] > val.data[i]) {
                return true;
            } else if(data[i] < val.data[i]) {
                return false;
            }
        }
        return data[0] > val.data[0];
    }

    void plus(long long val, size_t place) {
        BigInteger value(val);
        long long temporary = 0;
        size_t prev_size = data.size();
        data.resize(std::max(data.size(), value.data.size() + place));
        for (size_t i = prev_size; i < data.size(); ++i) {
            data[i] = 0;
        }

        for (size_t k = 0; k < value.data.size(); ++k) {
            temporary += data[k + place];
            temporary += value.data[k];
            data[k + place] = temporary % base;
            temporary /= base;
        }
        if (temporary != 0) {
            if (data.size() <= value.data.size() + place) {
                data.push_back(temporary);
            } else {
                data[value.data.size() + place] += temporary;
            }
        }

        for (int i : data) {
            if (i < 0) {
                resign();
                break;
            }
        }
    }

    void push(size_t val) {
        data.resize(data.size() + val);
        auto end = data.rend() - val;
        for (auto i = data.rbegin(); i < end; ++i) {
            *i = *(i + val);
        }
        for (auto i = end; i != data.rend() ; ++i) {
            *i = 0;
        }
    }

    void pull(size_t val) {
        auto max = data.end() - val;
        for (auto i = data.begin(); i < max; ++i) {
            *i = *(i + val);
        }
        data.resize(data.size() - val);
    }
    mutable bool positive = true;
    std::vector<short int> data;
public:

    BigInteger() : positive(true), data(1,0){}
    BigInteger(const std::string & str){
        unsigned int multiplier = 1, min = 0;
        if (str[0] == '-') {
            positive = false;
            min = 1;
        }
        int place = 0;
        for (size_t i = (str.size()); i > min; --i) {
            if (multiplier == 1) {
                data.push_back(0);
            }
            data[place] += multiplier * static_cast<unsigned int>(str[i - 1] -'0');
            multiplier *= 10;
            if (multiplier >= base) {
                multiplier = 1;
                ++place;
            }
        }
    }

    BigInteger(const BigInteger& other) : data(other.data){
        positive = other.positive;
    }
    BigInteger(BigInteger&& other) :  positive(other.positive), data(std::move(other.data)){}
    explicit BigInteger(long long val) : positive(val >= 0) {
        val = std::abs(val);
        if (val == 0){
            data.push_back(0);
        } else {
            val = std::abs(val);
            while (val > 0) {
                data.push_back(val % base);
                val /= base;
            }
        }
    }
    explicit BigInteger(int val) : positive(val >= 0){
        if (val == 0) {
            data.push_back(0);
        } else {
            val = std::abs(val);
            while (val > 0) {
                data.push_back(val % base);
                val /= base;
            }
        }
    }
    BigInteger(unsigned long long val) : positive(true){
        if (val == 0) {
            data.push_back(0);
        } else {
            while (val > 0) {
                data.push_back(val % base);
                val /= base;
            }
        }
    }

    BigInteger& operator = (const BigInteger& other) = default;
    BigInteger& operator = (BigInteger&& other) {
        data = std::move(other.data);
        positive = other.positive;
        return *this;
    }
    BigInteger& operator = (std::string str) {
        positive = true;
        data.clear();
        unsigned int multiplier = 1, min = 0;
        if (str[0] == '-') {
            positive = false;
            min = 1;
        }
        int place = 0;
        for (size_t i = (str.size()); i > min; --i) {
            if (multiplier == 1) {
                data.push_back(0);
            }
            data[place] += multiplier * static_cast<unsigned int>(str[i - 1] -'0');
            multiplier *= 10;
            if (multiplier >= base) {
                multiplier = 1;
                ++place;
            }
        }
        return *this;
    }
    BigInteger& operator = (int val) {
        data.clear();
        positive = val >= 0;
        val = std::abs(val);
        if (val == 0){
            data.push_back(0);
        } else {
            val = std::abs(val);
            while (val > 0) {
                data.push_back(val % base);
                val /= base;
            }
        }
        return *this;
    }
    BigInteger& operator = (long long val) {
        data.clear();
        positive = val >= 0;
        val = std::abs(val);
        if (val == 0){
            data.push_back(0);
        } else {
            val = std::abs(val);
            while (val > 0) {
                data.push_back(val % base);
                val /= base;
            }
        }
        return *this;
    }

    BigInteger& operator ++ () {
        *this += 1;
        return *this;
    }
    BigInteger operator ++(int) {
        BigInteger temp = *this;
        *this += 1;
        return temp;
    }

    BigInteger& operator -- () {
        *this -= 1;
        return *this;
    }
    BigInteger operator --(int) {
        BigInteger temp = *this;
        *this -= 1;
        return temp;
    }



    BigInteger& operator += (const BigInteger & other) {
        long long temporary = 0;
        size_t prev_size = data.size();
        data.resize(std::max(data.size(), other.data.size()));
        for (size_t i = prev_size; i < data.size(); ++i) {
            data[i] = 0;
        }

        for (size_t k = 0; k < other.data.size(); ++k) {
            temporary += data[k];
            temporary += (other.data[k] * (positive == other.positive ? 1 : -1));
            data[k] = temporary % base;
            temporary /= base;
        }
        if (temporary != 0) {
            if (data.size() == other.data.size()) {
                data.push_back(temporary);
            } else {
                for (uint i = other.data.size(); i < data.size() && temporary != 0; ++i) {
                    data[i] += temporary;
                    temporary = data[i] / base;
                    data[i] %= base;
                }

            }
        }

        resign();

        return *this;
    }
    BigInteger operator + (const BigInteger& other) const {
        BigInteger t = *this;
        t += other;
        return t;
    }

    BigInteger& operator -= (const BigInteger& quotient) {
        quotient.positive = !quotient.positive;
        *this += quotient;
        quotient.positive = !quotient.positive;
        resign();
        return *this;
    }
    BigInteger operator - (const BigInteger& other) const{
        BigInteger t = *this;
        t -= other;
        return t;
    }

    BigInteger& operator *= (const BigInteger& value) {
        BigInteger temporary(0);
        for (size_t j = 0; j < data.size(); ++j) {
            for (size_t i = 0; i < value.data.size(); ++i) {
                long long a = value.data[i] * 1LL * data[j];
                temporary.plus(a , i + j);
            }
        }
        data = std::move(temporary.data);
        positive = (positive == value.positive);
        if (data.size() == 1 && data[0] == 0) {
            positive = true;
        }
        return *this;
    }
    BigInteger operator * (const BigInteger& value) const{
        BigInteger temporary(0);
        for (size_t j = 0; j < data.size(); ++j) {
            for (size_t i = 0; i < value.data.size(); ++i) {
                long long a = value.data[i] * 1LL * data[j];
                temporary.plus(a , i + j);
            }
        }
        temporary.positive = (positive == value.positive);
        if (data.size() == 1 && data[0] == 0) {
            positive = true;
        }
        return temporary;
    }

    BigInteger& operator /= (const  BigInteger& value) {
        bool this_pos = positive, val_pos = value.positive;
        positive = true;
        value.positive = true;
        if(value > *this) {
            value.positive = val_pos;
            *this = 0;
            return *this;
        } else {
            BigInteger result;
            result.data.resize(2 + data.size() - value.data.size());
            for (short &i : result.data) {
                i = 0;
            }

            if (value.data.size() > 1) {
                auto helper = *this;
                long long divisor = (*value.data.rbegin()) * base + *(++value.data.rbegin());
                long long quotient;
                long long temp = 0;
                for (auto i = static_cast<long long>(data.size()) - 1;i >= static_cast<long long>(value.data.size()) - 2; -- i) {
                    for (long long j = static_cast<long long>(data.size()) - 1; j >= i ; --j) {
                        temp *= base;
                        temp += data[j];
                    }
                    if (temp >= divisor) {
                        quotient = temp / divisor;
                        BigInteger test = value * quotient;
                        size_t delta = i + 2 - value.data.size();
                        test.data.resize(test.data.size() + delta);
                        for (size_t j = test.data.size(); j > delta; --j) {
                            test.data[j - 1] = test.data[j - 1 - delta];
                        }
                        for (size_t j = 0; j < delta; ++j) {
                            test.data[j] = 0;
                        }
                        if(test > *this) {
                            for (size_t j = delta; j < test.data.size(); ++j) {
                                test.data[j - delta] = test.data[j];
                            }
                            test.clearNulls();
                            test -= value;
                            for (size_t j = test.data.size(); j > delta; --j) {
                                test.data[j - 1] = test.data[j - 1 - delta];
                            }
                            for (size_t j = 0; j < delta; ++j) {
                                test.data[j] = 0;
                            }
                            --quotient;
                        }
                        *this -= test;
                        clearNulls();
                        result.data[i + 2 - value.data.size()] += quotient;
                    }
                    temp = 0;
                }
            } else if (value.data[0] != 0) {
                long long temp = 0;
                long long divisor = value.data[0];
                for (auto i = static_cast<long long>(data.size()) - 1; i >= 0; --i) {
                    temp *= base;
                    temp += data[i];
                    result.data[i] = temp / divisor;
                    temp %= divisor;
                }
            }
            result.clearNulls();
            data = std::move(result.data);
            positive = (this_pos == val_pos);
            value.positive = val_pos;
            resign();
            return *this;
        }
    }
    BigInteger operator / (const BigInteger & value) const {
        BigInteger t = *this;
        t /= value;
        return t;
    }

    BigInteger& operator %= (const BigInteger& value) {
        bool this_pos = positive, val_pos = value.positive;
        positive = true;
        value.positive = true;
        if(value > *this) {
            value.positive = val_pos;
            positive = this_pos;
            return *this;
        } else {
            if (value.data.size() > 1) {
                long long divisor = (*value.data.rbegin()) * base + *(++value.data.rbegin());
                long long temp = 0;
                for (auto i = static_cast<long long>(data.size()) - 1;i >= static_cast<long long>(value.data.size()) - 2; -- i) {
                    for (long long j = static_cast<long long>(data.size()) - 1; j >= i ; --j) {
                        temp *= base;
                        temp += data[j];
                    }
                    if (temp >= divisor) {
                        BigInteger test = value * (temp / divisor);
                        size_t delta = i + 2 - value.data.size();
                        test.data.resize(test.data.size() + delta);
                        for (size_t j = test.data.size(); j > delta; --j) {
                            test.data[j - 1] = test.data[j - 1 - delta];
                        }
                        for (size_t j = 0; j < delta; ++j) {
                            test.data[j] = 0;
                        }
                        if(test > *this) {
                            for (size_t j = delta; j < test.data.size(); ++j) {
                                test.data[j - delta] = test.data[j];
                            }
                            test.clearNulls();
                            test -= value;
                            for (size_t j = test.data.size(); j > delta; --j) {
                                test.data[j - 1] = test.data[j - 1 - delta];
                            }
                            for (size_t j = 0; j < delta; ++j) {
                                test.data[j] = 0;
                            }
                        }
                        *this -= test;
                        clearNulls();
                    }
                    temp = 0;
                }
            } else if (value.data[0] != 0) {
                long long temp = 0;
                long long divisor = value.data[0];
                for (auto i = static_cast<long long>(data.size()) - 1; i >= 0; --i) {
                    temp *= base;
                    temp += data[i];
                    temp %= divisor;
                }
                *this = temp;
            }
            positive = (this_pos == val_pos);
            value.positive = val_pos;
            resign();
            return *this;
        }
    }
    BigInteger operator % (const BigInteger& value) const{
        BigInteger t;
        t = *this;
        t %= value;
        return t;
    }

    BigInteger operator -() const{
        BigInteger t = *this;
        if( t != 0) {
            t.positive = !positive;
            return t;
        } else {
            return t;
        }
    }


    explicit operator bool() {
        return (*this) != 0;
    }

    bool operator >= (const BigInteger & val) const {
        if (positive) {
            if(val.positive) {
                if (data.size() > val.data.size()) {
                    return true;
                } else if (data.size() < val.data.size()) {
                    return false;
                }
                for (size_t i = data.size() - 1; i > 0; --i) {
                    if (data[i] > val.data[i]) {
                        return true;
                    } else if (data[i] < val.data[i]) {
                        return false;
                    }
                }
                return data[0] >= val.data[0];
            } else {
                return true;
            }
        } else {
            if(val.positive) {
                return false;
            } else {
                bool tp = positive, vp = val.positive;
                positive = true;
                val.positive = true;
                bool ans = (val >= *this);
                val.positive = vp;
                positive = tp;
                return ans;
            }
        }
    }
    bool operator > (const BigInteger & val) const {
        if (positive) {
            if(val.positive) {
                if (data.size() > val.data.size()) {
                    return true;
                } else if (data.size() < val.data.size()) {
                    return false;
                }
                for (size_t i = data.size() - 1; i > 0; --i) {
                    if (data[i] > val.data[i]) {
                        return true;
                    } else if (data[i] < val.data[i]) {
                        return false;
                    }
                }
                return data[0] > val.data[0];
            } else {
                return true;
            }
        } else {
            if(val.positive) {
                return false;
            } else {
                bool tp = positive, vp = val.positive;
                positive = true;
                val.positive = true;
                bool ans = (val > *this);
                val.positive = vp;
                positive = tp;
                return ans;
            }
        }
    }
    bool operator <= (const BigInteger & val) const {
        return val >= *this;
    }
    bool operator < (const BigInteger & val) const {
        return val > *this;
    }
    bool operator == (const BigInteger & val) const {
        if(data.size() != val.data.size() || positive != val.positive) {
            return false;
        }
        for (size_t i = 0; i < data.size(); ++i) {
            if(data[i] != val.data[i]) {
                return false;
            }
        }
        return true;
    }
    bool operator != (const BigInteger & val) const {
        if (data.size() != val.data.size() || positive != val.positive) {
            return true;
        }
        for (size_t i = 0; i < data.size(); ++i) {
            if(data[i] != val.data[i]) {
                return true;
            }
        }
        return false;
    }
    bool operator >= (int val) const {
        BigInteger temp = val;
        return *this >= temp;
    }
    bool operator > (int val) const {
        BigInteger temp = val;
        return *this > temp;
    }
    bool operator <= (int val) const {
        BigInteger temp = val;
        return *this <= temp;
    }
    bool operator < (int val) const {
        BigInteger temp = val;
        return *this < temp;
    }
    bool operator == (int val) const {
        BigInteger t(val);
        return t == *this;
    }

    std::string toString() const {
        std::string ans;
        if (*this != 0) {
            if (!positive) {
                ans.push_back('-');
            }
            long long scaner = data[data.size() - 1];
            long long multipl = base / 10;
            while (scaner / multipl == 0) {
                multipl /= 10;
            }
            for (auto i = data.rbegin(); i != data.rend(); ++i) {
                scaner = *i;
                while (multipl > 0) {
                    ans.push_back(scaner / multipl + '0');
                    scaner %= multipl;
                    multipl /= 10;
                }
                multipl = base / 10;
            }
        } else {
            ans = '0';
        }
        return ans;
    }

    size_t len() const {
        size_t ans = base_decimal_size*data.size();
        for (int j = base / 10; j > 0 && data[data.size() - 1] / j == 0; j/=10) {
            -- ans;
        }
        return ans;
    }

    short int remains2(){
        short int remains = 0;
        for (uint i = data.size(); i > 0; --i) {
            data[i - 1] += remains*base;
            remains = data[i - 1];
            remains %= 2;
            data[i - 1] /= 2;
        }
        clearNulls();
        return remains;
    }
    BigInteger& devide2(){
        short int remains = 0;
        for (uint i = data.size(); i > 0; --i) {
            data[i - 1] += remains*base;
            remains = data[i - 1];
            remains %= 2;
            data[i - 1] /= 2;
        }
        clearNulls();
        return *this;
    }
    const BigInteger& prog2() {
        short int remains = 0;
        for (uint i = 0; i < data.size(); ++i) {
            data[i] *= 2;
            data[i] += remains;
            remains = data[i];
            data[i] %= base;
            remains /= base;
        }
        if (remains != 0) {
            data.push_back(remains);
        }
        return *this;
    }
    bool odd() const {
        short int remains = data[0];
        remains %= 2;
        return (remains == 1);
    }
    bool isNull() const {
        return (data.size() == 1) && (data[0] == 0);
    }
    bool isOne() const {
        return (data.size() == 1) && (data[0] == 1) && positive;
    }
    bool last_bit() {
        return data[0] & 1u;
    }

    friend std::ostream& operator << (std::ostream& output, const BigInteger& value);
    friend class Rational;
    friend BigInteger NOD(BigInteger first, BigInteger second);
    friend int main();
};

std::ostream& operator << (std::ostream& output, const BigInteger& value) {
    if (! value.positive) {
        output << '-';
    }
    output << value.data[value.data.size() - 1];
    for (size_t i = value.data.size() - 1; i > 0; --i) {
        int place_print = BigInteger::base / 10, current = value.data[i - 1];
        for (size_t j = 0; j < BigInteger::base_decimal_size; ++j) {
            output << current / place_print;
            current %= place_print;
            place_print /= 10;
        }
    }
    return output;
}
std::istream& operator >> (std::istream& input, BigInteger& value) {
    std::string scan;
    input >> scan;
    value = BigInteger(scan);
    return input;
}

bool operator >= (int first, const BigInteger& second){
    return second <= first;
}
bool operator > (int first, const BigInteger& second){
    return second < first;
}
bool operator <= (int first, const BigInteger& second){
    return second >= first;
}
bool operator < (int first, const BigInteger& second){
    return second > first;
}

BigInteger operator + (int first, const BigInteger & second){
    BigInteger t = first;
    t += second;
    return t;
}
BigInteger operator - (int first, const BigInteger & second){
    BigInteger t = first;
    t -= second;
    return t;
}

BigInteger NOD(BigInteger first, BigInteger second) {
    first.positive = true;
    second.positive = true;

    BigInteger ans = 1;
    while(true) {
        bool odd1 = first.odd();
        bool odd2 = second.odd();
        if (first.isNull()) {
            return (second *= ans);
        } else if (second.isNull()){
            return (first *= ans);
        } else if (first.isOne() || second.isOne()){
            return ans;
        } else {
            if (odd1) {
                if (odd2) {
                    if (first > second) {
                        first -= second;
                    } else {
                        second -= first;
                    }
                } else {
                    second.devide2();
                }
            } else {
                if (odd2) {
                    first.devide2();
                } else {
                    first.devide2();
                    second.devide2();
                    ans.prog2();
                }
            }
        }
    }
}


class Finite {
    unsigned long long N;
    unsigned long long value = 0;

public:
    Finite(unsigned long long N) : N(N) {}
    Finite(unsigned long long N, unsigned long long val) : N(N), value(val) {}
    Finite(const Finite& other) = default;


    Finite& operator = (const Finite& other) = default;
    Finite& operator += (const Finite& other) {
        if(N == 0) {
            N = other.N;
        }
        value += other.value;
        value %= N;
        return *this;
    }
    Finite& operator -= (const Finite& other) {
        if(N == 0) {
            N = other.N;
        }
        value += N;
        value -= other.value;
        value %= N;
        return *this;
    }
    Finite& operator *= (const Finite& other) {
        if(N == 0) {
            N = other.N;
        }
        long long val = value;
        val *= other.value;
        value = val % N;
        return *this;
    }
    Finite& operator ++() {
        ++value;
        value = ((value == N) ? 0 : value);
        return *this;
    }
    const Finite operator ++(int) {
        auto ans = *this;
        ++value;
        value = ((value == N) ? 0 : value);
        return ans;
    }
    Finite& operator --() {
        --value;
        value = ((value == -1) ? N - 1 : value);
        return *this;
    }
    const Finite operator --(int) {
        auto ans = *this;
        --value;
        value = ((value == -1) ? N - 1 : value);
        return ans;
    }


    Finite operator + (const Finite& other) const {
        auto ans = *this;
        return ans += other;
    }
    Finite operator - (const Finite& other) const {
        auto ans = *this;
        return ans -= other;
    }
    Finite operator * (const Finite& other) const {
        auto ans = *this;
        ans *= other;
        return ans;
    }

    Finite operator - () {
        return (Finite(0) - *this);
    }

    bool operator == (const Finite& other) const {
        return value == other.value;
    }
    bool operator != (const Finite& other) const {
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
std::ostream& operator << (std::ostream& output, const Finite& finite) {
    output << finite.val();
    return output;
}




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
    SmallMatrix<T> result(n, m, T( 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < another.m; ++j) {
            result[i][j] = data[i][0] * another.data[0][j];
            for (int k = 1; k < m; ++k) {
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
T power(const T& value, BigInteger power) {
    --power;
    T ans(value), in_pow_2(value);
    while (power > 0) {
        if (power.last_bit()) {
            ans *= in_pow_2;
        }
        in_pow_2 *= in_pow_2;
        power.devide2();
    }
    return ans;
}


bool check(unsigned int a, unsigned int b, size_t size) {
    unsigned int mask  = 1;
    for (int i = 0; i < size - 1; ++i) {
        if (bool(mask & a) == bool((mask << 1) & a)) {
            if (bool(mask & b) == bool((mask << 1) & b) && bool(mask & a) == bool(mask & b)) {
                return false;
            }
        }
        mask = (mask << 1);
    }
    return true;
}


int main() {
    unsigned long long m, mod;
    BigInteger n;
    std::cin >> n >> m >> mod;
    if(n == 1) {
        std::cout << (1u << m);
        return 0;
    }
    std::vector<std::vector<Finite>> dp((1u << m), std::vector<Finite>((1u << m), mod));
    for (int i = 0; i < dp.size(); ++i) {
        for (int j = 0; j < dp[0].size(); ++j) {
            if(check(i,j,m)) {
                dp[i][j] = Finite(mod, 1);
            } else {
                dp[i][j] = Finite(mod, 0);
            }
        }
    }
    SmallMatrix ans(dp);
    Finite answer(mod, 0);
    --n;
    ans = power(ans, n);
    for (int i = 0; i < ans.size_x(); ++i) {
        for (int j = 0; j < ans.size_y(); ++j) {
            answer += ans[i][j];
        }
    }
    std::cout << int(answer);
    return 0;
}
