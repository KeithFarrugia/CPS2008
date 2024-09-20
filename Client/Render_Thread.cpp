#include "Client.h"
/* ========================================================================================================
 *                                             Render Test Loop                                            
 * This is a simpler render loop as it does not launch the window but only executes the recieved commands
 * ========================================================================================================
 */
void* Client::render_test_loop(void*) {
    std::string     message;
    PB_Package      package;

    while (!kill_flag) {

        while(!message_queue.empty()){
            // ---------------------------------------------- Pop Queue
            message_queue_mutex.lock();
            message = message_queue.front();
            message_queue.pop();
            message_queue_mutex.unlock();
            // ---------------------------------------------- Decode & Execute
            package.ParseFromString(message);
            execute_command(package);
        }

        execute_command(package);
    }

    draw_list.de_init_draw_list();
    is_render_thread_dead = true;
    pthread_exit(0); return nullptr;
}

/* ========================================================================================================
 *                                               Render Loop                                               
 * This is the more complex loop, it will both execute the given commands as well as 
 * render the drawlist whenever their is an update.
 * ========================================================================================================
 */
void* Client::render_loop(void*) {
    // ---------------------------------------------- Set Flag
    is_render_thread_dead = false;

    // ---------------------------------------------- Genral Variables
    is_render_thread_dead = false;
    bool            is_dragging;
    std::string     message;
    PB_Package      package;

    // ---------------------------------------------- Window Variables
    int offset_x        = 0, 
        offset_y        = 0,
        last_mouse_x    = 0, 
        last_mouse_y    = 0,
        width           = 800, 
        height          = 600;
    
    // ---------------------------------------------- WSL Setup
    SDL_Window*     wb_window       = SDL_CreateWindow(
        "Whiteboard",
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        800, 600, SDL_WINDOW_SHOWN
    );

    SDL_Renderer*   wb_renderer     = SDL_CreateRenderer(
        wb_window,  -1, 
        SDL_RENDERER_ACCELERATED
    );

    TTF_Font*       ui_font         = TTF_OpenFont(
        "fonts/SpaceMono-Regular.ttf", 10
    );
    
    // ---------------------------------------------- Verify SDL
    if (!ui_font || !wb_window || !wb_renderer) {
        std::cerr << "Render Setup Failed" << TTF_GetError() << std::endl;
        kill_flag = true;}
    


    // ----------------------------------------------
    //                     LOOP                      
    // ----------------------------------------------
    while (!kill_flag) {
        
        while(!message_queue.empty()){
            // ---------------------------------------------- Pop Queue
            message_queue_mutex.lock();
            message = message_queue.front();
            message_queue.pop();
            message_queue_mutex.unlock();
            // ---------------------------------------------- Decode & Execute
            package.ParseFromString(message);
            execute_command(package);
        }

        // ---------------------------------------------- Poll SDL Window Events
        poll_sdl(
            wb_window,
            width,          height, 
            last_mouse_x,   last_mouse_y,
            offset_x,       offset_y, 
            is_dragging);

        // ---------------------------------------------- If needs to be rendered -> render
        if(should_render){
            generate_and_render( 
                wb_renderer,    ui_font, 
                offset_x,       offset_y, 
                width,          height
            );
        }
        should_render = false;
    }

    
    SDL_DestroyRenderer(wb_renderer);
    SDL_DestroyWindow(wb_window);

    if (ui_font) { TTF_CloseFont(ui_font); }
    draw_list.de_init_draw_list();
    TTF_Quit();
    wb_window = nullptr;
    wb_renderer = nullptr;
    is_render_thread_dead = true;
    pthread_exit(0); return nullptr;
}



