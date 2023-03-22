#include <iostream>
#include <stdlib.h>

static const int MAX_MARK_SIZE = 10;

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
    NEW_LINE             = 9, 
    ARITHMETIC_SIGN      = 10,
    NOT_A_COMMAND        = 11,
    PARSE_ERROR          = 12,
    PARSE_SUCCESS        = 13
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

enum arithmetic_sign
{
    PLUS  = 0,
    MINUS = 1,
    MULTIPLY = 2,
    DIVISION = 3
};

enum register_list
{
    RAX = 0,
    RBX = 1,
    RCX = 2,
    RDX = 3,
    UNKNOWN_REGISTER = 4
};


typedef struct _token {
    enum command_tags  tag;
    enum command_list  command_code;
    enum register_list register_code; // rax - 0, rbx - 1, rcx - 2, rdx - 3
    int  arithmetic_code;             // '+' - 0, '-' - 1, '*' - 2, '/' - 3
    int  i_value;       //value if its number
    int  in_line_pos;   // position in the source text line
    char mark[MAX_MARK_SIZE] = ""; //mark name
}single_token;

struct set_of_tokens {
    single_token* set_of_tokens;
    int size_of_set;
};

#define POSITION(ptr) (int)((ptr->offset) - (ptr->origin))

#define ADD_REGISTER_TOKEN(set, register_code)                   \
do{                                                              \
    single_token token  = {};                                    \
    token.tag           = REGISTER;                              \
    token.register_code = register_code;                         \
    token.in_line_pos   = POSITION(ptr);                         \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_COMMAND_TOKEN(set, command_code)                     \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = COMMAND;                                \
    token.command_code = command_code;                           \
    token.in_line_pos  = POSITION(ptr);                          \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_COMMENT_TOKEN(set)                                   \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = COMMENT;                                \
    token.in_line_pos  = POSITION(ptr);                          \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_NUMBER_TOKEN(set, i_value)                           \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = NUMBER;                                 \
    token.i_value      = i_value;                                \
    token.in_line_pos  = POSITION(ptr);                          \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_BRAKET_TOKEN(set, bracket_code)                      \
do{                                                              \
    single_token token    = {};                                  \
    token.tag             = bracket_code;                        \
    token.in_line_pos     = POSITION(ptr);                       \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_MARK_TOKEN(set, mark)                                \
do{                                                              \
    single_token token = {};                                     \
    token.tag          = MARK;                                   \
    strncpy(token.mark, mark, MAX_MARK_SIZE);                    \
    token.in_line_pos = POSITION(ptr);                           \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \

#define ADD_SIGN_TOKEN(set, sign_code)                           \
do{                                                              \
    single_token token    = {};                                  \
    token.tag             = ARITHMETIC_SIGN;                     \
    token.arithmetic_code = sign_code;                           \
    token.in_line_pos     = POSITION(ptr);                       \
                                                                 \
    set->set_of_tokens[set->size_of_set] = token;                \
    set->size_of_set ++;                                         \
}while(0)                                                        \


#define BAD_TOKEN(ptr)                                           \
do{                                                              \
single_token bad_token = {};                                     \
                                                                 \
bad_token.tag         = PARSE_ERROR;                             \
bad_token.in_line_pos = (int)((ptr->offset) - (ptr->origin));    \
                                                                 \
return bad_token;                                                \
}                                                                \
while(0)                                                         \

#define SUCCESS_TOKEN()                          \
do {                                             \
single_token success_token = {};                 \
success_token.tag = PARSE_SUCCESS;               \
                                                 \
return success_token;                            \
}                                                \
while(0)                                         \


struct parse_ptr
{
    char* offset;
    char* origin;
};

int get_file_size (FILE* file) {

    fseek(file, SEEK_SET, SEEK_END);
    int file_size = ftell(file);
    fseek(file, SEEK_SET, SEEK_SET);

    return file_size;
}

enum command_list find_command(char* command){
    if (!strcmp(command, "push"))
        return PUSH;
    else 
        return UNKNOWN_COMMAND;
}

enum register_list find_register(char* reg_name){
    if (!strcmp (reg_name, "rax"))
        return RAX;
    else if (!strcmp(reg_name, "rbx"))
        return RBX;
    else if (!strcmp(reg_name, "rcx"))
        return RCX;
    else if (!strcmp(reg_name, "rdx"))
        return RDX;
    else 
        return UNKNOWN_REGISTER;
}

void print_tokens(struct set_of_tokens* set){
    for (int i = 0; i < set->size_of_set; i ++){
        switch (set->set_of_tokens[i].tag){
            case COMMAND:
                printf("COMMAND %d\n", set->set_of_tokens[i].command_code);
            break;
            case MARK:
                printf("MARK: %s\n", set->set_of_tokens[i].mark);
            break;
            case REGISTER:
                printf("REGISTER: %d\n", set->set_of_tokens[i].register_code);
            break;
            case NUMBER:
                printf("NUMBER: %d\n", set->set_of_tokens[i].i_value);
            break;
            case OPEN_SQUARE_BRACKET:
                printf("OPEN_SQUARE_BRAKET\n");
            break;
            case CLOSE_SQUARE_BRACKET:
                printf("CLOSE_SQUARE_BRACKET\n");
            break;
            case OPEN_ROUND_BRACKET:
                printf("OPEN_ROUND_BRACKET\n");
            break;
            case CLOSE_ROUND_BRACKET:
                printf("CLOSE_ROUND_BRACKET\n");
            break;
            case COMMENT:
                printf("COMMENT\n");
            break;
            case NEW_LINE:
                printf ("NEW_LINE\n");
            break;
            case ARITHMETIC_SIGN:
                printf ("ARITHMETIC_SIGN\n");
            break;
        }
    }
}


single_token create_tokens_from_string(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_register_token(struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_comment_token (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_command_token (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_bracket_token (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_number_token  (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_mark_token    (struct parse_ptr* ptr, struct set_of_tokens* set);
bool add_sign_token    (struct parse_ptr* ptr, struct set_of_tokens* set);
void skip_spaces       (struct parse_ptr* ptr);

void create_string   (const char* file_name, char** string);
void destroy_srting  (char* string);


void create_string(const char* file_name, char** string){
    FILE* source_file = fopen(file_name, "r");
    assert (source_file != NULL); //bad create

    int file_size = get_file_size(source_file);
    (*string) = (char*)calloc(file_size, sizeof(char)); //

    fread(*string, sizeof(char), file_size, source_file);
    fclose(source_file);

    return;
}

void destroy_string(char** string){
    free(*string);
    *string = NULL;

    return;
}

void create_set (struct set_of_tokens* set, size_t size){
    single_token* tokens_from_string = (single_token*)calloc(size, sizeof(single_token));

    set->set_of_tokens = tokens_from_string;
    set->size_of_set   = 0;

    return;
}

void destroy_set (struct set_of_tokens* set){
    set->size_of_set = 0;
    free(set->set_of_tokens);

    set->set_of_tokens = NULL;
    return;
}


int main () {

    char* string = NULL;
    create_string("source_file.txt", &string);

    struct set_of_tokens new_set = {};
    create_set(&new_set, strlen(string));


    struct parse_ptr ptr = {};
    ptr.origin = string;
    ptr.offset = string;

    single_token result = {};
    result = create_tokens_from_string(&ptr, &new_set);

    if (result.tag == PARSE_SUCCESS)
        printf ("SUCCESS\n");
    else 
        printf ("ERROR PARSE\n");

    print_tokens(&new_set);


    destroy_srting(&string);
    destroy_set   (&new_set);
    return 0;
}

single_token create_tokens_from_string(struct parse_ptr* ptr, struct set_of_tokens* set) { //in case of error, it returns
                                                                                              //single token with required info to display
    while (*(ptr->offset) != '\0'){
        skip_spaces(ptr);

        // i added 'breaks' to make my code more readable
        if      (add_command_token (ptr, set))  
            continue;
        else if (add_register_token(ptr, set))
            continue;
        else if (add_number_token  (ptr, set))
            continue;
        else if (add_mark_token    (ptr, set))
            continue;
        else if (add_bracket_token (ptr, set))
            continue;
        else if (add_sign_token    (ptr, set))
            continue;
        else if (add_comment_token (ptr, set)) //comment can be only after expression
            break;
        else if (*(ptr->offset) != '\0')
            BAD_TOKEN(ptr);  //aka unknown symbol was met
    }
    SUCCESS_TOKEN(); //in case if everything worked out fine
}


void skip_spaces(struct parse_ptr* ptr){
    while (*(ptr->offset) == ' ' && *(ptr->offset) != '\0'){ //then we are inside the string
        (ptr->offset) ++;
    } 
}


bool add_command_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char command[100] = "";

    int start_command = 0;
    int end_command   = 0;

    sscanf(ptr->offset, "%n%[^# ]%n", &start_command, command, &end_command);
    int command_size = end_command - start_command; 

    //code gen
    enum command_list command_code = find_command(command);

    if (command_code == UNKNOWN_COMMAND)
        return false;
    else 
    {
        ADD_COMMAND_TOKEN(set, command_code);

        (ptr->offset) += command_size;
        return true;
    }
}

bool add_register_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char register_name[3] = "";

    for (int i = 0; i < 3; i ++){
        if (*(ptr->offset) != '\0') // check if we inside
            register_name[i] = (ptr->offset)[i];
    }

    //code gen
    enum register_list register_code = find_register(register_name); 

    if (register_code == UNKNOWN_REGISTER)
        return false;
    else
    {
        ADD_REGISTER_TOKEN(set, register_code);

        (ptr->offset) += 3; //move it for register size
        return true;
    }
}

bool add_number_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    char* old_ptr = ptr->offset;
    int val = 0;

    while ('0' <= *(ptr->offset) && *(ptr->offset) <= '9') {
        val = val*10 + *(ptr->offset) - '0';
        (ptr->offset) ++;
    }

    if (old_ptr == (ptr->offset))
        return false;
    else
    {
        ADD_NUMBER_TOKEN(set, val);

        return true;
    }
} 

bool add_mark_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    if ((*(ptr->offset)) == ':'){
        (ptr->offset) ++;
        skip_spaces(ptr);

        char mark[100] = "";
        int start_mark = 0;
        int end_mark   = 0;

        sscanf(ptr->offset, "%n%[^# ]%n", &start_mark, mark, &end_mark);
        int mark_size = end_mark - start_mark;

        if (mark_size > 10)
            return false;

        if (mark_size == 0)
            return false; //empty mark - syntax error
        else 
        {
            ADD_MARK_TOKEN(set, mark);

            (ptr->offset) += mark_size;
            return true;
        }
    }
    else
        return false; //if there was not found ':' sign at all
}

bool add_sign_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    switch (*(ptr->offset)){
        case '+':
        {
            ADD_SIGN_TOKEN(set, PLUS);

            (ptr->offset) ++;
            return true;
        }
        break;

        case '-':
        {
            ADD_SIGN_TOKEN(set, MINUS);

            (ptr->offset) ++;
            return true;
        }

        break;

        case '*':
        {
            ADD_SIGN_TOKEN(set, MULTIPLY);

            (ptr->offset) ++;
            return true;
        }
        break;

        case '/':
        {
            ADD_SIGN_TOKEN(set, DIVISION);

            (ptr->offset) ++;
            return true;
        }

        case '\n':
        {
            ADD_SIGN_TOKEN(set, NEW_LINE);

            (ptr->offset) ++;
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
    switch (*(ptr->offset)){
        case '(':
        {
            ADD_COMMAND_TOKEN(set, OPEN_ROUND_BRACKET);

            (ptr->offset) ++;
            return true;
        }
        break;

        case ')':
        {
            ADD_COMMAND_TOKEN(set, CLOSE_ROUND_BRACKET);

            (ptr->offset) ++;
            return true;
        }
        break;

        case '[':
        {
            ADD_COMMAND_TOKEN(set, OPEN_SQUARE_BRACKET); 

            (ptr->offset) ++;
            return true;
        }
        break;

        case ']':
        {
            ADD_COMMAND_TOKEN(set, CLOSE_SQUARE_BRACKET);

            (ptr->offset) ++;
            return true;
        }
        break;

        default:
            return false;
        break;
    }

    return false;
}

bool add_comment_token(struct parse_ptr* ptr, struct set_of_tokens* set){
    if (*(ptr->offset) == '#')
    {
        ADD_COMMENT_TOKEN(set);

        (ptr->offset) ++;
        return true;
    }
    return false;
}