#include "../Lexer.h"
/** ==========================================================================================================================
 *  ---------------------- Initialization of Transitions
 * ===========================================================================================================================
 */




void Lexer::initTransitions(){
    dfsa.Generate_Transition_Table();

    //                      Current State               ,           Lexime                      ,           Next State

// INTEGER/FLOAT ---------------------------------------------------------------------------------------------------------------------
    dfsa.addTransition(     state_t::ST_START           ,           lexeme_t::digit             ,           state_t::ST_INT         );
    dfsa.addTransition(     state_t::ST_INT             ,           lexeme_t::digit             ,           state_t::ST_INT         );

    dfsa.addTransition(     state_t::ST_START           ,           lexeme_t::minus             ,           state_t::ST_INT_NEG     );
    dfsa.addTransition(     state_t::ST_INT_NEG         ,           lexeme_t::digit             ,           state_t::ST_INT         );

    // SYNTAX ERROR HANDLING
    dfsa.addTransition(     state_t::ST_INT             ,           lexeme_t::letter            ,           state_t::ST_SYNTAX_ERROR);


// IDENTIFIER ------------------------------------------------------------------------------------------------------------------------
    dfsa.addTransition(     state_t::ST_START           ,           lexeme_t::letter            ,           state_t::ST_IDEN        );

    dfsa.addTransition(     state_t::ST_IDEN            ,           lexeme_t::letter            ,           state_t::ST_IDEN        );
    

// WHITESPACE ------------------------------------------------------------------------------------------------------------------------
    dfsa.addTransition(     state_t::ST_START           ,           lexeme_t::space             ,           state_t::ST_WS          );
    dfsa.addTransition(     state_t::ST_START           ,           lexeme_t::newline           ,           state_t::ST_WS          );

    dfsa.addTransition(     state_t::ST_WS              ,           lexeme_t::space             ,           state_t::ST_WS          );
    dfsa.addTransition(     state_t::ST_WS              ,           lexeme_t::newline           ,           state_t::ST_WS          );

// STRING ------------------------------------------------------------------------------------------------------------------------
    dfsa.addTransition(     state_t::ST_START           ,           lexeme_t::quotation         ,           state_t::ST_STRING_1    );

    dfsa.addTransition(     state_t::ST_STRING_1        ,           lexeme_t::letter            ,           state_t::ST_STRING_1    );
    dfsa.addTransition(     state_t::ST_STRING_1        ,           lexeme_t::digit             ,           state_t::ST_STRING_1    );
    dfsa.addTransition(     state_t::ST_STRING_1        ,           lexeme_t::space             ,           state_t::ST_STRING_1    );
    dfsa.addTransition(     state_t::ST_STRING_1        ,           lexeme_t::newline           ,           state_t::ST_STRING_1    );
    dfsa.addTransition(     state_t::ST_STRING_1        ,           lexeme_t::unknown           ,           state_t::ST_STRING_1    );

    
    dfsa.addTransition(     state_t::ST_STRING_1        ,           lexeme_t::quotation         ,           state_t::ST_STRING_2    );

}