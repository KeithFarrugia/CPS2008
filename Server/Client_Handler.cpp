#include "Server.h"
/* ========================================================================================================
 *                                        Client Connection Handler                                        
 * This is the thread per Client which recieves messages executes them and broadcasts them
 * ========================================================================================================
 */
void* Server::client_handler(void* clientSocket_ptr) {

    // ----------------------------------------------- Variables
    connection_t            connection;
    struct pollfd           fds[1];
    std::queue<PB_Package>  messages_buf;
    time_point_t            last_activity_time = std::chrono::steady_clock::now();
    
    // ----------------------------------------------- Set the Socket
    connection.socket =  *(int*)clientSocket_ptr;   free(clientSocket_ptr);

    // ----------------------------------------------- Poll Setup
    fds[0].fd = connection.socket;
    fds[0].events = POLLIN;


    // ----------------------------------------------- Check Username
    if(check_client(connection, connection.socket) != success){ pthread_exit(0); return nullptr; }

    // ----------------------------------------------- Setting user name
    PB_Package package_username;
    package_username.set_message_type(PB_MessageType::PB_BULK);
    package_username.mutable_bulk_draw_data()->CopyFrom(draw_list.serialize());
    send_message(package_username.SerializeAsString(), connection.socket);

    
    // ----------------------------------------------- Loop
    while (true) {

        // ----------------------------------------------- Poll
        if(poll(fds, 1, POLL_TIMEOUT) < 0){
            print_message("Error Polling on thread for ["+connection.username+"]");
        };

        // ----------------------------------------------- Is Server/Thread Set to Shutdown?
        if (check_kill_flag(connection.username) != off){
            remove_user(connection.username); pthread_exit(0); return nullptr;
        }

        if (fds[0].revents & POLLIN){
            // Data is available, receive and process it
            std::string message = receive_message(connection.socket);

            // ----------------------------------------------- Disconnect
            if (message == "Disconnect") {
                PB_Package package;

                // ----------------------------------------------- Wait for reconnect
                if(handle_reconnect(connection) != status_t::success){ pthread_exit(0); return nullptr; }
                
                // ----------------------------------------------- Successfull Reconnect -> send bulk
                package.set_message_type(PB_MessageType::PB_BULK);
                package.mutable_bulk_draw_data()->CopyFrom(draw_list.serialize());
                send_message(package.SerializeAsString(), connection.socket);

                last_activity_time = std::chrono::steady_clock::now();

            // ----------------------------------------------- Successfull Message
            }else{
                PB_Package package;
                package.set_message_type(PB_INVALID);
                package.ParseFromString(message);

                // ----------------------------------------------- If its an insert -> add draw_id
                if( package.message_type() == PB_MessageType::PB_INSERT){
                    draw_mutex.lock();
                    PB_DrawData* draw_data = package.mutable_draw_data();
                    draw_data->set_draw_id(glb_draw_id++);
                    draw_mutex.unlock();
                }
                
                // ----------------------------------------------- Check if an invalid
                if(package.message_type() != PB_MessageType::PB_INVALID){
                    messages_buf.push(package);
                    last_activity_time = std::chrono::steady_clock::now();
                }
                
            }
            
            // ----------------------------------------------- Nothing Recieved -> Do somthing else
        } else {
            if(!messages_buf.empty()){
                execute_and_broadcast(connection.username, connection.socket, messages_buf);
            }
        }

        // ----------------------------------------------- Check inactivity timeout
        time_point_v2_t current_time = std::chrono::steady_clock::now();
        int64_t         elapsed_time = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_activity_time).count();
        if (elapsed_time >= INACTIVITY_TIME_Limit) {
            PB_Package package, error_package;

            // ----------------------------------------------- Send Error Message
            print_message("Client [ " + connection.username + "] has been disconnected due to inactivity");
            error_package.set_message_type(PB_MessageType::PB_ERROR);
            error_package.set_generic("You have been inactive for 5 minutes");
            send_message(error_package.SerializeAsString(), connection.socket);
            connection.is_active = false;

            // ----------------------------------------------- Wait for reconnect
            sleep(1);
            if(handle_reconnect(connection) != status_t::success){ pthread_exit(0); return nullptr; }
            
            // ----------------------------------------------- Successfull Reconnect -> send bulk
            package.set_message_type(PB_MessageType::PB_BULK);
            package.mutable_bulk_draw_data()->CopyFrom(draw_list.serialize());
            send_message(package.SerializeAsString(), connection.socket);

            last_activity_time = std::chrono::steady_clock::now();
        }
    }

    pthread_exit(0); return nullptr;
}

/* ========================================================================================================
 *                                              Handle Reconnect                                               
 * Handles reconnect by checking the clients status, goes to sleep for 1 second and repeats
 * until the client either reconnects or the time runs out
 * ========================================================================================================
 */

status_t Server::handle_reconnect(connection_t& connection) {
    print_message("Client [" + connection.username + "] disconnected");

    // ----------------------------------------------- Try to diconnect the user
    if(disconnect_user(connection.username) != status_t::success){
        remove_user(connection.username);       return status_t::error;
    }
    
    // ----------------------------------------------- Loop until time limit
    for (int i = 0; i < RECONNECT_TIME; i++) { 
        
        // ----------------------------------------------- Sleep 1 Second
        sleep(1);
        
        // ----------------------------------------------- Check Kill Flag
        if (check_kill_flag(connection.username) != off){
            remove_user(connection.username);   return status_t::error;
        }

        // ----------------------------------------------- Check Status 
        connections_mutex.lock();
        
        // ----------------------------------------------- Successfull Reconnect
        if (connection.is_active == true) {
            PB_Package package;
            // ----------------------------------------------- Send welcome back
            package.set_message_type(PB_MessageType::PB_GENERIC);
            package.set_generic("Welcome Back");
            send_message(package.SerializeAsString(), connection.socket);
            
            // ----------------------------------------------- Print and return
            print_message("Client [ " + connection.username + "] re-connected");
            connections_mutex.unlock(); return status_t::success;

        }
        
        connections_mutex.unlock();
    }
    
    // ----------------------------------------------- Unsuccessfull Reconnect
    print_message("Client [ " + connection.username + "] did not re-connected : Terminating Thread");
    remove_user(connection.username);           return status_t::failure;
}

/* ========================================================================================================
 *                                               Check Client                                               
 * Takes care of the client connection initialization
 *  - Waits for the username to arrive from the client
 *  - Makes Sure that the Username is not already in use
 *  - If the Username is in use then checks if that client was
 *    disconnected before
 * ========================================================================================================
 */
status_t Server::check_client(connection_t& connection, int& clientSocket){
    char username_buffer[BUFFER_SIZE];
    status_t state;

    std::string username = receive_message(clientSocket);
    if (username == "Disconnect") {
        print_message("Error receiving username from client");
        close(clientSocket);    return status_t::error;
    }

    // --------------------------- Client Not Found -> New Client
    if (!is_username_in_use(username)) {
        connection.socket       = clientSocket;
        connection.user_ID      = user_id_rec++;
        connection.username     = username;
        connection.kill_flag    = false;
        connection.is_active    = true;
        
        if (create_new_user(connection) != status_t::success){ return status_t::failure; };
        print_connection(connection);
        send_message(std::to_string(connection.user_ID), clientSocket);
        return status_t::success;

    // --------------------------- Client Found -> Was Disconnected
    }else if (!is_user_active(username))     {
        re_connect_user(username, clientSocket);
        return status_t::failure;

    // --------------------------- Username is already used by an active client
    }else{
        send_message(std::to_string(-1), clientSocket);
        PB_Package package;
        package.set_message_type(PB_MessageType::PB_ERROR);
        package.set_generic("Username In Use");
        send_message(package.SerializeAsString(), clientSocket);
        close(clientSocket);
        return status_t::failure;
    }
}

