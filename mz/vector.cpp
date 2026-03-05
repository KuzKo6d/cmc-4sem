#include <iostream>
#include <vector> // Обязательно подключаем

int main() {
    // Разные способы создания вектора:

    // 1. Пустой вектор целых чисел
    std::vector<int> empty_vector;

    // 2. Вектор из 5 целых чисел, заполненных нулями (или конструктором по умолчанию)
    std::vector<int> five_zeros(5); // {0, 0, 0, 0, 0}

    // 3. Вектор из 3 строк, заполненных "Hello"
    std::vector<std::string> string_vector(3, "Hello"); // {"Hello", "Hello", "Hello"}

    // 4. Инициализация списком (C++11 и новее)
    std::vector<int> numbers = {10, 20, 30, 40, 50};
    // или так:
    // std::vector<int> numbers {10, 20, 30, 40, 50};

    // 5. Копирование другого вектора
    std::vector<int> numbers_copy(numbers);

    return 0;
}