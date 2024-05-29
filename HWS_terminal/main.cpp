#include "main.hpp"

SharedMemory sharedMemory(true);

int main()
{
    Logger log("smhometerminal", "/usr/local/sm_home/smhometerminal.log", "a");
    log.systemlog(LOG_INFO, "SmartHomeTerminal has been succesfully started!");

    // Проверка того, как открылась разделяемая память
    if ((sharedMemory.shMemSem == SEM_FAILED) || (sharedMemory.shmFd < 0)) {
        log.systemlog(LOG_ERR, "Error while start to work with shared_memory!");
        return 1;
    }

    if (sharedMemory.copyFromSharedMemory()) {
        log.systemlog(LOG_ERR, "Error to copy data from shared memory!");
    }   

    TerminalGraphic terminal;

    terminal.printBackgroundWindow();
    terminal.printMainMenu(log);


    getch();
    endwin();

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