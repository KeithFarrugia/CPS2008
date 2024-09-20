#ifndef STATES_H
#define STATES_H
#include <string>

#define MAX_STATES 9
typedef enum {
    ST_START,
    ST_ERROR,
    ST_SYNTAX_ERROR, // used in the stack to have an endpoint
    
    // --------------------- INTEGER
    ST_INT,
    ST_INT_NEG,

    // --------------------- IDENTTIFIER STATES
    ST_IDEN,
    
    // --------------------- WHITESPACE STATES
    ST_WS,

    // --------------------- STRING STATES
    ST_STRING_1,
    ST_STRING_2,
}state_t;


static const std::string st_string(const state_t state) {
    switch (state) {
        case ST_START               : return "START";
        case ST_ERROR               : return "ERROR";
        case ST_SYNTAX_ERROR        : return "ST_SYNTAX_ERROR";

        // ---------------------  INTEGER
        case ST_INT                 : return "ST_INT";
        case ST_INT_NEG             : return "ST_INT_NEG";

        // --------------------- IDENTTIFIER STATES
        case ST_IDEN                : return "ST_IDEN";

        // --------------------- WHITESPACE STATES
        case ST_WS                  : return "ST_WS";
    
        // --------------------- WHITESPACE STATES
        case ST_STRING_1            : return "ST_STRING_1";
        case ST_STRING_2            : return "ST_STRING_2";

        // --------------------- OTHER
        default                     : return "UNKNOWN_STATE";
    }
}
static const std::string st_string(int state) {
    return st_string(static_cast<state_t>(state));
}
#endif // STATES_H