#include <iostream>

int calculate(int a, int b) {
    return a * b + 123;
}
//комментарий
int main() {
    int x = 15;
    int y = 30;
    std::cout << "Result: " << calculate(x, y) << std::endl;
}