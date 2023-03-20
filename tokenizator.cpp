#include <iostream>
#include <stdlib.h>


char commands[] = (
    "push",
    "pop",
    "mul",
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
    MARK_DECLARATION     = 1,
    MARK_CALL            = 2,
    REGISTER             = 3,
    NUMBER               = 4,
    OPEN_SQUARE_BRACKET  = 5,
    CLOSE_SQUARE_BRACKET = 6,
    OPEN_ROUND_BRACKET   = 7,
    CLOSE_ROUND_BRACKET  = 8,
    COMMENT              = 9,
    ARITHMETIC_SIGN      = 10,
    NOT_A_COMMAND        = 11
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


typedef struct _token
{
    enum command_tags tag;
    enum command_list command_code;
    int i_value;            //value if its number
    int register_code;      // rax - 0, rbx - 1, rcx - 2, rdx - 3
    int arithmetic_code;    // '+' - 0, '-' - 1, '*' - 2, '/' - 3
    int in_line_position;   // position in the source text line
    char mark[10] = "";     //mark name
}single_token;


//thats great, TASK - write a solid tokenizator :)
//BEFORE write a code - think about its logic

int main ()
{
    return 0;
}