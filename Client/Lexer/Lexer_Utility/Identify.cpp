#include "../Lexer.h"

token_t Lexer::identify(std::string lexeme_string){
    toUpperCase(lexeme_string);


    if      (   lexeme_string == "HELP"     )   { return token_t::HELP;     }
    else if (   lexeme_string == "TOOL"     )   { return token_t::TOOL;     }
    else if (   lexeme_string == "COLOUR"   )   { return token_t::COLOUR;   }
    else if (   lexeme_string == "DRAW"     )   { return token_t::DRAW;     }
    else if (   lexeme_string == "LIST"     )   { return token_t::LIST;     }
    else if (   lexeme_string == "SELECT"   )   { return token_t::SELECT;   }
    else if (   lexeme_string == "DELETE"   )   { return token_t::DELETE;   }
    else if (   lexeme_string == "UNDO"     )   { return token_t::UNDO;     }
    else if (   lexeme_string == "CLEAR"    )   { return token_t::CLEAR;    }
    else if (   lexeme_string == "SHOW"     )   { return token_t::SHOW;     }
    else if (   lexeme_string == "EXIT"     )   { return token_t::EXIT;     }

    
    else if (   lexeme_string == "LINE"         )   { return token_t::LINE;         }
    else if (   lexeme_string == "RECTANGLE"    )   { return token_t::RECTANGLE;    }
    else if (   lexeme_string == "CIRCLE"       )   { return token_t::CIRCLE;       }
    else if (   lexeme_string == "TEXT"         )   { return token_t::TEXT;         }
    else if (   lexeme_string == "FILL"         )   { return token_t::FILL;         }
    else if (   lexeme_string == "ALL"          )   { return token_t::ALL;          }
    else if (   lexeme_string == "MINE"         )   { return token_t::MINE;         }
    else if (   lexeme_string == "NONE"         )   { return token_t::NONE;         }

    return token_t::IDENTIFIER;
 }