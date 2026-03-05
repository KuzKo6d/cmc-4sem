#include <iostream>
#include <fstream>
#include <mutex>
#include <stdio.h>
#include <map>
#include <string>

class Singleton {
private:
    static Singleton* instancePtr;
    static std::mutex mtx;
    std::map <std::string, bool>& filenamesMap;

    std::string filename;
    FILE* file = 0;

    Singleton(std::string filename) : filename(filename), file(nullptr) {
        std::cout << "Created instance for file: " << filename << std::endl;
    }

public:
    Singleton(const Singleton& obj) = delete;
    Singleton& operator=(const Singleton& obj) = delete;

    ~Singleton() {
        if (file != nullptr) {
            fclose(file);
            std::cout << "Destroyed instance for file: " << filename << std::endl;
        }
        std::lock_guard<std::mutex> lock(mtx);
        filenamesMap[filename] = false;
        std::cout << "File " << filename << "is now free." << std::endl;
    }

    static Singleton& getInstance(std::string filename) {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = filenamesMap.find(filename);
        if (it != filenameMap.end() && it-> second) {
            throw std::runtime_error("Instance with " + filename + " already exists.");
        }

        if (instancePtr == nullptr) {
            instancePtr = new Singleton(filename);
            filenamesMap[filename] = true;
        }

        return *instancePtr;
    }

    bool openFile() {
        if (file == nullptr) {
            file = fopen(filename.c_str(), "w");
            return (file != nullptr);
        }
        return true; // file already opened
    }

    bool closeFile() {
        if (file != nullptr) {
            fclose(file);
        }
        return (file == nullptr);
    }

    bool writeFile(std::string text) {
        if (file != nullptr) {
            size_t written = fwrite(text.c_str(), sizeof(char), text.length(), file);
            fflush(file);
            return (written == text.length());
        }
        return false;
    }

    bool isOpen() {
        return (file != nullptr);
    }
};

Singleton* Singleton::instancePtr = nullptr;
std::mutex Singleton::mtx;

int main() {
    Singleton& s1 = Singleton::getInstance("text.txt");
    s1.openFile();
    s1.writeFile("miumiu");
    s1.closeFile();
    std::cout << "wrote some text to .txt file" << std::endl;

    std::cout << "\n";
    Singleton& s2 = Singleton::getInstance("text.md");
    std::cout << s2.openFile();
    std::cout << s2.writeFile("miumiu2");
    s2.closeFile();
    std::cout << "wrote some text to .md file" << std::endl;
    return 0;
}