#include <iostream>

unsigned long long powOfValMod(unsigned long long val,unsigned long long power, unsigned long long mod) {
    unsigned long long ans = 1;
    unsigned long long current_power = val % mod;
    while (power > 0) {
        if (power & 1ull) {
            ans *= current_power;
            ans %= mod;
        }
        current_power = current_power * current_power;
        current_power %= mod;
        power = (power >> 1);
    }
    return ans;
}
void optimise() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
}

int main() {
    const static unsigned long long simple_module = 1000000007;

    optimise();
    unsigned int k;
    unsigned long long a, denominator = 2;
    bool minus_one_pow_is_even = false;
    std::cin >> k;
    for (int i = 0; i < k; ++i) {
        std::cin >> a;
        denominator = powOfValMod(denominator, a, simple_module);
        if (!(a & 1ull)) {
            minus_one_pow_is_even = true;
        }
    }
    unsigned long long opposite_2, opposite_3;
    opposite_2 = powOfValMod(2, simple_module - 2, simple_module);
    opposite_3 = powOfValMod(3, simple_module - 2, simple_module);
    denominator = (denominator * opposite_2) % simple_module;
    unsigned long long numerator = denominator;
    if (minus_one_pow_is_even) {
        ++numerator;
    } else {
        --numerator;
    }
    numerator = (numerator * opposite_3) % simple_module;
    std::cout << numerator << "/" << denominator;
    return 0;
}
