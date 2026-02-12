#include <iostream>

class Fraction {
    int nom;
    int dem;
public:
    int getNumerator() const {return nom;}
    int getDenominator() const {return dem;}

    Fraction(int a, int b = 1) : nom(a), dem(b) {
        int r = reduce(a, b);
        nom = a / r;
        dem = b / r;
    }
private:
    int reduce(int a, int b) {
        if (a == 0 || b == 0) return a + b;
        return reduce(b, a % b);
    }
};

int main() {
    int a, b;
    std::cin >> a >> b;
    Fraction f(a, b);
    std::cout << f.getNumerator() << '/' << f.getDenominator() << std::endl;
}