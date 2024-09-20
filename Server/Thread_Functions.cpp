#include "Server.h"

/* ========================================================================================================
 *                                             Check Kill Flag                                             
 * Checks if a the kill flags of either the server or the thread are on 
 * 
 * returns [status_t::on]       if either the thread's kill flag is on
 *                              or if the server kill flag is on
 * returns [status_t::off]      both kill flags are off
 * returns [status_t::error]    if map fails to find username
 * ========================================================================================================
 */
status_t Server::check_kill_flag ( const std::string username ){

    connections_mutex.lock();
    con_map_iter c_m = connections.find(username);

    // ----------------------------------------------------------- Username Not Found
    if(c_m == connections.end()){
        connections_mutex.unlock();     return status_t::error;

    // ---------------------------------------- Kill Flag On
    }else if(Server::server_kill_flag == true || c_m->second->kill_flag){
        connections_mutex.unlock();     return status_t::on;

    // ---------------------------------------- Kill Flag Off
    }else{
        connections_mutex.unlock();     return status_t::off;
    }
}

/* ========================================================================================================
 *                                            Disconnect A User                                            
 * Disconnects a User by closing the socket and setting the is_active to false
 * 
 * returns [status_t::success]  upon successfull disconnection
 * returns [status_t::error]    upon not finding the user or close causing an error
 * ========================================================================================================
 */
status_t Server::disconnect_user ( const std::string username ){
    connections_mutex.lock();
    con_map_iter c_m = connections.find(username);

    // ---------------------------------------- Not Found
    if(c_m == connections.end()){
        connections_mutex.unlock();     return status_t::error;

    // ---------------------------------------- Found
    }else {
        c_m->second->is_active = false;
        try{    close(c_m->second->socket);     }catch(const std::exception& e){    connections_mutex.unlock();     return status_t::error;}
    }
    connections_mutex.unlock();     return status_t::success;
}

/* ========================================================================================================
 *                                               Remove User                                               
 * Removes a User from connection map after disconnecting them
 * 
 * returns [status_t::success]  upon successfull disconnection & removal
 * returns [status_t::error]    upon not finding the user
 * ======================================================================================================== 
 */
status_t Server::remove_user ( const std::string username ){
    
    // ---------------------------------------- Disconnect User
    disconnect_user(username);

    // ---------------------------------------- Remove User
    connections_mutex.lock();
    con_map_iter c_m = connections.find(username);

    // ---------------------------------------- Not Found
    if(c_m == connections.end()){ connections_mutex.unlock(); return status_t::error; }
    
    // ---------------------------------------- Found
    print_message("Killing Thread For: \t\t [" + username + "]");
    connections.erase(c_m);

    connections_mutex.unlock();     return status_t::success;
}


/* ========================================================================================================
 *                                             Re-Connect User                                             
 * Reconnects the user by setting the appropriote values
 * 
 * returns [status_t::success]  upon successfull disconnection & removal
 * returns [status_t::error]    upon not finding the user
 * ======================================================================================================== 
 */
status_t Server::re_connect_user ( const std::string username, const int new_socket ){

    // ---------------------------------------- Remove User
    connections_mutex.lock();
    con_map_iter c_m = connections.find(username);

    // ---------------------------------------- Not Found
    if(c_m == connections.end()){ connections_mutex.unlock();     return status_t::error; }
    
    // ---------------------------------------- Found
    send_message(std::to_string(c_m->second->user_ID), c_m->second->socket);
    c_m->second->is_active = true;
    c_m->second->socket    = new_socket;

    connections_mutex.unlock();     return status_t::success;
}

/* ========================================================================================================
 *                                             Create New User                                             
 * Creates a new User
 * ======================================================================================================== 
 */

status_t Server::create_new_user ( connection_t& connection ){
    connections_mutex.lock();
    con_map_iter c_m = connections.find(connection.username);

    // ---------------------------------------- Not Found
    if(c_m != connections.end()){ connections_mutex.unlock();     return status_t::error; }
    
    // ---------------------------------------- Found
    connections[connection.username]   = &connection;

    connections_mutex.unlock();     return status_t::success;
}