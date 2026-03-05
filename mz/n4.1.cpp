#include <iostream>

class ECounter {
    int max = 52;
    int counter = 0;
public:
    int getMax() const {
        return this->max;
    }

    void setMax(int max) {
        this->max = max;
    }

    int getCounter() const {
        return this->counter;
    }

    void increaseCounter(int x) {
        this->counter += x;
        if (this->counter > max) {
            this->counter = 0;
        }
    }
};

int main () {
    ECounter counter;
    int max = 0;
    std::cin >> max;
    counter.setMax(max);
    int count = 0;

    while (true) {
        std::cin >> count;
        if (count == 0) break;
        counter.increaseCounter(count);
        std::cout << "penis: " << counter.getCounter() << '\n';
    }
    std::cout << std::endl;
}