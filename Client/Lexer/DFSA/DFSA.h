#ifndef DFSA_H
#define DFSA_H
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iomanip>
#include <unordered_set>

#include "../Types/Include_Types.h"
#include "../Utility.h"
class DFSA{

public:
    DFSA();

    static const std::string ErrorState;
    static const std::string StartState;

    void Generate_Transition_Table();

    void addTransition(state_t state, lexeme_t lexeme, state_t next_state);

    void displayTable()const;

    state_t getNextState(state_t current_state, lexeme_t next_lexeme)const;
    //bool isFinalState(state_t state)const;

private:

    std::vector<std::vector<state_t>> transition_table;
    int transition_table_columns, transition_table_rows;

};

#endif // DFSA_H
