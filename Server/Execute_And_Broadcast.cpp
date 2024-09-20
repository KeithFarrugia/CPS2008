#include "Server.h"

/* ========================================================================================================
 *                                            Execute and Broadcast                                             
 * Tries to access the drawlist mutex, If unable to by a certain amount of time
 * it returns to do other work.
 * 
 * If the mutex lock was able to be taken then the command is executed and sent to the clients.
 * ========================================================================================================
 */
void Server::execute_and_broadcast(std::string& username, int clientSocket, std::queue<PB_Package>& messages_buf) {
    PB_Package package;

    // ---------------------------------------------- Try to lock for a time limit
    if (!draw_list_mutex.try_lock_for(std::chrono::milliseconds(MUTEX_TIMEOUT))) {
        return;
    }

    // ---------------------------------------------- Remove message from queue
    package = messages_buf.front(); messages_buf.pop();
    print_message("Client [" + username + "] => " + summarise_package(package));


    // ---------------------------------------------- Execute
    switch (package.message_type()){
        // ---------------------------------------------- INSERT
        case PB_MessageType::PB_INSERT: 
            draw_list.insert(package.draw_data());
            break;
        
        // ---------------------------------------------- EDIT
        case PB_MessageType::PB_EDIT:
            draw_list.edit(
                package.edit_data().draw_id(),
                package.edit_data().draw_data()
            );
            break;
        
        // ---------------------------------------------- DELETE
        case PB_MessageType::PB_REMOVE:
            draw_list.erase(package.draw_id());
            break;
        
        // ---------------------------------------------- CLEAR
        case PB_MessageType::PB_CLEAR :
            draw_list.clear(package.user_id());
            break;
        
        // ---------------------------------------------- UNDO
        case PB_MessageType::PB_UNDO :
            draw_list.undo(package.user_id());
            break;

        // ---------------------------------------------- DEFUALT
        default:
            break;
    }

    // ---------------------------------------------- Unlock Mutex
    draw_list_mutex.unlock();


    // ---------------------------------------------- broadcast
    broadcast_message(package.SerializeAsString());
}