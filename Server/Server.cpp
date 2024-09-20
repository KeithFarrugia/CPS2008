#include "Server.h"



/* ========================================================================================================
 *                                             Static Variables                                            
 * ========================================================================================================
 */
bool                Server::server_kill_flag    = false;
int64_t             Server::glb_draw_id         = 0;
int                 Server::user_id_rec         = 0;

DrawList                        Server::draw_list;
std::mutex                      Server::connections_mutex;
std::mutex                      Server::draw_mutex;
std::timed_mutex                Server::draw_list_mutex;


std::vector<std::string>                            Server::message_history;
std::unordered_map<std::string, connection_t*>      Server::connections;


/* ========================================================================================================
 *                                               Constructor                                               
 * ========================================================================================================
 */
Server::Server() {
    serverSocket = socket(   
        AF_INET,            // TCP/UDP
        SOCK_STREAM,        // Specify TCP
        IPPROTO_TCP         // Specify Tcp
    );
    connections.clear();
    check(serverSocket , "Error creating socket\n");

    draw_list.init_draw_list("fonts/SpaceMono-Regular.ttf", 10);
}

/* ========================================================================================================
 *                                               De-Structor                                               
 * ========================================================================================================
 */
Server::~Server() {}


/* ========================================================================================================
 *                                           Broadcast Message                                           
 * Sends a given message to all clients
 * ========================================================================================================
 */
void Server::broadcast_message(const std::string message) {
    connections_mutex.lock();
    for (const auto con : connections) {
        send_message(message, con.second->socket);
    }
    connections_mutex.unlock();
}

/* ========================================================================================================
 *                                               Send Message                                               
 * Sends a given message to a chosen client,
 * Allows for bulk sending
 * ========================================================================================================
 */
void Server::send_message(const std::string message, const int clientSocket ) {
    // ---------------------------------------------- Variables
    int bytesSent           = 0;
    uint32_t messageLength  = htonl(message.size());

    // ---------------------------------------------- Send Message Length
    send(clientSocket, &messageLength, sizeof(messageLength), 0);

    // ---------------------------------------------- Send message in chunks
    while (bytesSent < message.size()) {
        // ---------------------------------------------- Calculate
        int remainingBytes  = message.size() - bytesSent;
        int bytesToSend     = std::min(BUFFER_SIZE, remainingBytes);

        // ---------------------------------------------- Send
        send(clientSocket, message.data() + bytesSent, bytesToSend, 0);

        // ---------------------------------------------- Update
        bytesSent           += bytesToSend;
    }
}

/* ========================================================================================================
 *                                             Recieve Messages                                            
 * Reads a message from a user, allows for bulk recieving
 * ========================================================================================================
 */

std::string Server::receive_message(int clientSocket) {
    // ---------------------------------------------- Variables
    int bytesReceived       = 0;
    int totalReceived       = 0;
    uint32_t messageLength  = 0;
    char buffer[BUFFER_SIZE];
    std::string message;

    // ---------------------------------------------- Recieve Message Length
    bytesReceived = recv(clientSocket, &messageLength, sizeof(messageLength), 0);
    if (bytesReceived != sizeof(messageLength)) { return "Disconnect"; }
    messageLength = ntohl(messageLength);
    
    
    // ---------------------------------------------- Read Message
    while (totalReceived < messageLength) {

        // ---------------------------------------------- Read a Chunk of maximum 1024
        bytesReceived = recv(clientSocket, buffer, std::min<int>(BUFFER_SIZE, messageLength - totalReceived), 0);
        if (bytesReceived <= 0) { return "Disconnect"; }

        // ---------------------------------------------- Append message
        message.append(buffer, bytesReceived);

        // ----------------------------------------------  Update
        totalReceived += bytesReceived;
    }
    
    return message;
}



/* ========================================================================================================
 *                                              Check Username                                             
 * Checks if the username is already used by another client
 * ========================================================================================================
 */
bool Server::is_username_in_use(std::string& username){
    connections_mutex.lock();

    if(connections.find(username) == connections.end()){
        connections_mutex.unlock(); 
        return false; 
    };

    connections_mutex.unlock();
    return true;
}

/* ========================================================================================================
 *                                           Check User Activity                                          
 * Checks if the username is already used by another client
 * ========================================================================================================
 */
bool Server::is_user_active(std::string& username){
    connections_mutex.lock();
    con_map_iter user = connections.find(username);

    if(user == connections.end()){connections_mutex.unlock(); return false; };
    if(!user->second->is_active ){connections_mutex.unlock(); return false; };

    connections_mutex.unlock();
    
    return true;
}

/* ========================================================================================================
 *                                       Print a Message to Terminal                                       
 * ========================================================================================================
 */
void Server::print_message(const std::string message) {
    std::cout<< message.c_str() << std::endl << "> " << std::flush;
}

/* ========================================================================================================
 *                                                  Check                                                  
 * Checks if a result is -1, this is because most socket functions return -1 as an error flag
 * ========================================================================================================
 */
void Server::check(int x, std::string message){
    if (x == -1){
        std::cout<< message <<"\n";
        exit(EXIT_FAILURE);
    }
}


/* ========================================================================================================
 *                                            Print Connection                                             
 * Prints a clients connection information
 * ========================================================================================================
 */
void Server::print_connection(const connection_t& c){
    std::fflush(stdin);
    std::cout << "\n> ------------------------------------\n> New Client:\n";

    std::cout << std::setw(15) << std::left << "> Socket";
    std::cout << std::setw(10) << std::right << c.socket;
    std::cout << "\n";
    
    std::cout << std::setw(15) << std::left << "> ID";
    std::cout << std::setw(10) << std::right << c.user_ID;
    std::cout << "\n";
    
    std::cout << std::setw(15) << std::left << "> Username";
    std::cout << std::setw(10) << std::right << c.username;
    std::cout << "\n";
    
    std::cout << std::setw(15) << std::left << "> Activity";
    std::cout << std::setw(10) << std::right << (c.is_active? "Active" : "In-active");
    std::cout << "\n";
    
    std::cout << std::setw(15) << std::left << "> Kill Flag";
    std::cout << std::setw(10) << std::right << (c.kill_flag? "ON" : "OFF");
    std::cout << "\n> ------------------------------------\n> " << std::flush;
}




/* ========================================================================================================
 *                                            Summarise Package                                            
 * Summarises a command package without giving all the detail
 * ========================================================================================================
 */
std::string Server::summarise_package(const PB_Package& package) {
    std::stringstream output;

    switch (package.message_type()) {

        // ----------------------------------------- Insert
        case PB_INSERT: {
            const auto& draw_data = package.draw_data();
                                                        output << "Insert ";
                   if   (draw_data.has_circle())    {   output << "Circle";
            } else if   (draw_data.has_line())      {   output << "Line";
            } else if   (draw_data.has_rectangle()) {   output << "Rectangle";
            } else if   (draw_data.has_text())      {   output << "Text";}
            output << " with Draw ID: " << draw_data.draw_id();
            break;
        }

        // ----------------------------------------- Edit
        case PB_EDIT: {
            const auto& edit_data = package.edit_data().draw_data();
            output << "Edit ";
                   if (edit_data.has_circle())      {   output << "Circle";
            } else if (edit_data.has_line())        {   output << "Line";
            } else if (edit_data.has_rectangle())   {   output << "Rectangle";
            } else if (edit_data.has_text())        {   output << "Text";}
            output << " with Draw ID: " << package.edit_data().draw_id();
            break;
        }

        // ----------------------------------------- Clear 
        case PB_CLEAR:      output << "Clear User ID: "     << package.user_id();   break;

        // ----------------------------------------- Error 
        case PB_ERROR:      output << "Error: "             << package.generic();   break;
        
        // ----------------------------------------- Generic 
        case PB_GENERIC:    output << "Generic: "           << package.generic();   break;
        
        // ----------------------------------------- Undo 
        case PB_UNDO:       output << "Undo";                                       break;
        
        // ----------------------------------------- Invalid 
        case PB_INVALID:    output << "Invalid";                                    break;
        
        // ----------------------------------------- Remove 
        case PB_REMOVE:     output << "Remove Draw ID: "    << package.draw_id();   break;
        
        // ----------------------------------------- Bulk 
        case PB_BULK:       
                            output << "Bulk with "      
                                   << package.bulk_draw_data().draw_data_size() << " elements";
            break;

        // ----------------------------------------- Unkown 
        default:
            output << "Unknown message type";
            break;
    }

    return output.str();
}