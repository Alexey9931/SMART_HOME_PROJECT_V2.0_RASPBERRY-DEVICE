#include "main.hpp"

extern SharedMemory sharedMemory;

int main()
{
    sharedMemory.openSharedMemory(true);
    sharedMemory.copyFromSharedMemory();
    

    TerminalGraphic terminal;

    terminal.printBackgroundWindow();
    terminal.printMainMenu();


    getch();
    endwin();
    sharedMemory.closeSharedMemory();
 
    return 0;
}

// NetMapBox::NetMapBox(std::list<ConfigEntry> parsed_configs){
//     for ( ConfigEntry  el : parsed_configs )
//     {
//         if (el.first == "[NETWORK_MAP]")
//         {
//             for ( std::pair<std::string, std::string> el1 : el.second)
//             {
//                 net_map[el1.first] = make_pair(el1.second, false);
//             }
//             break;
//         }
//     }
// }