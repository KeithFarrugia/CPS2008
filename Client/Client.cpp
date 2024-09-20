#include "Client.h"
/* =====================================================================================================================
 *                                                   Static Variables                                                   
 * =====================================================================================================================
 */
int         Client::clientSocket            = -1;
bool        Client::test_flag               = false;
bool        Client::kill_flag               = false;
bool        Client::is_server_thread_dead   = false;
bool        Client::is_render_thread_dead   = false;
bool        Client::should_render           = true;
long        Client::user_id                 = -1;


token_t                     Client::current_show;
DrawList                    Client::draw_list;
std::mutex                  Client::draw_mutex;
std::mutex                  Client::message_queue_mutex;
std::queue<std::string>     Client::message_queue;

/* =====================================================================================================================
 *                                                     Constructor                                                   
 * =====================================================================================================================
 */
Client::Client(const std::string& ip_address, const std::string& username, bool test_flag){

    // ---------------------------------------------- Initialize non-static Variables
    lexer               = Lexer();                  // Init Lexer
    select_id           = -1;                       // default select id
    should_render       = true;                     // set should render to true to generate rulers
    this->username      = username;                 // set username
    this->test_flag     = test_flag;                // set test flag
    current_tool        = token_t::RECTANGLE;       // set defualt draw tool
    current_show        = token_t::ALL;             // set defualt show type
    
    // ---------------------------------------------- Initialize defulat draw colour (black)
    current_colour.set_r(0);
    current_colour.set_g(0);  
    current_colour.set_b(0); 
    current_colour.set_a(255); 
    
    // ---------------------------------------------- Initialize socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        print_message("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    // ---------------------------------------------- Initialize drawlist (select font and font size)
    draw_list.init_draw_list("fonts/SpaceMono-Regular.ttf", 30);
}

/* =====================================================================================================================
 *                                                     De-Structor                                                      
 * =====================================================================================================================
 */
Client::~Client() {
    // ---------------------------------------------- Close socket
    close(clientSocket);
}

/* =====================================================================================================================
 *                                                   Message Printing                                                   
 * The following functions take care of printing messages to the terminal
 * They makes use of coloured output as well as making sure to print the ">" symbol
 * =====================================================================================================================
 */

// ---------------------------------------------- Introduction Message
void Client::print_opening_message(std::string ip_address, int port) {
    const std::string CYAN = "\033[36m";
    const std::string RESET = "\033[0m";
    if(test_flag == false){
        std::cout << CYAN 
        << std::endl
        << "======================================================================" << std::endl
        << "                          Welcome Net-Scetch                          " << std::endl
        << "            Connected to server :       " <<  ip_address                << std::endl
        << "            Connected to port   :       " <<  port                      << std::endl
        << "======================================================================" << std::endl
        << std::endl << RESET << std::flush;
        print_message();
    }
}

// ---------------------------------------------- General Message
void Client::print_message(const std::string message) {
    const std::string CYAN = "\033[36m";
    const std::string GREEN = "\033[32m";
    const std::string RESET = "\033[0m";
    if(test_flag == false){
        std::cout << CYAN << message.c_str() << std::endl << GREEN << "> " << RESET << std::flush;
    }
}

// ---------------------------------------------- Print blank message
void Client::print_message() {
    const std::string GREEN = "\033[32m";
    const std::string RESET = "\033[0m";
    if(test_flag == false){
        std::cout << GREEN << "> " << RESET << std::flush;
    }
}
/* ========================================================================================================
 *                                              Send Message                                              
 * Sends a message to the server. First sends the length of the message. 
 * The sends the message itself. If the message is to large then it is split
 * into smaller checks and each is sent individually
 * ========================================================================================================
 */
void Client::send_message(const std::string message, int clientSocket) {
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
 *                                             Recieve Message                                             
 * Recieves a message from the server, if the message is to large then
 * it reads multiple chunks and concatinates them into a single string.
 * ========================================================================================================
 */
std::string Client::recieve_message(int clientSocket) {
    // ---------------------------------------------- Variables
    int bytesReceived       = 0;
    int totalReceived       = 0;
    uint32_t messageLength  = 0;
    std::string message;
    char buffer[BUFFER_SIZE];

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