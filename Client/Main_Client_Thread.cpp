#include "Client.h"
/* ========================================================================================================
 *                                            Main Client Loop                                             
 * Its purpose is to poll user input and make sure everything terminates correctly
 * ========================================================================================================
 */
void Client::client_loop() {
    std::string message;

    // ----------------------------------------- Poll Setup
    struct pollfd poll_fds[1];
    poll_fds[0].fd      = STDIN_FILENO;
    poll_fds[0].events  = POLLIN;

    // ----------------------------------------- Client Loop
    while (!kill_flag) {
        // ----------------------------------------- Poll
        if(poll(poll_fds, 1, 100) == -1){
            print_message("Error Polling");
            kill_flag = true; break;
        };

        // ----------------------------------------- Received Input From Terminal
        if (poll_fds[0].revents & POLLIN) {
            std::getline(std::cin, message); print_message();
            if (message == "exit") { kill_flag = true; }
            parse_input(message);
        }
    }

    // ----------------------------------------- Wait for threads to close
    while (!is_server_thread_dead || !is_render_thread_dead);
    
    // ----------------------------------------- Print Exit message
    if(test_flag == false){
        const std::string CYAN = "\033[36m";
        std::cout << CYAN << "Exiting ........... " << std::endl << std::flush;

    }else{
        std::cout << "[" << std::setw(15) << std::left << username; 
        std::cout <<"]: ";
        std::cout <<draw_list.generate_hash() << std::endl << std::flush;
    }
}