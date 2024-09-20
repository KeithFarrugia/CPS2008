#ifndef LEXEMES_H
#define LEXEMES_H
#include <string>

#define MAX_LEXEMES 7

typedef enum           {
    letter,
    digit,
    space,
    newline,

    quotation,
    minus,

    unknown
}lexeme_t;

static const std::string lex_string(const lexeme_t lexeme) {
    switch (lexeme) {
        case letter     : return "letter";
        case digit      : return "digit";

        case space      : return "space";
        case newline    : return "newline";
        
        case quotation  : return "quotation";
        case minus  : return "minus";

        case unknown    : return "unknown";
        default         : return "UNKNOWN_LEXEME";
    }
}

static const std::string lex_string(int state) {
    return lex_string(static_cast<lexeme_t>(state));
}
#endif // LEXEMES_H