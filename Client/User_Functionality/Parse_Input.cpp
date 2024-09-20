#include "../Client.h"
/* ========================================================================================================
 *                                              Parse input                                              
 * Tokenises the string and calls the related handler
 * ========================================================================================================
 */
void Client::parse_input(const std::string input) {
    std::vector<tokenised_t> token_list;
    int index = 0;
    // -------------------------------- Tokenise Input
    try {
        token_list = lexer.GenerateTokens(input, false);
    } catch (const std::exception& e) {
        print_message("Error Invalid Input");
        print_message("Please make sure that input is in the correct format");
        print_message("\t- Strings must be enclosed in quotations:  \" ... \"");
        print_message("\t- Commands are written properly");
        print_message("\t- Only Integers are inputted");
        print_message("Type \"help\" for command list");
        return;
    }

    // -------------------------------- Check if not a token
    if (token_list.empty()) { return; }

    // -------------------------------- Call Handler
    switch (token_list[0].token_type) {
        case token_t::HELP      :handle_help    (token_list);   break;
        case token_t::TOOL      :handle_tool    (token_list);   break;
        case token_t::COLOUR    :handle_colour  (token_list);   break;
        case token_t::DRAW      :handle_draw    (token_list);   break;
        case token_t::LIST      :handle_list    (token_list);   break;
        case token_t::SELECT    :handle_select  (token_list);   break;
        case token_t::DELETE    :handle_delete  (token_list);   break;
        case token_t::UNDO      :handle_undo    (token_list);   break;
        case token_t::SHOW      :handle_show    (token_list);   break;
        case token_t::CLEAR     :handle_clear   (token_list);   break;

        case token_t::EXIT      :handle_exit    (token_list);   break;

        default:print_message("Unknown command"); break;
    }
}

/* ========================================================================================================
 *                                               Handle Help                                               
 * Handles the Help Command
 * - prints the command list
 * ========================================================================================================
 */
void Client::handle_help(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 1) {
        print_message("Error: Command expects no parameters [help]");
        return;
    }

    print_comands();
}

/* ========================================================================================================
 *                                               Handle Tool                                               
 * Handles the Tool Command
 * - updates the current tool
 * ========================================================================================================
 */
void Client::handle_tool(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 2) {
        print_message("Error: Command expects 1 parameters [tool <object>]");
        return;
    }

    if (token_list[1].token_type != token_t::LINE       &&
        token_list[1].token_type != token_t::RECTANGLE  &&
        token_list[1].token_type != token_t::CIRCLE     &&
        token_list[1].token_type != token_t::TEXT           ) {
        print_message("Error: Command expects either \"line\", \"rectangle\", \"circle\" or \"text\"");
        return;
    }

    current_tool = token_list[1].token_type;
}

/* ========================================================================================================
 *                                              Handle Colour                                              
 * Handles Colour Command
 * - updates the current colour
 * ========================================================================================================
 */
void Client::handle_colour(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 4) {
        print_message("Error: Command expects 3 parameters [colour r g b]");
        return;
    }

    if (token_list[1].token_type != token_t::INTEGER ||
        token_list[2].token_type != token_t::INTEGER ||
        token_list[3].token_type != token_t::INTEGER) {
        print_message("Error: Command expects 3 Integers");
        return;
    }

    current_colour.set_r((unsigned char)std::stoi(token_list[1].lexeme_string));
    current_colour.set_g((unsigned char)std::stoi(token_list[2].lexeme_string));
    current_colour.set_b((unsigned char)std::stoi(token_list[3].lexeme_string));
    current_colour.set_a(255);
}

/* ========================================================================================================
 *                                               Handle Draw                                               
 * Handles Draw Command
 * - Calls the Create Draw Item function
 * - Sends the item as a package to the server
 * ========================================================================================================
 */
void Client::handle_draw(const std::vector<tokenised_t>& token_list) {
    PB_DrawData draw_item = create_draw_item(token_list);
    PB_Package  package;
    PB_UpdateDrawData update;
    std::string serialized_package;

    if(select_id == -1){
        package.set_message_type(PB_MessageType::PB_INSERT);
        package.mutable_draw_data()->CopyFrom(draw_item);
    }else{
        package.set_message_type(PB_MessageType::PB_EDIT);
        update.mutable_draw_data()->CopyFrom(draw_item);
        update.set_draw_id(select_id);
        package.mutable_edit_data()->CopyFrom(update);
        select_id = -1;
    }
    package.SerializeToString(&serialized_package);
    send_message(serialized_package, clientSocket);
}

/* ========================================================================================================
 *                                                  Handle List
 * Handles List Command
 * - Generates the list of the command depending on what the parameters
 * - calls the list command
 * ========================================================================================================
 */
void Client::handle_list(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 3) {
        print_message("Error: Command expects 2 parameters [list <type> all/mine]");
        return;
    }

    if ((token_list[1].token_type != token_t::ALL       &&
         token_list[1].token_type != token_t::LINE      &&
         token_list[1].token_type != token_t::RECTANGLE &&
         token_list[1].token_type != token_t::CIRCLE    &&
         token_list[1].token_type != token_t::TEXT
        )||(
         token_list[2].token_type != token_t::ALL       &&
         token_list[2].token_type != token_t::MINE          )) {
        print_message("Error: Command expects a shape type and a user specifier");
        return;
    }
    int id = -1;
    if(token_list[2].token_type != token_t::ALL){id = user_id;}

    draw_mutex.lock();
    draw_list.list(token_list[1].token_type, id);
    draw_mutex.unlock();
}

/* ========================================================================================================
 *                                              Handle Select                                              
 * Handles Select Command
 * - sets the select_id to whatever the
 * ========================================================================================================
 */
void Client::handle_select(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 2) {
        print_message("Error: Command expects 1 parameters [select ID]");
        return;
    }

    if (token_list[1].token_type != token_t::INTEGER    && 
        token_list[1].token_type != token_t::NONE        ) {
        print_message("Error: Command expects an Integer or NONE");
        return;
    }
    if(token_list[1].token_type == token_t::INTEGER){
        select_id = std::stoi(token_list[1].lexeme_string);
    }else{
        select_id = -1;
    }
}

/* ========================================================================================================
 *                                              Handle Delete                                              
 * Handles Delete Command
 * - Deletes the selected ID
 * ========================================================================================================
 */
void Client::handle_delete(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 2) {
        print_message("Error: Command expects 1 parameters [delete ID]");
        return;
    }

    if (token_list[1].token_type != token_t::INTEGER) {
        print_message("Error: Command expects an Integer");
        return;
    }
    
    PB_Package  package;
    std::string serialized_package;

    package.set_message_type(PB_MessageType::PB_REMOVE);
    package.set_draw_id(std::stoi(token_list[1].lexeme_string));
    
    package.SerializeToString(&serialized_package);
    send_message(serialized_package, clientSocket);
}

/* ========================================================================================================
 *                                               Handle Undo                                               
 * Handles Undo Command
 * - undos the last command related to the user
 * ========================================================================================================
 */
void Client::handle_undo(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 1) {
        print_message("Error: Command expects no parameters [undo]");
        return;
    }

    PB_Package  package;
    std::string serialized_package;

    package.set_message_type(PB_MessageType::PB_UNDO);
    package.set_user_id(user_id);
    
    package.SerializeToString(&serialized_package);
    send_message(serialized_package, clientSocket);
}

/* ========================================================================================================
 *                                               Handle Show                                               
 * Handles Show Command
 * - sets the display method
 * ========================================================================================================
 */
void Client::handle_show(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 2) {
        print_message("Error: Command expects 1 parameter [show mine/all]");
        return;
    }
    if ( token_list[1].token_type != token_t::ALL   &&
         token_list[1].token_type != token_t::MINE      ) {
        print_message("Error: Command expects user specifier");
        return;
    }
    current_show = token_list[1].token_type;
    should_render = true;
    
}

/* ========================================================================================================
 *                                               Handle Clear                                              
 * Handles Clear Command
 * - Sends Clear to the Server
 * ========================================================================================================
 */
void Client::handle_clear(const std::vector<tokenised_t>& token_list) {
    if (token_list.size() != 2) {
        print_message("Error: Command expects 1 parameter [clear mine/all]");
        return;
    }
    
    PB_Package  package;
    std::string serialized_package;

    if(token_list[1].token_type == token_t::ALL){
        package.set_message_type(PB_MessageType::PB_CLEAR);
        package.set_user_id(-1);
    }else if (token_list[1].token_type == token_t::MINE){
        package.set_message_type(PB_MessageType::PB_CLEAR);
        package.set_user_id(user_id);
    }else{
        print_message("Error: Command expects user specifier");
        return;
    }
    
    package.SerializeToString(&serialized_package);
    send_message(serialized_package, clientSocket);
}

/* ========================================================================================================
 *                                               Handle Exit                                               
 * ========================================================================================================
 */
void Client::handle_exit(const std::vector<tokenised_t>& token_list) {
    kill_flag = true;
}
