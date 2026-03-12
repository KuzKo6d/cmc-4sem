#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>


class Text {
    std::vector<std::string> words;
    std::vector<bool> alive;
    std::map<std::string, std::vector<int>> word_map;

public:
    int get_length() const {
        int cnt = 0;
        for (int j = 0; j < alive.size(); ++j) {
            if (alive[j] == true) cnt++;
        }
        return cnt;
    }

    Text& operator+ (const std::string& s) { // add string
        words.push_back(s);
        alive.push_back(true);
        word_map[s].push_back(words.size() - 1);
        return *this;
    }

    Text& operator- (const std::string& s) { // turn deleted flags
        if (word_map.find(s) != word_map.end()) {
            std::vector<int>& vec = word_map[s];
            for (int i = 0; i < vec.size(); ++i) {
                alive[vec[i]] = false;
            }
            word_map.erase(s);
        }
        return *this;
    }

    std::string operator[] (int index) const { // get by index
        if (index >= 0 && index < words.size() && alive[index] ) {
            return words[index];
        }
        return "";
    }

    Text& remap() {
        for (int i = alive.size() - 1; i >= 0; i--) {
            if (!alive[i]) {
                words.erase(words.begin() + i);
                alive.erase(alive.begin() + i);
            }
        }
        word_map.clear();
        for (int i = 0; i < words.size(); i++) {
            word_map[words[i]].push_back(i);
        }
        return *this;
    }

    Text& operator= (const Text& t) {
        if (this == &t) return *this;

        words.clear();
        alive.clear();
        word_map.clear();

        for (int i = 0; i < t.words.size(); i++) {
            if (t.alive[i]) {
                words.push_back(t.words[i]);
                alive.push_back(true);
                word_map[t.words[i]].push_back(words.size() - 1);
            }
        }
        return *this;
    }

    Text operator+ (const Text& t) const {
        Text result = *this;
        for (int i = 0; i < t.get_length(); i++) {
            result + t[i];
        }
        return result;
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

    A = A + T;
    std::cout << "A + T: ";
    for (int i = 0; i < A.get_length(); i++) {
        std::cout << A[i] << ' ';
    }
    std::cout << std::endl;

    Text Res;
    Res = A - T;
    std::cout << "A - T: ";
    for (int i = 0; i < Res.get_length(); i++) {
        std::cout << Res[i] << ' ';
    }
    std::cout << std::endl;

    return 0;
}