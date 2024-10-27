#include "main.hpp"

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cout << "Incorrect usage! Type usage options." << std::endl;
        return 1;
    }
    if (strcmp(argv[1], "remote") == 0) {
        if (argc != 3) {
            std::cout << "Incorrect usage! Type usage options." << std::endl;
            return 1;
        }
        TerminalGraphic terminal(argv[2]);
    } else if (strcmp(argv[1], "local") == 0) {
        if (argc != 2) {
            std::cout << "Incorrect usage! Type usage options." << std::endl;
            return 1;
        }
        TerminalGraphic terminal;
    } else {
        std::cout << "Incorrect usage of options!" << endl;
        return 1;
    }

    return 0;
}