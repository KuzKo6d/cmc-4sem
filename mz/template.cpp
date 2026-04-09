#include <iostream>
#include <tuple>
#include <numeric>

template <typename T> T myMax(T a, T b) {
    return a > b ? a : b;
}

template <class T>
class Fraction (T n, T d) {
    T n;
    T d;

    public:
    std::tuple<T, T> getFr() const {
        return {n, d};
    }

    Fraction operator+(std::tuple<T, T> a) {
        return {a[0] / std::gcd(a[0], a[1]), a[1] / std::gcd(a[0], a[1])};
    }
};

int main() {
    std::cout << myMax(3.1415926535897932384626433, 35.0) << std::endl;
    return 0;
}