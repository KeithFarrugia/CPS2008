#include "DrawList.h"

#define S_NUM_COL 3
#define L_NUM_COL 5

/* =========================================================================================================================
 * ------------------------------ Type Definitions
 * These are the types used mostly relating to the iterators
 * =========================================================================================================================
 */
typedef std::map<int64_t, PB_DrawData>::iterator map_iterator_t;

/* =========================================================================================================================
 * ------------------------------ Constructor and Destructor
 * =========================================================================================================================
 */
DrawList::DrawList() {}

DrawList::~DrawList() {}

/* =========================================================================================================================
 * ------------------------------ Initialise and De-Initialise
 * This is to set the value relating to the 
 * SDL Font as well as deleting it
 * =========================================================================================================================
 */
void DrawList::init_draw_list(const std::string& font_path, int font_size) {
    font = TTF_OpenFont(font_path.c_str(), font_size);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        exit(1);
    }
}

void DrawList::de_init_draw_list() {
    if (font) {
        TTF_CloseFont(font);
    }
}

/* =========================================================================================================================
 * ------------------------------ Insert PB_DrawData
 * Inserts a new PB_DrawData into the data_map.
 * Parameters:
 * - data: The PB_DrawData object to be inserted.
 * =========================================================================================================================
 */
void DrawList::insert(const PB_DrawData& data) {
    access_mutex.lock();
    data_map[data.draw_id()] = data;
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Erase PB_DrawData
 * Deletes a PB_DrawData entry from the data_map based on the given draw_id.
 * Parameters:
 * - id: The draw_id of the PB_DrawData object to be erased.
 * =========================================================================================================================
 */
void DrawList::erase(int64_t id) {
    access_mutex.lock();
    data_map.erase(id);
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Edit PB_DrawData
 * Edits an existing PB_DrawData entry in the data_map.
 * Keeps the creation time and draw_id the same.
 * Parameters:
 * - id: The draw_id of the PB_DrawData object to be edited.
 * - new_data: The new PB_DrawData object with updated values.
 * =========================================================================================================================
 */
void DrawList::edit(int64_t id, const PB_DrawData& new_data) {
    access_mutex.lock();
    map_iterator_t it = data_map.find(id);
    if (it != data_map.end()) {
        int64_t original_draw_id = it->second.draw_id();
        it->second.CopyFrom(new_data);
        it->second.set_draw_id(original_draw_id);
    }
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Traverse PB_DrawData
 * Traverses all PB_DrawData entries in sorted order of their creation time and applies a given function to each entry.
 * Parameters:
 * - func: A function that takes a PB_DrawData reference and performs an action on it.
 * =========================================================================================================================
 */
void DrawList::traverse(void func(const PB_DrawData&)){
    access_mutex.lock();
    for (const auto& pair : data_map) {
        func(pair.second);
    }
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Print User Items
 * Prints all PB_DrawData entries related to a specific user or all entries if user_id is -1.
 * Parameters:
 * - user_id: The user_id of the PB_DrawData objects to be printed. Prints all entries if user_id is -1.
 * =========================================================================================================================
 */
void DrawList::list(token_t type, int64_t user_id){
    const std::string CYAN = "\033[36m";
    const std::string GREEN = "\033[32m";
    const std::string RESET = "\033[0m";

    access_mutex.lock();
    std::cout << CYAN << "-------------------------------------------------------List--------------------------------------------------------\n";
    for (const auto& pair : data_map) {
        const PB_DrawData& data = pair.second;

        if (user_id == -1 || data.user_id() == user_id) {
            bool display = false;
            switch (type) {
                case LINE           :display = data.has_line();         break;
                case RECTANGLE      :display = data.has_rectangle();    break;
                case CIRCLE         :display = data.has_circle();       break;
                case TEXT           :display = data.has_text();         break;
                case ALL            :display = true;                    break;
                default             :display = false;                   break;
            }

            if (display) {
                display_draw_item(data);
            } 
        }
    }
    std::cout 
        << GREEN << "> " << CYAN << "-------------------------------------------------------------------------------------------------------------------\n" 
        << GREEN << "> " << RESET << std::flush;
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Clear PB_DrawData
 * Deletes all PB_DrawData entries or all entries related to a specific user.
 * Parameters:
 * - user_id: The user_id of the PB_DrawData objects to be deleted. Deletes all entries if user_id is -1.
 * =========================================================================================================================
 */
void DrawList::clear(int64_t user_id) {
    access_mutex.lock();
    if (user_id == -1) {
        data_map.clear();
    } else {
        for (auto it = data_map.begin(); it != data_map.end(); ) {
            if (it->second.user_id() == user_id) {
                it = data_map.erase(it);
            } else {
                ++it;
            }
        }
    }
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Delete Last Entry
 * Deletes the last PB_DrawData entry related to a specific user.
 * Parameters:
 * - user_id: The user_id of the PB_DrawData object to be deleted.
 * =========================================================================================================================
 */
void DrawList::undo(int64_t user_id) {
    access_mutex.lock();
    for (auto it = data_map.rbegin(); it != data_map.rend(); ++it) {
        if (it->second.user_id() == user_id) {
            data_map.erase(std::next(it).base());
            break;
        }
    }
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Serialize
 * Serializes the data by converting a Bulk draw data object which
 * is an array holding all the drawobjects contained in the map,
 * =========================================================================================================================
 */
PB_BulkDrawData DrawList::serialize(){
    access_mutex.lock();
    PB_BulkDrawData bulk_data;
    for (const auto& pair : data_map) {
        PB_DrawData* new_data = bulk_data.add_draw_data();
        *new_data = pair.second;
    }
    access_mutex.unlock();
    return bulk_data;
}

/* =========================================================================================================================
 * ------------------------------ Load 
 * This takes a bulk data object,
 * clears the current datastructure
 * and fills it with the bulk data.
 * =========================================================================================================================
 */
void DrawList::load(const PB_BulkDrawData& bulk_data) {
    access_mutex.lock();
    // Clear the current data
    data_map.clear();

    // Populate the draw items from the bulk data
    for (const auto& data : bulk_data.draw_data()) {
        data_map[data.draw_id()] = data;
    }
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Add to Renderer
 * Adds all PB_DrawData objects to an SDL_Renderer.
 * Parameters:
 * - renderer: The SDL_Renderer to which the draw objects should be added.
 * =========================================================================================================================
 */
void DrawList::add_to_renderer(SDL_Renderer* renderer, int offset_x, int offset_y, int width, int height, int user_id){
    access_mutex.lock();
    if (user_id == -1) {
        for (const auto& pair : data_map) {
            render_draw_item(renderer, pair.second, offset_x, offset_y, width, height);
        }
    } else {
        for (const auto& pair : data_map) {
            if (pair.second.user_id() == user_id) {
                render_draw_item(renderer, pair.second, offset_x, offset_y, width, height);
            }
        }
    }
    access_mutex.unlock();
}

/* =========================================================================================================================
 * ------------------------------ Generate Hash
 * Generates a SHA-256 hash value that uniquely describes the set of PB_DrawData instances in the DrawList.
 * Returns:
 * - A string representing the SHA-256 hash value.
 * =========================================================================================================================
 */
std::string DrawList::generate_hash(){
    access_mutex.lock();
    std::stringstream ss;

    for (const auto& pair : data_map) {
        std::string serialized_data;
        pair.second.SerializeToString(&serialized_data);
        ss << serialized_data;
    }

    std::string concatenated_data = ss.str();

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)concatenated_data.c_str(), concatenated_data.size(), hash);

    std::stringstream hash_ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        hash_ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    access_mutex.unlock();
    return hash_ss.str();
}

/* =========================================================================================================================
 * ------------------------------ Render Draw Item
 * Renders a single PB_DrawData object to the given SDL_Renderer.
 * Parameters:
 * - renderer: The SDL_Renderer to which the draw object should be rendered.
 * - data: The PB_DrawData object to be rendered.
 * =========================================================================================================================
 */
void DrawList::render_draw_item(SDL_Renderer* renderer, const PB_DrawData& data, int offset_x, int offset_y, int width, int height) const {
    int center_x = width / 2;
    int center_y = height / 2;

    // ---------------------------------------------- CIRCLE
    if (data.has_circle()) {
        const PB_DrawCircle& circle = data.circle();

        SDL_SetRenderDrawColor(
            renderer,
            circle.colour().r(),
            circle.colour().g(),
            circle.colour().b(),
            circle.colour().a()
        );
        if(circle.filled()){
            filledCircleRGBA(
                renderer,
                center_x + circle.x() - offset_x,
                center_y - circle.y() - offset_y,
                circle.radius(),
                circle.colour().r(),
                circle.colour().g(),
                circle.colour().b(),
                circle.colour().a()
            );
        }else{
            circleRGBA(
                renderer,
                center_x + circle.x() - offset_x,
                center_y - circle.y() - offset_y,
                circle.radius(),
                circle.colour().r(),
                circle.colour().g(),
                circle.colour().b(),
                circle.colour().a()
            );
        }
        

    // ---------------------------------------------- RECTANGLE
    } else if (data.has_rectangle()) {
        const PB_DrawRectangle& rectangle = data.rectangle();

        SDL_SetRenderDrawColor(
            renderer,
            rectangle.colour().r(),
            rectangle.colour().g(),
            rectangle.colour().b(),
            rectangle.colour().a()
        );

        SDL_Rect rect = {
            center_x + rectangle.x() - offset_x,
            center_y - rectangle.y() - offset_y - rectangle.height(),
            rectangle.width(),
            rectangle.height()
        };

        if (rectangle.filled()) {
            SDL_RenderFillRect(renderer, &rect);
        } else {
            SDL_RenderDrawRect(renderer, &rect);
        }

    // ---------------------------------------------- LINE
    } else if (data.has_line()) {
        const PB_DrawLine& line = data.line();

        SDL_SetRenderDrawColor(
            renderer,
            line.colour().r(),
            line.colour().g(),
            line.colour().b(),
            line.colour().a()
        );

        SDL_RenderDrawLine(
            renderer,
            center_x + line.x() - offset_x,
            center_y - line.y() - offset_y,
            center_x + line.x_to() - offset_x,
            center_y - line.y_to() - offset_y
        );

    // ---------------------------------------------- TEXT
    } else if (data.has_text()) {
        const PB_DrawText& text = data.text();

        SDL_SetRenderDrawColor(
            renderer,
            text.colour().r(),
            text.colour().g(),
            text.colour().b(),
            text.colour().a()
        );

        SDL_Color color = {
            (u_char)text.colour().r(),
            (u_char)text.colour().g(),
            (u_char)text.colour().b(),
            (u_char)text.colour().a()
        };

        SDL_Surface* surface = TTF_RenderText_Blended(font, text.text().c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dstrect = {
            center_x + text.x() - offset_x,
            center_y - text.y() - offset_y,
            surface->w,
            surface->h
        };

        SDL_RenderCopy(renderer, texture, NULL, &dstrect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}

/* =========================================================================================================================
 * ------------------------------ Display Draw Item
 * Displays a PB_DrawData entry in a formatted manner, showing its details such as user_id, draw_id, and shape-specific data.
 * Parameters:
 * - data: The PB_DrawData object to be displayed.
 * =========================================================================================================================
 */
void DrawList::display_draw_item(const PB_DrawData& data) const {
    std::cout << "> " <<"\033[32m"<< "\033[36m" << "user_id [ " << std::setw(L_NUM_COL)     << std::right << data.user_id();
    std::cout << "] draw_id [ " << std::setw(L_NUM_COL)     << std::right << data.draw_id();
    std::cout << "] ";
    
    if (data.has_circle()) {
        const PB_DrawCircle& circle = data.circle();
        std::cout << "colour: ["    << std::setw(S_NUM_COL) << std::right << circle.colour().r();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << circle.colour().g();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << circle.colour().b();
        std::cout << "] ";
        
        std::cout << "Circle {";
        std::cout << "x  = "        << std::setw(L_NUM_COL) << std::right << circle.x();
        std::cout << ", y  = "      << std::setw(L_NUM_COL) << std::right << circle.y();
        std::cout << ", r  = "      << std::setw(L_NUM_COL) << std::right << circle.x();
        std::cout << ", filled = "  << std::setw(1)         << std::right << (circle.filled()?"T":"F");
                
    } else if (data.has_rectangle()) {
        const PB_DrawRectangle& rectangle = data.rectangle();
        std::cout << "colour: ["    << std::setw(S_NUM_COL) << std::right << rectangle.colour().r();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << rectangle.colour().g();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << rectangle.colour().b();
        std::cout << "] ";

        std::cout << "Rec    {";
        std::cout << "x  = "        << std::setw(L_NUM_COL) << std::right << rectangle.x();
        std::cout << ", y  = "      << std::setw(L_NUM_COL) << std::right << rectangle.y();
        std::cout << ", w  = "      << std::setw(L_NUM_COL) << std::right << rectangle.width();
        std::cout << ", h  = "      << std::setw(L_NUM_COL) << std::right << rectangle.height();
        std::cout << ", filled = "  << std::setw(1)         << std::right << (rectangle.filled()?"T":"F");
        
    } else if (data.has_line()) {
        const PB_DrawLine& line = data.line();
        std::cout << "colour: ["    << std::setw(S_NUM_COL) << std::right << line.colour().r();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << line.colour().g();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << line.colour().b();
        std::cout << "] ";

        std::cout << "Line   {";
        std::cout << "x1 = "        << std::setw(L_NUM_COL) << std::right << line.x();
        std::cout << ", y1 = "      << std::setw(L_NUM_COL) << std::right << line.y();
        std::cout << ", x2 = "      << std::setw(L_NUM_COL) << std::right << line.x_to();
        std::cout << ", y2 = "      << std::setw(L_NUM_COL) << std::right << line.y_to();
        
    } else if (data.has_text()) {
        const PB_DrawText& text = data.text();
        std::cout << "colour: ["    << std::setw(S_NUM_COL) << std::right << text.colour().r();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << text.colour().g();
        std::cout << ", "           << std::setw(S_NUM_COL) << std::right << text.colour().b();
        std::cout << "] ";

        std::cout << "Text   {";
        std::cout << "x  = "        << std::setw(L_NUM_COL) << std::right << text.x();
        std::cout << ", y  = "      << std::setw(L_NUM_COL) << std::right << text.y();
        std::cout << ", t  = "      << std::setw(L_NUM_COL) << std::right << text.text();
        
    } else {
        std::cout << "No-tool {";
    }
    std::cout << "}" << std::endl << std::flush;
}
