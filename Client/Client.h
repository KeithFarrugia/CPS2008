#ifndef CLIENT_H
#define CLIENT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/poll.h>
#include <vector>
#include <thread>
#include <queue>

#include "../Common/DrawList.h"
#include "../Common/Serialize/Serialize.pb.h"
#include "Lexer/Types/Include_Types.h"
#include "Lexer/Lexer.h"

constexpr int BUFFER_SIZE       = 1024;
constexpr int POLL_TIMEOUT      = 10;
constexpr int RULER_INTERVAL    = 50;
constexpr int RULER_SIZE        = 10;

class Client {
private:
    // ---------------------------------------------- Non-Static Variables
    std::string         username;
    long                select_id;
    Lexer               lexer;
    token_t             current_tool;
    PB_Colour           current_colour;
    int                 screen_width        = 800;
    int                 screen_height       = 600;

    // ---------------------------------------------- Static Variables
    static int          clientSocket;
    static bool         test_flag;
    static bool         kill_flag;
    static bool         is_server_thread_dead;
    static bool         is_render_thread_dead;
    static bool         should_render;
    static long         user_id;
    static DrawList     draw_list;
    static std::mutex   draw_mutex;
    static std::mutex   message_queue_mutex;
    static token_t      current_show;

    static std::queue<std::string> message_queue;

public:

    Client(const std::string& ip_address, const std::string& username, bool test_flag);

    ~Client();
    
    // ---------------------------------------------- Global Variables
    void            connect_to_server   (const std::string& ip_address, const int port);

private:

    // ---------------------------------------------- Threads & Loops
    static void*            render_loop             (void*);
    static void*            render_test_loop        (void*);
    static void*            server_loop             (void*);
    void                    client_loop             ();
    
    // ---------------------------------------------- Genral Functions
    void                    parse_input             (const std::string  input);
    static  void            print_opening_message   (std::string ip_address, int port);
    static  void            print_message           (const std::string message);
    static  void            print_message           ();
    static  void            send_message            (const std::string message, int clientSocket);
    static  std::string     recieve_message         (int clientSocket);
    static  void            print_comands           ();


    // ---------------------------------------------- Command Handlers
    void handle_help        (const std::vector<tokenised_t>& token_list);
    void handle_tool        (const std::vector<tokenised_t>& token_list);
    void handle_colour      (const std::vector<tokenised_t>& token_list);
    void handle_draw        (const std::vector<tokenised_t>& token_list);
    void handle_list        (const std::vector<tokenised_t>& token_list);
    void handle_select      (const std::vector<tokenised_t>& token_list);
    void handle_delete      (const std::vector<tokenised_t>& token_list);
    void handle_undo        (const std::vector<tokenised_t>& token_list);
    void handle_show        (const std::vector<tokenised_t>& token_list);
    void handle_clear       (const std::vector<tokenised_t>& token_list);
    void handle_exit        (const std::vector<tokenised_t>& token_list);

    static void execute_command     (PB_Package& package);

    // ---------------------------------------------- Create Functions
    PB_DrawData create_draw_item    (const std::vector<tokenised_t>& token_list);
    PB_DrawData create_circle       (int x, int y, int          radius, bool   filled               );
    PB_DrawData create_line         (int x, int y, int          x_to  , int    y_to                 );
    PB_DrawData create_rectangle    (int x, int y, int          width , int    height, bool filled  );
    PB_DrawData create_text         (int x, int y, std::string  text                                );
    PB_DrawData create_empty        ();


    // ---------------------------------------------- SDL Functions
    static void poll_sdl            (   SDL_Window*     gWindow,        int&        width,          int&    height, 
                                        int&            last_mouse_x,   int&        last_mouse_y,   int&    offset_x,  
                                        int&            offset_y,       bool&       is_dragging
                                    );

    static void generate_and_render (   SDL_Renderer*   renderer,       TTF_Font*   ui_font,        int&    offset_x,       
                                        int&            offset_y,       int&        width,          int&    height);

    static void draw_rulers         (   SDL_Renderer*   renderer,       TTF_Font*   ui_font,        int     offset_x, 
                                        int             offset_y,       int         screen_width,   int     screen_height);
    
};



#endif // CLIENT_H