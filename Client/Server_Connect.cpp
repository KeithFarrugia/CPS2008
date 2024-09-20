#include "Client.h"

/* ========================================================================================================
 *                                              Connect to Server                                          
 * This is the more complex loop, it will both execute the given commands as well as 
 * render the drawlist whenever their is an update.
 * ========================================================================================================
 */
void Client::connect_to_server(const std::string& ip_address, const int port) {
    struct sockaddr_in serverAddr;

    // ----------------------------------------- Connection Creation
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip_address.c_str(), &serverAddr.sin_addr);
    

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        print_message("Error connecting to server");    exit(EXIT_FAILURE);
    }

    

    // ----------------------------------------- Sending Username & Receiving ID
    send_message(username, clientSocket);
    user_id = std::stoi(recieve_message(clientSocket));

    
    print_opening_message(ip_address, port);
    // ----------------------------------------- Print Info
    print_message("User ID Got [" + std::to_string(user_id) + "]");
    print_message("Connected to server as [" + username + "]. Type 'exit' to quit");



    
    // ----------------------------------------- Launch Server thread
    pthread_t server_loop_thread_id;
    pthread_create(&server_loop_thread_id, NULL, server_loop, this);
    
    
    // ----------------------------------------- Launch Render Thread
    pthread_t render_thread_id;
    if(test_flag == false){
        pthread_create(&render_thread_id, NULL, render_loop, this);
    }else{
        pthread_create(&render_thread_id, NULL, render_test_loop, this);
    }

    // ----------------------------------------- Loop
    client_loop();
}

