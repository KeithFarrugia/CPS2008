#include "../Client.h"

#define COMMAND_COL     15
#define PARAMETER_COL   30
#define DESCRIPTION_COL 40

/* =========================================================================================================================
 *                                                      Print Commands                                                      
 * Prints a list of commands, their parameters
 * and a small Description in neat columns
 * =========================================================================================================================
 */
void Client::print_comands(){
    const std::string CYAN = "\033[36m";
    const std::string GREEN = "\033[32m";
    const std::string RESET = "\033[0m";

    std::cout << GREEN << "\n> " << CYAN << "======================================================================================================\n";
    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "Command Name";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "Parameters";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Description";
    std::cout << "\n\n";
    
    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "help"; 
    std::cout << std::setw(PARAMETER_COL)   << std::left << "----------";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Lists all available commands and their usage";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "tool";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{line | rectangle";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Select tool/shape for drawing";
    std::cout << "\n";
    std::cout << std::setw(COMMAND_COL)     << std::left << "";
    std::cout << std::setw(PARAMETER_COL)   << std::left << " | circle | text}";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "draw";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{parameters} fill?";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Draws depending on the given parameter";
    std::cout << "\n";
    std::cout << std::setw(COMMAND_COL)     << std::left << "";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "and current choosen tool/colour";
    std::cout << "\n\n";
    
    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "list";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{all | line | rectangle";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Draws depending on the given parameter";
    std::cout << "\n";
    std::cout << std::setw(COMMAND_COL)     << std::left << "";
    std::cout << std::setw(PARAMETER_COL)   << std::left << " | circle | text}";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "and current choosen tool/colour";
    std::cout << "\n";
    std::cout << std::setw(COMMAND_COL)     << std::left << "";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{all | mine}";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "select"; 
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{none | ID}";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Selects an existing draw command";
    std::cout << "\n";
    std::cout << std::setw(COMMAND_COL)     << std::left << "";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << " to be modified in the next draw";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "delete"; 
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{ID}";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Selects an existing draw command";
    std::cout << "\n";
    std::cout << std::setw(COMMAND_COL)     << std::left << "";
    std::cout << std::setw(PARAMETER_COL)   << std::left << "";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << " to delete";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "undo"; 
    std::cout << std::setw(PARAMETER_COL)   << std::left << "----------";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Undo the last command";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "clear"; 
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{all | mine}";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Clears the canvas";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "show"; 
    std::cout << std::setw(PARAMETER_COL)   << std::left << "{all | mine}";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Displays the Canvas";
    std::cout << "\n\n";

    std::cout << GREEN << "> " << CYAN;
    std::cout << std::setw(COMMAND_COL)     << std::left << "exit"; 
    std::cout << std::setw(PARAMETER_COL)   << std::left << "----------";
    std::cout << std::setw(DESCRIPTION_COL) << std::left << "Disconnect from server and exits the application";
    
    std::cout << GREEN << "\n\n> " << CYAN;
    std::cout << "======================================================================================================\n\n";
    std::cout << GREEN << "> " << RESET << std::flush;
}
