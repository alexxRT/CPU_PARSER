#include <iostream>
#include <stdlib.h>


char commands[] = (
    "push",
    "pop",
    "mul",
    "in",
    "add",
    "div",
    "sub",
    "out",
    "pop",
    "hlt",
    "call",
    "jump",
    "jump_b",
    "jump_a",
    "jump_be",
    "jump_ae",
    ":",
    "(", ")",
    "[", "]",
    "+", "-", "*", "/"
);

enum command_tags{
    COMMAND              = 0,
    MARK                 = 1,
    REGISTER             = 2,
    NUMBER               = 3,
    OPEN_SQUARE_BRACKET  = 4,
    CLOSE_SQUARE_BRACKET = 5,
    OPEN_ROUND_BRACKET   = 6,
    CLOSE_ROUND_BRACKET  = 7,
    COMMENT              = 8,
    ARITHMETIC_SIGN      = 9,
    NOT_A_COMMAND        = 10
};

enum command_list {
    HLT     = 0,
    PUSH    = 1,
    ADD     = 2,
    SUB     = 3,
    MUL     = 4,
    DIV     = 5,
    OUT     = 6,
    IN      = 7,
    JUMP    = 8,
    JMP_A   = 9,
    JMP_B   = 10,
    JMP_AE  = 11,
    JMP_BE  = 12,
    CALL    = 13, 
    POP     = 14,
    UNKNOWN_COMMAND = 15,
};


typedef struct _token {
    enum command_tags tag;
    enum command_list command_code;
    int  i_value;            //value if its number
    int  register_code;      // rax - 0, rbx - 1, rcx - 2, rdx - 3
    int  arithmetic_code;    // '+' - 0, '-' - 1, '*' - 2, '/' - 3
    int  in_line_position;   // position in the source text line
    char mark[10] = "";     //mark name
}single_token;

struct set_of_tokens {
    single_token* set_of_tokens;
    int size_of_set;
};


//TASK - write a solid tokenizator :)
//BEFORE write a code - think about its logic

//command -> open_bracket -> register -> arithmetic_sign -> number -> close_bracket ->.....


int get_file_size (FILE* file) {
    fseek(file, SEEK_SET, SEEK_END);
    int file_size = ftell(file);
    printf ("file size is %d\n", file_size);
    fseek(file, SEEK_SET, SEEK_SET);

    return file_size;
}



int main () {

    FILE* source_file = fopen("source_file.txt", "r");
    assert (source_file != NULL);

    int file_size = get_file_size(source_file);

    char test_string[100] = "";
    fread(test_string, sizeof(char), file_size, source_file);

    struct set_of_tokens new_set;
    single_token* tokens_from_string = (single_token*)calloc(file_size, sizeof(single_token));

    new_set.set_of_tokens = tokens_from_string;
    new_set.size_of_set   = 0;

    int error_code = create_tokens_from_string(test_string, &set_of_tokens);

    print_tokens(tokens_from_string);

    free  (tokens_from_string);
    fclose(source_file);

    return 0;
}

bool add_register_token(char** parse_ptr, struct set_of_tokens* set);
bool add_command_token (char** parse_ptr, struct set_of_tokens* set);
bool add_bracket_token (char** parse_ptr, struct set_of_tokens* set);
bool add_number_token  (char** parse_ptr, struct set_of_tokens* set);
bool add_mark_token    (char** parse_ptr, struct set_of_tokens* set);
bool add_sign_token    (char** parse_ptr, struct set_of_tokens* set);
void skip_spaces       (char** parse_ptr);

single_token create_tokens_from_string(char* string, struct set_of_tokens* set) { //in case of error, it returns
    char* parse_ptr = string;                                                      //single token with required info to display
                                                                             
    while (*parse_ptr != "\0"){
        skip_spaces(&parse_ptr);

        // i added breaks expressions to make my code more readable
        if      (add_command_token (&parse_ptr, set))  
            break;
        else if (add_register_token(&parse_ptr, set))
            break;
        else if (add_number_token  (&parse_ptr, set))
            break;
        else if (add_mark_token    (&parse_ptr, set))
            break;
        else if (add_bracket_token (&parse_ptr, set))
            break;
        else if (add_sign_token    (&parse_ptr, set))
            break;
        else if (*parse_ptr != '\0')
            return bad_token;  //aka unknown symbol was met
    }


    return success_token; //in case if everything worked out fine
}


void skip_spaces(char** parse_ptr){
    while ((**parse_ptr) == ' ' && (**parse_ptr) != '\0'){ //then we are inside the string
        (*parse_ptr) ++;
    } 
}


bool add_command_token(char** parse_ptr, struct set_of_tokens* set){
    char command[100] = "";

    int start_command = 0;
    int end_command   = 0;

    sscanf(*parse_ptr, "%n%[^# ]%n", &start_command, command, &end_command);
    int command_size = end_command - start_command;

    int command_code = find_command(command);

    if (command_code < 0)
        return false;
    else 
    {
        //add new token in the set
        (*parse_ptr) += command_size;
        return true;
    }
}

bool add_register_token(char** parse_ptr, struct set_of_tokens* set){
    char register_name[3] = "";

    for (int i = 0; i < 3; i ++){
        if ((**parse_ptr) != '\0') // check if we inside
            register_name[i] = (*parse_ptr)[i];
    }

    int register_code = find_register(register_name);

    if (register_code < 0)
        return false;
    else
    {
        //add new token
        (*parse_ptr) += 3; //move it for register size
        return true;
    }
}

bool add_number_token(char** parse_ptr, struct set_of_tokens* set){
    char* old_ptr = *parse_ptr;
    int val = 0;

    while ('0' <= **parse_ptr && **parse_ptr <= '9')
    {
        val = val*10 + **parse_ptr - '0';
        (*parse_ptr) ++;
    }

    if (old_ptr == *parse_ptr)
        return false;
    else
    {
        //add new token
        return true;
    }
} 

bool add_mark_token(char** parse_ptr, struct set_of_tokens* set){
    if ((**parse_ptr) == ':'){
        (*parse_ptr) ++;

        char mark[100] = "";
        int start_mark = 0;
        int end_mark   = 0;

        sscanf(*parse_ptr, "%n%[^# ]%n", &start_mark, mark, &end_mark);
        int mark_size = end_mark - start_mark;

        if (mark_size == 0)
            return false //empty mark - syntax error
        else 
        {
            //add new mark
            (*parse_ptr += mark_size);
            return true;
        }
    }
    else
        return false; //if there was not found ':' sign at all
}

bool add_sign_token(char** parse_ptr, struct set_of_tokens* set){
    switch (**parse_ptr){
        case '+':
            //add token with + code
            (*parse_ptr) ++;
            return true;
        break;

        case '-':
            //add token with - code
            (*parse_ptr) ++;
            return true;
        break;

        case '*':
            //add token with * code
            (*parse_ptr) ++;
            return true;
        break;

        case '/':
            //add token with / code
            (*parse_ptr) ++;
            return true;
        break;

        default:
            return false;
        break;
    }

    return false;
}

bool add_bracket_token(char** parse_ptr, struct set_of_tokens* set){
    switch (**parse_ptr){
        case '(':
            //add token with ( code
            (*parse_ptr) ++;
            return true;
        break;

        case ')':
            //add token with ) code
            (*parse_ptr) ++;
            return true;
        break;

        case '[':
            //add token with [ code
            (*parse_ptr) ++;
            return true;
        break;

        case ']':
            //add token with ] code
            (*parse_ptr) ++;
            return true;
        break;

        default:
            return false;
        break;
    }

    return false;
}