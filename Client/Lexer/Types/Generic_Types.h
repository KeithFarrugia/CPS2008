#ifndef GENERIC_TYPES_H
#define GENERIC_TYPES_H

#include "Tokens.h"
#include <string>
#include <memory>
#include <vector>

typedef struct tokenised_t{
    token_t token_type;
    std::string lexeme_string;

    tokenised_t(){}
    tokenised_t(token_t token_type, std::string lexeme_string) : token_type(token_type), lexeme_string(lexeme_string) {}
    
}tokenised_t;



#endif //GENERIC_TYPES_H