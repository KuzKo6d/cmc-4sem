#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

class Text {
    std::vector <std::string> words;
    std::vector <bool> deleted;
    std::map <std::string, std::vector <int>> word_map;

    public:
    int get_length() const {
        return words.size();
    }

    Text& operator+ (std::string s) {
        words.push_back(s);
        deleted.push_back(false);
        word_map[s].push_back(words.size() - 1);
        return *this;
    }

    Text& operator- (std::string s) {
        if (word_map.find(s) != word_map.end()) {
            std::vector<int>& vec = word_map[s]; // link to indexes

            std::sort(vec.begin(), vec.end(), std::greater<int>());
            for (int index : vec) { // cleanup
                words[words.begin() + index] = true;
            }
            word_map.erase(s);
        }
        return *this;
    }

    std::string operator[] (int index) const {
        if (word_map.find(index) != word_map.end()) {
            return words[index];
        }
        return NULL;
    }

    Text& operator+ (const Text& t) {
        for (int i = 0; i < t.get_length(); i++) {
            (*this) + t[i];
        }
        return *this;
    }


    Text& operator= (const Text& t) {
        int len = (*this).get_length();
        for (int i = 0; i < len; i++) { // clear
            (*this) - (*this)[i];
        }

        for (int i = 0; i < t.get_length(); i++) { // copy
            (*this) + t[i];
        }
        return *this;
    }


    Text operator- (const Text& t) const {
        Text result = *this;
        for (int i = 0; i < t.get_length(); i++) {
            result - t[i];
        }
        return result;
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

    Text Res;
    Res = A - T;
    std::cout << "Res = A - T: ";
    for (int i = 0; i < Res.get_length(); i++) {
        std::cout << Res[i] << ' ';
    }
    std::cout << std::endl;

    return 0;
}