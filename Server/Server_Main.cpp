#include "Server.h"


/* ========================================================================================================
 *                                              Server Start                                               
 * - This sets up the server socket
 * - Binds the server
 * - Sets up listening
 * - and finally calls the main server loop
 * ========================================================================================================
 */
void Server::start(int port) {
    struct sockaddr_in server_address, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    // ----------------------------------------- Server Set Up
    server_address.sin_family                   =   AF_INET;                // Address family (IPv4)
    server_address.sin_port                     =   htons(port);            // Port number (converted to network byte order)
    server_address.sin_addr.s_addr              =   INADDR_ANY;             // Bind to all available network interfaces
    memset(&(server_address.sin_zero),'\0', 8);                             // Zero out the rest of the struct


    // ----------------------------------------- Server Bind
    check(
        bind(serverSocket,
             (struct sockaddr*) &server_address,
             sizeof(server_address)),
        std::string("> Error binding\n")
            + "> \t - This usually means the port is already in use\n"
            + "> \t - Please Disconnect other server, or give some time for port to clear\n");
    

    // ----------------------------------------- Server Listen
    check(listen(serverSocket, MAX_CLIENTS), "Error listening\n");

    std::cout   << "> Server started. Listening on port : " 
                << port
                << "\n> " << std::flush;

   // ----------------------------------------- Server Loop
    server_loop();
}

/* ========================================================================================================
 *                                               Server Loop                                               
 * This is the main server loop which runs forever after the server is started.
 * The loop polls between the terminal input and server socket.
 * It checks for user connections and if so spawns a thread
 * Or it checks for terminal input where if it is "exit" it kills the server and
 * threads
 * ========================================================================================================
 */
void Server::server_loop() {
    std::string                 input;
    std::vector<pollfd>         poll_fds(2);
    struct sockaddr_in          clientAddr;
    socklen_t                   clientAddrSize = sizeof(clientAddr);

    // ----------------------------------------- Server Poll Creation
    poll_fds[0].fd = serverSocket;      poll_fds[0].events = POLLIN;
    poll_fds[1].fd = STDIN_FILENO;      poll_fds[1].events = POLLIN;

    while (true) {
        // ----------------------------------------- Wait for Poll
        int activity = poll(poll_fds.data(), poll_fds.size(), -1);
        check(activity, "Error Polling");

        // ----------------------------------------- User Connection
        if (poll_fds[0].revents & POLLIN) {

            //  ----------------------------------------- Accept Connection
            int clientSocket = accept( serverSocket, (struct sockaddr*) &clientAddr, &clientAddrSize );
            check(clientSocket, "Error accepting connection\n");

            //  ----------------------------------------- Create a Thread
            pthread_t thread_id;
            int* client_Socket = new int(clientSocket);
            pthread_create(&thread_id, NULL, client_handler, client_Socket);

        // ----------------------------------------- Terminal Input
        } else if (poll_fds[1].revents & POLLIN) {

            // ----------------------------------------- Read Input
            std::getline(std::cin, input);

            // ----------------------------------------- Process Input
            if (input == "exit") {
                printf(
                    "\n----------------------------------------%s%s",
                    "\n             Killing Server             ",
                    "\n----------------------------------------\n"
                );
                server_kill_flag = true;
                while (!connections.empty());
                close(serverSocket);
                return;
            }
        }
    }
}
