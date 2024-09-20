#include "../Client.h"

/* ========================================================================================================
 *                                             Execute Command                                             
 * Checks the command package type and executes it
 * it has a draw mutex that controlls it but this is sudo redundent since
 * the drawlist itself already has built in mutex locks.
 * ========================================================================================================
 */
void Client::execute_command(PB_Package& package){
    // ---------------------------------------------- Lock Mutex
    draw_mutex.lock();


    switch (package.message_type()){

        // ---------------------------------------------- INVALID
        case PB_MessageType::PB_INVALID:
            break;

        // ---------------------------------------------- INSERT
        case PB_MessageType::PB_INSERT: 
            draw_list.insert(package.draw_data());
            should_render = true;
            break;
        
        // ---------------------------------------------- EDIT
        case PB_MessageType::PB_EDIT:
            draw_list.edit(
                package.edit_data().draw_id(),
                package.edit_data().draw_data()
            );
            should_render = true;
            break;
        
        // ---------------------------------------------- DELETE
        case PB_MessageType::PB_REMOVE:
            draw_list.erase(package.draw_id());
            should_render = true;
            break;
        
        // ---------------------------------------------- CLEAR
        case PB_MessageType::PB_CLEAR :
            draw_list.clear(package.user_id());
            should_render = true;
            break;
        
        // ---------------------------------------------- UNDO
        case PB_MessageType::PB_UNDO :
            draw_list.undo(package.user_id());
            should_render = true;
            break;
        // ---------------------------------------------- BULK
        case PB_MessageType::PB_BULK :
            draw_list.load(package.bulk_draw_data());
            should_render = true;
            break;

        // ---------------------------------------------- Error
        case PB_MessageType::PB_ERROR : 
            print_message("Error/Shutdown from server [" + package.generic() + "]");
            kill_flag = true;
            break;

        // ---------------------------------------------- DEFUALT
        default:
            print_message("Message from server [" + package.generic() +"]");
            break;
    }


    // ---------------------------------------------- Un-Lock Mutex
    draw_mutex.unlock();
}

