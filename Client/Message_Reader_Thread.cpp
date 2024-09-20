#include "Client.h"

/* ========================================================================================================
 *                                               Server Loop                                               
 * This loop's purpose is to try and remove messages from the socket buffer as quickly as possible
 * ========================================================================================================
 */
void* Client::server_loop (void*){
    is_server_thread_dead = false;

    // ----------------------------------------- Poll Setup
    struct pollfd poll_fds[1];
    poll_fds[0].fd = clientSocket;
    poll_fds[0].events = POLLIN;

    // ----------------------------------------- Server Loop
    while(!kill_flag){

        // ----------------------------------------- Poll
        if(poll(poll_fds, 1, 100) == -1){
            print_message("Error Polling");
            kill_flag = true; break;
        };

        // ----------------------------------------- Received Message
        if (poll_fds[0].revents & POLLIN) {
            // ---------------------------------------------- READ MESSAGE
            std::string message = recieve_message(clientSocket);

            // ---------------------------------------------- Check for Disconnect
            if (message == "Disconnect") {
                print_message("Server has disconnected/shutdown");
                kill_flag = true;
            }

            // ---------------------------------------------- Move message to Queue
            message_queue_mutex.lock();
            message_queue.push(message);
            message_queue_mutex.unlock();
        }
    }


    is_server_thread_dead = true;
    pthread_exit(0); return nullptr;
}