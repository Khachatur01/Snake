#include <iostream>
#include "game/game.h"

int main() {
    uint16_t width = 10;
    uint16_t height = 10;
    uint16_t choice;

    std::cout << "Welcome to Snake game" << std::endl;
    std::cout << "Input width of board: ";
    std::cin >> width;
    std::cout << "Input height of board: ";
    std::cin >> height;
    do {
        std::cout << "Borders\n1) Solid\n2) Non Solid\n: ";
        std::cin >> choice;
    } while (choice != 1 && choice != 2);

    Snake snake(width, height, choice == 1);
    std::cout << std::endl;
    return 0;
}
