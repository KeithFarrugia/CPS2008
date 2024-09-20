#include "Lexer.h"

/** ==========================================================================================================================
 *  ---------------------- Initialization
 * ===========================================================================================================================
 */
Lexer::Lexer(){
    initDFSA();
}

void Lexer::displayTable(){
    dfsa.displayTable();
}

void Lexer::initDFSA(){
    initTransitions();
    initTokenMap();
}



/** ==========================================================================================================================
 *  ---------------------- Next Token 
 *  Reads and Generates the next token starting at the index
 * ===========================================================================================================================
 */

tokenised_t Lexer::nextToken(std::string src_program_str, int& src_program_idx, const bool is_debug) {
    // ---------------------------- Initialisation
    std::vector<state_t> stack;
    std::string lexeme_string;
    state_t state = state_t::ST_START;
    stack.push_back(state_t::ST_SYNTAX_ERROR);  

    // ---------------------------- Read A toke
    while(state != state_t::ST_ERROR){

        // ----------------------- Printing Testing
        if(is_debug){
            std::cout<< std::setw(4) << src_program_idx << std::left;
        }



        // ----------------------- Delete Previous states if we reach a safe (final) state
        if (token_map.find(state) != token_map.end()) { stack.clear(); }

        // ----------------------- Push current State and get the next Char.
        stack.push_back(state);
        char next_char = nextChar(src_program_str, src_program_idx);


        // ----------------------- Check if end of string
        if (next_char == EOF) {  
            break; /*End of string*/ 
        }
        

        // ----------------------- Generate Lexeme
        lexeme_t lexeme_type = generateLexeme(next_char);

        // ----------------------- Printing Testing
        if(is_debug){
            std::cout << "[" << next_char<< "]   ";
            std::cout << std::setw(18) << std::left << lex_string(lexeme_type);
            std::cout << std::setw(18) << std::left << st_string(state);
        }
        // ----------------------- Calculate Next State 
        state = dfsa.getNextState(state, lexeme_type);

        if(state == state_t::ST_SYNTAX_ERROR){throw std::runtime_error("Error [LEXER - DFSA GENERATED]: SYNTAX ERROR [NOT EXPECTING A CHARACTER/DIGIT/UNDERSCORE]");}

        // ----------------------- Printing Testing
        if(is_debug){
            std::cout << std::setw(18) << std::left << st_string(state) <<"\n";
        }

        // ----------------------- Printing Testing
        if(state != state_t::ST_ERROR){lexeme_string += next_char;}else{src_program_idx--;}

    }

    // ================================ ROLL Back
    if(is_debug){
        std::cout<<"\n";
    }
    while (!stack.empty()) {

        // ----------------------- Reached the end of the stck => Error
        if (stack.back() == state_t::ST_SYNTAX_ERROR) { throw std::runtime_error("Error [LEXER]: SYNTAX ERROR [string]: " + lexeme_string); }

        // ----------------------- Not Accepting State
        if (token_map.find(stack.back()) == token_map.end()) {
            stack.pop_back();
            lexeme_string = lexeme_string.substr(0, lexeme_string.size() - 1);  // Remove the last character
            src_program_idx--;

        // ----------------------- Reached the Accepting State
        } else {
            state = stack.back();
            stack.pop_back();
            break;
        }
    }
    // ----------------------- Success!
    if (token_map.find(state) != token_map.end()) {
        tokenised_t temp_tok = generateToken(state, lexeme_string);

        switch (temp_tok.token_type){
            case token_t::IDENTIFIER:
                temp_tok.token_type = identify(temp_tok.lexeme_string);
                if(temp_tok.token_type == IDENTIFIER) {throw std::runtime_error("Error [LEXER]: Invalid Keyword: " + temp_tok.lexeme_string);}
                break;
            
            case token_t::STRING:
                temp_tok.lexeme_string = removeFirstAndLast(temp_tok.lexeme_string);
                break;
            
            default:
                break;
        }
        return temp_tok;

    // ----------------------- Error
    } else {
        throw std::runtime_error("Error [LEXER]: LEXICAL ERROR");
    }
}


/** ==========================================================================================================================
 *  ---------------------- Generate Tokens 
 *  Generate all tokens
 * ===========================================================================================================================
 */
std::vector<tokenised_t> Lexer::GenerateTokens(const std::string& src_program_str, const bool is_debug) {
    std::vector<tokenised_t> tokens_list;
    int src_program_idx = 0;

    while (true) {

        // --------------------- Read Error
        tokenised_t token = nextToken(src_program_str, src_program_idx, is_debug);
        if(token.token_type != token_t::WHITESPACE){
            tokens_list.push_back(token);
        }
        
        
        if (peekChar(src_program_str, src_program_idx) == EOF) { break; /* The end of the source program */ }
    }
    
    return tokens_list;
}
