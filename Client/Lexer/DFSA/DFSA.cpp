#include "DFSA.h"

const std::string DFSA::ErrorState = "ERROR";
const std::string DFSA::StartState = "START";

DFSA::DFSA(){
    transition_table_columns = 0;
    transition_table_rows = 0;
}

/** ==========================================================================================================================
 *  ---------------------- Generation of Transition Table
 * These are functions related to the generation of Transition Table
 * ===========================================================================================================================
 */

/** ---------------------- Create the Table -------------------------
 * Creates a 2d array that serves as the transition table based
 * on the number of lexemes and number of states.
 * 
 * Sets all the states to the Error state as Default 
 */
void DFSA::Generate_Transition_Table(){
    transition_table_columns = MAX_LEXEMES;
    transition_table_rows = MAX_STATES;
    
    transition_table.resize(transition_table_rows, std::vector<state_t>(transition_table_columns, state_t::ST_ERROR));
}

/** ---------------------- Add Transitions ----------------------------
 * Given a state, lexeme and the next state Creates the transition in the table
 */
void DFSA::addTransition(state_t state, lexeme_t lexeme, state_t next_state) {
    transition_table[state][lexeme] = next_state;
}


/** ==========================================================================================================================
 *  ---------------------- Usage of DFSA
 * The functions are to actually use the generated DFSA such as to validate a string
 * ===========================================================================================================================
 */

/** ---------------------- Get Next State ------------------------------
 * Given the Current State and a Lexeme the function Calculates the next state
 * From the Transition Table
 */
state_t DFSA::getNextState(state_t current_state, lexeme_t next_lexeme) const{
    if(current_state == state_t::ST_ERROR){
        return state_t::ST_ERROR;
    } else {
        return transition_table[current_state][next_lexeme];
    }
}



/** ==========================================================================================================================
 *  ---------------------- DISPLAY 
 *  For Readability and Dry Running
 * ===========================================================================================================================
 */
void DFSA::displayTable() const{
    const int numCols = transition_table_columns;
    const int numRows = transition_table_rows;

    // ----------------------------------------- Calculate column widths
    std::vector<int> colWidths(numCols + 1);

    // ----------------------------------------- Calculate the width of the first column which belongs to the column holding the row lengths
    for (int state_i = 0; state_i < MAX_STATES; state_i++) {
        int dataWidth = st_string(state_i).length();
        if (dataWidth > colWidths[0]) {
            colWidths[0] = dataWidth;
        }
    }

    // ----------------------------------------- Calculate the width of the columns containing the transition_table
    for (int lexeme_i = 0; lexeme_i < MAX_LEXEMES; lexeme_i++) {
        int lexemeWidth = lex_string(lexeme_i).length();
        for (int state_i = 0; state_i < MAX_STATES; state_i++) {
            int dataWidth = st_string(transition_table[state_i][lexeme_i]).length();
            if (dataWidth > lexemeWidth) {
                lexemeWidth = dataWidth;
            }
        }
        colWidths[lexeme_i + 1] = lexemeWidth;
    }

    // ----------------------------------------- Print column names
    // ------------------------- Leave Empty in Columns for Row Names
    std::cout << std::setw(colWidths[0] + 2) << std::left << "" << " ║ ";

    // ------------------------- Print Columns and their names
    for (int lexeme_i = 0; lexeme_i < MAX_LEXEMES; lexeme_i++) {
        std::cout << std::setw(colWidths[lexeme_i + 1]) << std::left << lex_string(lexeme_i) << " | ";
    }
    std::cout << std::endl;

    // ----------------------------------------- Print Separator For the column names
    std::cout << "+";
    for (int col = 0; col < numCols + 1; ++col) { std::cout << std::string(colWidths[col] + 2, '=') << "+"; }
    std::cout << std::endl;

    // ----------------------------------------- Print table transition_table
    for (int state_i = 0; state_i < MAX_STATES; state_i++) {

        // ------------------------- print the state (row name)
        std::cout << std::setw(colWidths[0] + 2) << std::left << st_string(state_i) << " ║ ";
        //
        for (int lexeme_i = 0; lexeme_i < MAX_LEXEMES; lexeme_i++) {
            std::cout << std::setw(colWidths[lexeme_i + 1]) << std::left << st_string(transition_table[state_i][lexeme_i]) << " | ";
        }
        std::cout << std::endl;

        // ------------------------- Print separator line
        std::cout << "+";
        for (int col = 0; col < numCols + 1; ++col) { std::cout << std::string(colWidths[col] + 2, '-') << "+"; }
        std::cout << std::endl;
    }
}