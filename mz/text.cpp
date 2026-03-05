#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

class Text {
    std::vector <std::string> words;
    std::map <std::string, std::vector <int>> word_map;

    public:
    int get_length() const {
        return words.size();
    }

    Text& operator+ (std::string s) {
        words.push_back(s);
        word_map[s].push_back(words.size() - 1);
        return *this;
    }

    Text& operator- (std::string s) {
        if (word_map.find(s) != word_map.end()) {
            std::vector<int>& vec = word_map[s]; // link to indexes

            std::sort(vec.begin(), vec.end(), std::greater<int>());
            for (int index : vec) { // cleanup
                words.erase(words.begin() + index);
            }
            word_map.erase(s);
        }
        return *this;
    }

    std::string operator[] (int index) const {
        return words[index];
    }

    Text& operator+ (const Text& t) {
        for (int i = 0; i < t.get_length(); i++) {
            (*this) + t[i];
        }
        return *this;
    }
};

int main() {
    Text T;
    T + "aboba";
    T + "Miumiu";
    T + "aaa";
    T + "Miumiu";
    // S = T - Q - W;
    std::cout << "T: ";
    for (int i = 0; i < T.get_length(); i++) {
        std::cout << T[i] << ' ';
    }
    std::cout << std::endl;

    T - "Miumiu";
    std::cout << "T after -: ";
    for (int i = 0; i < T.get_length(); i++) {
        std::cout << T[i] << ' ';
    }
    std::cout << std::endl;

    Text A;
    A + "aboba";
    A + "pupupu";
    std::cout << "A: ";
    for (int i = 0; i < A.get_length(); i++) {
        std::cout << A[i] << ' ';
    }
    std::cout << std::endl;

    A + T;
    std::cout << "A + T: ";
    for (int i = 0; i < A.get_length(); i++) {
        std::cout << A[i] << ' ';
    }
    std::cout << std::endl;
    return 0;
}