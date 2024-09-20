#include "../Client.h"
/* ========================================================================================================
 *                                                Poll SDL                                                 
 * From my understanding SDL has a buffer in order to hold the events.
 * This means that all events are first stored into the buffer until each is
 * handled independently, In other words the envents themselves aren't set up
 * to be handled like interrupts but more so stored in a stack.
 * 
 * There for this function will loop through the buffer handling each even case
 * until the buffer is empty.
 * - In the case of pressing the close button on the window, the kill flag is set
 * - In the case that the mouse left clicked was pressed then the dragging variable is set
 * - In the case the mouse click was let go the dragging variable is updated
 * - IF the mouse was moved while the left click is pressed then the new position is calculated
 * - If the Window was re-sized then the new size is stored
 * ========================================================================================================
 */
void Client::poll_sdl( 
            SDL_Window*     wb_window,      int&    width,          int&    height,
            int&            last_mouse_x,   int&    last_mouse_y,   int&    offset_x, 
            int&            offset_y,       bool&   is_dragging ){

    SDL_Event e;

    // ----------------------------- Loop To Empty Event Buffer
    while (SDL_PollEvent(&e) != 0) {

        // ----------------------------- Close Buton was pressed
        if (e.type == SDL_QUIT) {
            kill_flag = true;

        // ----------------------------- Left click is pressed
        }else if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                is_dragging = true;
                SDL_GetMouseState(&last_mouse_x, &last_mouse_y);
            }
        
        // ----------------------------- Left click is released
        }else if (e.type == SDL_MOUSEBUTTONUP) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                is_dragging = false;
            }

        // ----------------------------- Mouse was moved
        }else if (e.type == SDL_MOUSEMOTION) {
            if (is_dragging) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                offset_x -= mouseX - last_mouse_x;
                offset_y -= mouseY - last_mouse_y;
                last_mouse_x = mouseX;
                last_mouse_y = mouseY;
                should_render = true;
            }
        
        // ----------------------------- Window was re-sized
        }else if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                SDL_GetWindowSize(wb_window, &width, &height);
                should_render = true;
            }
        }
    }
}


/* ========================================================================================================
 *                                           Generate and Render                                           
 * This function takes care od redrawing the new frame/window
 * Here the draw list is traversed and each object is rendered,
 * The rulers are also drawn on top
 * ========================================================================================================
 */
void Client::generate_and_render(
        SDL_Renderer*  renderer,   TTF_Font*   ui_font,
        int&            offset_x,   int&        offset_y, 
        int&            width,      int&        height      ){
    
    // ----------------------------- Set Background Colour
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
    SDL_RenderClear(renderer);
    
    
    // ----------------------------- Render the  Draw Objects -> Check Show
    if(current_show == token_t::ALL){   
        draw_list.add_to_renderer(renderer, offset_x, offset_y, width, height, -1);
    }else{
        draw_list.add_to_renderer(renderer, offset_x, offset_y, width, height, user_id);
    }

    // ----------------------------- Render the Rulers
    draw_rulers(renderer, ui_font, offset_x, offset_y, width, height);

    // ----------------------------- Display to window
    SDL_RenderPresent(renderer);
}



/* ========================================================================================================
 *                                           Draw Rulers                                           
 * This Function generates the rulers show at the bottom and left columns
 * each ruller is split into two with the positive markers (50, 100, 150, ...)
 * and the negative markers (0, -50, -100, -150, ....) are drawn speratly.
 * The Intervals are set through the global variable
 * 
 * it first assigns two vectors with the desired location and text for each marker
 * The using the vectors the actuall text and line is generated into the renderer.
 * ========================================================================================================
 */
void Client::draw_rulers(
        SDL_Renderer*   renderer,   TTF_Font*   ui_font,        int     offset_x, 
        int             offset_y,   int         screen_width,   int     screen_height) {


    // ----------------------------- 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    int centerX = screen_width / 2 - offset_x;
    int centerY = screen_height / 2 - offset_y;

    // ============================ Vector Creation ============================

    // ===================================================== Horizontal Ruler
    std::vector<std::pair<int, std::string>> horizontalMarks;
    
    // ----------------------------- Negative Markers
    for (int x = centerX - RULER_INTERVAL; x > -screen_width; x -= RULER_INTERVAL) {
        std::stringstream ss;
        ss << std::to_string(x - centerX);
        horizontalMarks.emplace_back(x, ss.str());
    }
    // ----------------------------- Positive Markers
    for (int x = centerX; x < screen_width; x += RULER_INTERVAL) {
        std::stringstream ss;
        ss << std::to_string(x - centerX);
        horizontalMarks.emplace_back(x, ss.str());
    }

    // ===================================================== Vertical Ruler
    std::vector<std::pair<int, std::string>> verticalMarks;
    
    // ----------------------------- Negative Markers
    for (int y = centerY - RULER_INTERVAL; y > 0; y -= RULER_INTERVAL) {
        std::stringstream ss;
        ss << std::setw(5) << std::right << (centerY - y);
        verticalMarks.emplace_back(y, ss.str());
    }

    // ----------------------------- Positive Markers
    for (int y = centerY; y < screen_height; y += RULER_INTERVAL) {
        std::stringstream ss;
        ss << std::setw(5) << std::right << (centerY - y);
        verticalMarks.emplace_back(y, ss.str());
    }
    
    // ============================ Rendering ============================

    // ===================================================== Horizontal Ruler
    for (const auto& mark : horizontalMarks) {
        // ----------------------------- Line
        SDL_RenderDrawLine(renderer, mark.first, screen_height, mark.first, screen_height - RULER_SIZE);
        
        // ----------------------------- Text
        SDL_Surface* surface = TTF_RenderText_Blended(ui_font, mark.second.c_str(), { 0, 0, 0, 255 });
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = { mark.first - surface->w / 2, screen_height - (RULER_SIZE * 3), surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, nullptr, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }

    // ===================================================== Vertical Ruler
    for (const auto& mark : verticalMarks) {
        // ----------------------------- Line
        SDL_RenderDrawLine(renderer, 0, mark.first, RULER_SIZE, mark.first);
        
        // ----------------------------- Text
        SDL_Surface* surface = TTF_RenderText_Blended(ui_font, mark.second.c_str(), { 0, 0, 0, 255 });
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect textRect = { RULER_SIZE + 2, mark.first - (surface->h + 3) / 2, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture, nullptr, &textRect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
}
