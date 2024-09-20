#ifndef TOKENS_H
#define TOKENS_H
#include <string>

#define MAX_TOKENS 24
typedef enum {
  // --------------------- General Types
  INTEGER,
  IDENTIFIER,
  WHITESPACE,
  STRING,
  
  // --------------------- Keywords
  LINE,
  RECTANGLE,
  CIRCLE,
  TEXT,
  FILL,
  
  ALL,
  MINE,
  NONE,

  // --------------------- Commands
  HELP,
  TOOL,
  COLOUR,
  DRAW,
  LIST,
  SELECT,
  DELETE,
  UNDO,
  CLEAR,
  SHOW,
  EXIT,


  // --------------------- Other
  NULL_TOKEN,

} token_t;

static const std::string tk_string(const token_t token) {
    switch (token) {
        // --------------------- General Types
        case INTEGER            : return "INTEGER";
        case IDENTIFIER         : return "IDENTIFIER";
        case WHITESPACE         : return "WHITESPACE";
        case STRING             : return "STRING";
        
        // --------------------- Keywords
        case LINE               : return "LINE";
        case RECTANGLE          : return "RECTANGLE";
        case CIRCLE             : return "CIRCLE";
        case TEXT               : return "TEXT";
        case FILL               : return "FILL";
        case ALL                : return "ALL";
        case MINE               : return "MINE";
        case NONE               : return "NONE";
        
        // --------------------- Commands
        case HELP               : return "HELP";
        case TOOL               : return "TOOL";
        case COLOUR             : return "COLOUR";
        case DRAW               : return "DRAW";
        case LIST               : return "LIST";
        case SELECT             : return "SELECT";
        case DELETE             : return "DELETE";
        case UNDO               : return "UNDO";
        case CLEAR              : return "CLEAR";
        case SHOW               : return "SHOW";
        case EXIT               : return "EXIT";
        
        // --------------------- Other
        case NULL_TOKEN         : return "NULL_TOKEN";
        default                 : return "UNKNOWN_TOKEN";
    }
}

static const std::string tk_string(int token) {
  return tk_string(static_cast<token_t>(token));
}
#endif  // TOKENS_H