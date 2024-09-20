#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <queue>
#include <mutex>
#include <atomic>
#include <iomanip>
#include <sys/poll.h>
#include <chrono>

#include "../Common/Serialize/Serialize.pb.h"
#include "../Common/DrawList.h"

constexpr int   BUFFER_SIZE             = 1024;
constexpr int   MAX_CLIENTS             = 100;
constexpr int   POLL_TIMEOUT            = 50;
constexpr int   RECONNECT_TIME          = 60;
constexpr int   MUTEX_TIMEOUT           = 10;
const int       INACTIVITY_TIME_Limit   = 600;

// ---------------------------------------------- Connection Struct
typedef struct{
    int             socket      ;
    int             user_ID     ;
    bool            is_active   ;
    bool            kill_flag   ;
    std::string     username    ;
}connection_t;

// ---------------------------------------------- Status Enum
typedef enum {
    on,
    off,
    success,
    failure,
    error,
}status_t;

typedef std::unordered_map<std::string, connection_t*>::iterator con_map_iter;
typedef std::chrono::steady_clock::time_point time_point_t;
typedef std::chrono::_V2::steady_clock::time_point time_point_v2_t;
class Server {
public:
    Server();
    ~Server();

    void start(int port);
private:
    // ---------------------------------------------- Threads
    void                server_loop             ();
    static void*        client_handler          (void* clientSocket_ptr);


    // ---------------------------------------------- Messaging Functions
    static void         broadcast_message       (const std::string      message                             );  
    static void         send_message            (const std::string      message,    const int clientSocket  );
    static std::string  receive_message         (const int              clientSocket                        );

    // ---------------------------------------------- Utility Funcitons
    static status_t     check_kill_flag         (const std::string      username                            );
    static status_t     disconnect_user         (const std::string      username                            );
    static status_t     remove_user             (const std::string      username                            );
    static status_t     re_connect_user         (const std::string      username,   const int new_socket    );
    static status_t     create_new_user         (      connection_t&    connection                          );
    static status_t     handle_reconnect        (      connection_t&    connection                          );

    static void         print_message           (std::string message);
    static void         print_connection         (const connection_t& c);
    static void         execute_and_broadcast   (std::string& username, int clientSocket, std::queue<PB_Package>& messages_buf);
    static status_t     check_client            (connection_t& connection, int& clientSocket);
    static bool         is_username_in_use      (std::string& username);
    static bool         is_user_active          (std::string& username);
    static void         check                   (int x, std::string message);
    static std::string  summarise_package       (const PB_Package& package);

    // ---------------------------------------------- Variables
    int                                 serverSocket;
    static int                          user_id_rec;
    static bool                         server_kill_flag;
    static int64_t                      glb_draw_id;
    static DrawList                     draw_list;
    static std::mutex                   connections_mutex;
    static std::mutex                   draw_mutex;
    static std::timed_mutex             draw_list_mutex;
    static std::vector<std::string>     message_history;
    static std::vector<int>             clients;
    static std::vector<pthread_t>       thread_ids;

    // ---------------------------------------------- Connection List
    static std::unordered_map<std::string, connection_t*> connections;
};

#endif //SERVER_H