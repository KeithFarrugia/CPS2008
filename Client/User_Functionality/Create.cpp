#include "../Client.h"

/* ========================================================================================================
 *                                           Create a Draw Item                                           
 * Creates and returns a draw item
 * the general process is the following
 * - validate the number of parameters
 * - validate the types of the parameters
 * - create & return the object
 * 
 * in the case of failure it returns an 
 * empty object (user_id = -1)
 * ========================================================================================================
 */
PB_DrawData Client::create_draw_item(const std::vector<tokenised_t>& token_list) {
    bool fill = false;

    // ----------------------- Check Current tool
    switch(current_tool) {
        // ============================================================================== Circle
        case token_t::CIRCLE:
            // -------------------------------- Check correct parameter amount
            if(token_list.size() < 4 || token_list.size() > 5) {
                print_message("Command expects 3-4 parameters [draw x y radius fill?]");
                return create_empty();
            }
            
            // -------------------------------- Check correct parameter type
            if(token_list[1].token_type != token_t::INTEGER ||
               token_list[2].token_type != token_t::INTEGER ||
               token_list[3].token_type != token_t::INTEGER) {
                print_message("Command expects 3 Integers");
                return create_empty();
            }

            // -------------------------------- Check fill
            if(token_list.size() == 5) {
                if(token_list[4].token_type != token_t::FILL){
                    print_message("Command expects that the 5th parameter is 'fill'");
                    return create_empty();
                }
                fill = true;
            }

            // -------------------------------- Create
            return create_circle(
                std::stoi(token_list[1].lexeme_string), 
                std::stoi(token_list[2].lexeme_string), 
                std::stoi(token_list[3].lexeme_string), 
                fill);

        // ============================================================================== Rectangle
        case token_t::RECTANGLE:
            // -------------------------------- Check correct parameter amount
            if(token_list.size() < 5 || token_list.size() > 6) {
                print_message("Command expects 4-5 parameters [draw x y width height fill?]");
                return create_empty();
            }

            // -------------------------------- Check correct parameter type
            if(token_list[1].token_type != token_t::INTEGER ||
               token_list[2].token_type != token_t::INTEGER ||
               token_list[3].token_type != token_t::INTEGER ||
               token_list[4].token_type != token_t::INTEGER) {
                print_message("Command expects 4 Integers");
                return create_empty();
            }

            // -------------------------------- Check fill
            if(token_list.size() == 6) {
                if(token_list[5].token_type != token_t::FILL){
                    print_message("Command expects that the 5th parameter is 'fill'");
                    return create_empty();
                }
                fill = true;
            }
            
            // -------------------------------- Create
            return create_rectangle(
                std::stoi(token_list[1].lexeme_string), 
                std::stoi(token_list[2].lexeme_string),  
                std::stoi(token_list[3].lexeme_string), 
                std::stoi(token_list[4].lexeme_string), 
                fill);
            break;

        // ============================================================================== Line
        case token_t::LINE:
            // -------------------------------- Check correct parameter amount
            if(token_list.size() != 5) {
                print_message("Command expects 4 parameters [draw x1 y1 x2 y2]");
                return create_empty();
            }

            int x1, y1, x2, y2;

            // -------------------------------- Check correct parameter type
            if(token_list[1].token_type != token_t::INTEGER ||
               token_list[2].token_type != token_t::INTEGER ||
               token_list[3].token_type != token_t::INTEGER ||
               token_list[4].token_type != token_t::INTEGER) {
                print_message("Command expects 4 Integers");
                return create_empty();
            }

            // -------------------------------- Create
            return create_line(
                std::stoi(token_list[1].lexeme_string),
                std::stoi(token_list[2].lexeme_string),
                std::stoi(token_list[3].lexeme_string),
                std::stoi(token_list[4].lexeme_string));
            break;

        // ============================================================================== Text
        case token_t::TEXT:
            // -------------------------------- Check correct parameter amount
            if(token_list.size() != 4) {
                print_message("Command expects 3 parameters [draw x y text]");
                return create_empty();
            }

            // -------------------------------- Check correct parameter type
            if(token_list[1].token_type != token_t::INTEGER ||
               token_list[2].token_type != token_t::INTEGER ||
               token_list[3].token_type != token_t::STRING) {
                print_message("Command expects 2 Integers and 1 String");
                return create_empty();
            }

            // -------------------------------- Create
            return create_text(
                std::stoi(token_list[1].lexeme_string), 
                std::stoi(token_list[2].lexeme_string), 
                token_list[3].lexeme_string);
            break;

        default:
            print_message("No Draw Tool was chosen");
            return create_empty();
    }
}






/* ========================================================================================================
 *                                             Create a Circle  
 * Creates a PB_DrawData item holding a circle where the given x and y will dictate the center
 * the radius parameter dictates the radius
 * and the filled parameter will dictate whether its an outline or solid          
 * ========================================================================================================
 */
PB_DrawData Client::create_circle(int x, int y, int radius, bool filled) {
    PB_DrawCircle circle;
    circle.set_x(x);
    circle.set_y(y);
    circle.set_radius(radius);
    circle.set_filled(filled);
    circle.mutable_colour()->CopyFrom(current_colour);

    PB_DrawData draw_data;
    draw_data.set_user_id(user_id);
    draw_data.mutable_circle()->CopyFrom(circle);

    return draw_data;
}

/* ========================================================================================================
 *                                            Create a Rectangle                                            
 * ========================================================================================================
 */
PB_DrawData Client::create_rectangle(int x, int y, int width, int height, bool filled) {
    PB_DrawRectangle rectangle;
    rectangle.set_x(x);
    rectangle.set_y(y);
    rectangle.set_width(width);
    rectangle.set_height(height);
    rectangle.set_filled(filled);
    rectangle.mutable_colour()->CopyFrom(current_colour);

    PB_DrawData draw_data;
    draw_data.set_user_id(user_id);
    draw_data.mutable_rectangle()->CopyFrom(rectangle);

    return draw_data;
}

/* ========================================================================================================
 *                                              Create a Line                                              
 * Creates a PB_DrawData item holding a Line object that will be between the 2 given points
 * ========================================================================================================
 */
PB_DrawData Client::create_line(int x, int y, int x_to, int y_to) {
    PB_DrawLine line;
    line.set_x(x);
    line.set_y(y);
    line.set_x_to(x_to);
    line.set_y_to(y_to);
    line.mutable_colour()->CopyFrom(current_colour);

    PB_DrawData draw_data;
    draw_data.set_user_id(user_id);
    draw_data.mutable_line()->CopyFrom(line);

    return draw_data;
}

/* ========================================================================================================
 *                                            Create a Text Box                                            
 * Creates a PB_DrawData item holding a Text object
 * x and y will dictate the starting position and the text will dictate the text
 * ========================================================================================================
 */
PB_DrawData Client::create_text(int x, int y, std::string text) {
    PB_DrawText text_item;
    text_item.set_x(x);
    text_item.set_y(y);
    text_item.set_text(text);
    text_item.mutable_colour()->CopyFrom(current_colour);

    PB_DrawData draw_data;
    draw_data.set_user_id(user_id);
    draw_data.mutable_text()->CopyFrom(text_item);

    return draw_data;
}

/* ========================================================================================================
 *                                             Create Nothing                                              
 * Creates a PB_DrawData item holding nothing with the user id set to -1
 * ========================================================================================================
 */
PB_DrawData Client::create_empty(){
    PB_DrawData draw_data;
    draw_data.set_user_id(-1);
    return draw_data;
}