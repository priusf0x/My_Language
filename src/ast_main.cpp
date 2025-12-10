#include "my_string.h"
#include "state_machine_functions.h"

int
main()
{
    state_machine_t state_machine_gen = NULL;


    StateMachineInit(&state_machine_gen, 10); 



    AddKeyWord("внезапный-деп", " \n\t\r()[]{};", 2,state_machine_gen);

    AddKeyWord("внезапный-сосал", " \n\t\r()[]{};", 1, state_machine_gen);

    AddKeyWord("внезmeow", " \n\t\r()[]{};", 3, state_machine_gen);

    state_t current_state = 0;
    const char* string = "внезапный-деп внеmeow";
    while (!(current_state & END_STATE_FLAG))
    {
        current_state = GetNextState((unsigned char) *string, current_state, 
                                        state_machine_gen);
        string++;
    }   
    fprintf(stderr, "%u ",current_state);

    // string++;
    current_state = 0;
    while (!(current_state & END_STATE_FLAG))
    {
        current_state = GetNextState((unsigned char) *string, current_state, 
                                        state_machine_gen);
        string++;
    }   
    fprintf(stderr, "%u",current_state);

    WriteInFileStateMachine(state_machine_gen, "meow.h");

    StateMachineDestroy(&state_machine_gen);

    return 0;
}