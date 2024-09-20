#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include "../Client/Lexer/Types/Tokens.h"
#include <iostream>
#include <map>
#include <string>
#include <iomanip>
#include "Serialize/Serialize.pb.h"
#include <SDL2/SDL.h>
#include <openssl/sha.h>
#include <set>
#include <mutex>

class DrawList {
public:
    // Insert a new PB_DrawData
    DrawList();
    ~DrawList();
    void                init_draw_list      (const std::string& font_path, int font_size);
    void                de_init_draw_list   ();
    void                edit                ( int64_t id, const PB_DrawData& new_data);
    void                clear               ( int64_t user_id);
    void                erase               ( int64_t id);
    void                insert              ( const  PB_DrawData& data);
    void                traverse            ( void func(const PB_DrawData&));
    void                list                ( token_t type, int64_t user_id);
    void                undo                ( int64_t user_id);
    std::string         generate_hash       ();
    PB_BulkDrawData     serialize           ();
    void                load                (const PB_BulkDrawData& bulk_data);
    void                add_to_renderer     (SDL_Renderer* renderer, int offset_x, int offset_y, int width, int height, int user_id);

private:
    std::map<int64_t, PB_DrawData>              data_map;
    
    TTF_Font* font;
    std::mutex access_mutex;
    void display_draw_item(const PB_DrawData& data) const;
    void render_draw_item(SDL_Renderer* renderer, const PB_DrawData& data, int offset_x, int offset_y, int width, int height) const;
};

#endif // DATA_STRUCTURE_H