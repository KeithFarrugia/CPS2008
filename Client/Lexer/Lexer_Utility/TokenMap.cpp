#include "../Lexer.h"
/** ==========================================================================================================================
 *  ---------------------- Map Token
 * Function takes a final state and a token and maps them together through a map
 * ===========================================================================================================================
 */
void Lexer::mapToken(state_t final_state, token_t token) {
    token_map[final_state] = token;
}

/** ==========================================================================================================================
 *  ---------------------- INIT Token Map
 * Map all tokens to a final state 28
 * ===========================================================================================================================
 */

void Lexer::initTokenMap(){

    //          FINAL STATE             ,           TOKEN
    
    mapToken(   state_t::ST_INT         ,           token_t::INTEGER                );
    mapToken(   state_t::ST_IDEN        ,           token_t::IDENTIFIER             );
    mapToken(   state_t::ST_WS          ,           token_t::WHITESPACE             );

    mapToken(   state_t::ST_STRING_2    ,           token_t::STRING                 );
}