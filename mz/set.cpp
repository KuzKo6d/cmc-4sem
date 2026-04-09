#include <iostream>
#include <vector>
#include <algorithm>

template <class T>
class Set {
    std::vector<T> a;

public:
    const std::vector<T>& get() const {
        return a;
    }

    Set& add(const T& elem) {
        if (std::find(a.begin(), a.end(), elem) == a.end()) {
            a.push_back(elem);
        }
        return *this;
    }

    Set& del(const T& elem) {
        auto it = std::find(a.begin(), a.end(), elem);
        if (it != a.end()) {
            a.erase(it);
        }
        return *this;
    }

    bool in(const T& elem) const {
        return std::find(a.begin(), a.end(), elem) != a.end();
    }

    Set operator* (const Set<T>& b) const {
        Set<T> result;
        for (const T& elem : a) {
            if (b.in(elem)) {
                result.add(elem);
            }
        }
        return result;
    }

    Set operator+ (Set<T> b) {
        Set<T> result = *this;
        for (const T& elem : b.get()) {
            result.add(elem);
        }
        return result;
    }

    Set operator+ (const T& elem) {
        Set<T> result = {elem};
        return *this + result;
    }

    void display () const {
        std::cout << "{";
        for (const T& elem : a) {
            std::cout << elem << " ";
        }
        std::cout << "}" << std::endl;
    }
};

int main() {
    Set<int> set1;
    set1.add(1).add(2).add(3).add(2);  // 2 won't be added twice

    Set<int> set2;
    set2.add(2).add(3).add(4);

    std::cout << "Set1: ";
    set1.display();  // { 1 2 3 }

    std::cout << "Set2: ";
    set2.display();  // { 2 3 4 }

    // Union
    Set<int> unionSet = set1 + set2;
    std::cout << "Union: ";
    unionSet.display();  // { 1 2 3 4 }

    // Intersection
    Set<int> intersectSet = set1 * set2;
    std::cout << "Intersection: ";
    intersectSet.display();  // { 2 3 }

    // Check membership
    std::cout << "Is 2 in set1? " << (set1.in(2) ? "Yes" : "No") << std::endl;
    std::cout << "Is 5 in set1? " << (set1.in(5) ? "Yes" : "No") << std::endl;

    // Delete element
    set1.del(2);
    std::cout << "Set1 after deleting 2: ";
    set1.display();  // { 1 3 }

    return 0;
}