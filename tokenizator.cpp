#include <iostream>
#include <stdlib.h>



//they dont work by the way :)) i'll look up for syntax and refactor
#define BAD_TOKEN(ptr)                                           \
int parse_error_pos = (int)((ptr->ptr) - (ptr->origin));         \
single_token bad_token = {};                                     \
                                                                 \
bad_token.tag         = PARSE_ERROR                              \
bad_token.in_line_pos = parse_error_pos                          \
                                                                 \
return bad_token;                                                \

#define SUCCESS_TOKEN()                          \
single_token success_token = {};                 \
success_token.tag = PARSE_SUCCESS;               \
                                                 \
return success_token;                            \


#define POSITION(ptr) (int)((ptr->ptr) - (ptr->origin))

//developing.......................................................




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
    NOT_A_COMMAND        = 10,
    PARSE_ERROR          = 11,
    PARSE_SUCCESS        = 12
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

enum ARITHMETIC_SIGNS
{
    PLUS  = 0,
    MINUS = 1,
    MUL   = 2,
    DIV   = 3
};

enum REGISTER_CODE
{
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3
};


typedef struct _token {
    enum command_tags tag;
    enum command_list command_code;
    int  i_value;            //value if its number
    int  register_code;      // rax - 0, rbx - 1, rcx - 2, rdx - 3
    int  arithmetic_code;    // '+' - 0, '-' - 1, '*' - 2, '/' - 3
    int  in_line_pos;   // position in the source text line
    char mark[10] = "";     //mark name
}single_token;

struct set_of_tokens {
    single_token* set_of_tokens;
    int size_of_set;
};


struct parse_ptr
{
    char* ptr;
    char* origin;
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

    struct parse_ptr ptr = {};
    ptr.origin = test_string;
    ptr.ptr    = test_string;

    int error_code = create_tokens_from_string(&ptr, &set_of_tokens);

    print_tokens(tokens_from_string);

    free  (tokens_from_string);
    fclose(source_file);

    return 0;
}

bool add_register_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_command_token (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_bracket_token (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_number_token  (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_mark_token    (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_sign_token    (struct parse_ptr* ptr, struct set_of_tokens* set);
void skip_spaces       (struct parse_ptr* ptr);

single_token create_tokens_from_string(struct parse_ptr* ptr, struct set_of_tokens* set) { //in case of error, it returns
                                                                                              //single token with required info to display
                                                                             
    while (*(ptr->ptr) != "\0"){
        skip_spaces(&parse_ptr);

        // i added 'breaks' to make my code more readable
        if      (add_command_token (ptr, set))  
            break;
        else if (add_register_token(ptr, set))
            break;
        else if (add_number_token  (ptr, set))
            break;
        else if (add_mark_token    (ptr, set))
            break;
        else if (add_bracket_token (ptr, set))
            break;
        else if (add_sign_token    (ptr, set))
            break;
        else if (*(ptr->ptr) != '\0')
            BAD_TOKEN(ptr)  //aka unknown symbol was met
    }


    SUCCESS_TOKEN(); //in case if everything worked out fine
}


void skip_spaces(struct parse_ptr* ptr){
    while (*(ptr->ptr) == ' ' && *(ptr->ptr) != '\0'){ //then we are inside the string
        (ptr->ptr) ++;
    } 
}


bool add_command_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char command[100] = "";

    int start_command = 0;
    int end_command   = 0;

    sscanf(ptr->ptr, "%n%[^# ]%n", &start_command, command, &end_command);
    int command_size = end_command - start_command;

    int command_code = find_command(command);

    if (command_code < 0)
        return false;
    else 
    {
        single_token token = {};
        token.tag          = COMMAND;
        token.command_code = command_code;
        token.in_line_pos  = POSITION(ptr);

        set->set_of_tokens[set->size_of_set] = token;
        set->size_of_set ++;

        (*parse_ptr) += command_size;
        return true;
    }
}

bool add_register_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char register_name[3] = "";

    for (int i = 0; i < 3; i ++){
        if (*(ptr->ptr) != '\0') // check if we inside
            register_name[i] = (*parse_ptr)[i];
    }

    int register_code = find_register(register_name);

    if (register_code < 0)
        return false;
    else
    {
        single_token token  = {};
        token.tag           = REGISTER;
        token.register_code = register_code;
        token.in_line_pos   = POSITION(ptr);

        set->set_of_tokens[set->size_of_set] = token;
        set->size_of_set ++;

        (*parse_ptr) += 3; //move it for register size
        return true;
    }
}

bool add_number_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char* old_ptr = ptr->ptr;
    int val = 0;

    while ('0' <= *(ptr->ptr) && *(ptr->ptr) <= '9') {
        val = val*10 + *(ptr->ptr) - '0';
        (ptr->ptr) ++;
    }

    if (old_ptr == *parse_ptr)
        return false;
    else
    {
        //just create macroses to fill these repetarive code parts
        single_token token = {};
        token.tag          = NUMBER;
        token.i_value      = val;
        token.in_line_pos  = POSITION(ptr);

        set->set_of_tokens[set->size_of_set] = token;
        set->size_of_set ++;

        return true;
    }
} 

bool add_mark_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    if ((*(ptr->ptr)) == ':'){
        (ptr->ptr) ++;

        char mark[100] = "";
        int start_mark = 0;
        int end_mark   = 0;

        sscanf(ptr->ptr, "%n%[^# ]%n", &start_mark, mark, &end_mark);
        int mark_size = end_mark - start_mark;

        //assert(mark_size > MAX_MARK_SIZE) !!!
        //Do same for command size

        if (mark_size == 0)
            return false //empty mark - syntax error
        else 
        {
            single_token token = {};
            token.tag          = MARK;
            strncpy(token.mark, mark, mark_size);
            token.in_line_pos = POSITION(ptr);
            
            set->set_of_tokens[set->set_of_tokens] = token;
            set->size_of_set ++;

            (ptr->ptr += mark_size);
            return true;
        }
    }
    else
        return false; //if there was not found ':' sign at all
}

bool add_sign_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    switch (*(ptr->ptr)){
        case '+':
        {
            single_token token    = {};
            token.tag             = ARITHMETIC_SIGN;
            token.arithmetic_code = PLUS;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++;

            (ptr->ptr) ++;
            return true;
        }
        break;

        case '-':
        {
            single_token token    = {};
            token.tag             = ARITHMETIC_SIGN;
            token.arithmetic_code = MINUS;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++; 

            (ptr->ptr) ++;
            return true;
        }

        break;

        case '*':
        {
            single_token token    = {};
            token.tag             = ARITHMETIC_SIGN;
            token.arithmetic_code = MUL;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++; 

            (ptr->ptr) ++;
            return true;
        }
        break;

        case '/':
        {
            single_token token    = {};
            token.tag             = ARITHMETIC_SIGN;
            token.arithmetic_code = DIV;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++; 

            (ptr->ptr) ++;
            return true;
        }
        break;

        default:
            return false;
        break;
    }

    return false;
}

bool add_bracket_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    switch (*(ptr->ptr)){
        case '(':
        {
            single_token token    = {};
            token.tag             = OPEN_ROUND_BRACKET;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++; 

            (ptr->ptr) ++;
            return true;
        }
        break;

        case ')':
        {
            single_token token    = {};
            token.tag             = CLOSE_ROUND_BRACKET;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++; 

            (ptr->ptr) ++;
            return true;
        }
        break;

        case '[':
        {
            single_token token    = {};
            token.tag             = OPEN_SQUARE_BRACKET;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++; 

            (ptr->ptr) ++;
            return true;
        }
        break;

        case ']':
        {
            single_token token    = {};
            token.tag             = CLOSE_SQUARE_BRACKET;
            token.in_line_pos     = POSITION(ptr);

            set->set_of_tokens[set->size_of_set] = token;
            set->size_of_set ++; 

            (ptr->ptr) ++;
            return true;
        }
        break;

        default:
            return false;
        break;
    }

    return false;
}