enum class OpCode : uint8_t
{
    CONSTANT,

    NULL,
    TRUE,
    FALSE,

    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,

    NOT,
    NEGATE,

    EQUAL,
    GREATER,
    LESS,

    PRINT,
    PRINTLN,

    POP,

    DEFINE_GLOBAL,
    GET_GLOBAL,
    SET_GLOBAL,

    GET_LOCAL,
    SET_LOCAL,

    JUMP,
    JUMP_IF_TRUE,
    JUMP_IF_FALSE,
    LOOP,

    CALL,
    CLOSURE,

    ARRAY,
    GET_INDEX,
    SET_INDEX,
    GET_PROPERTY,

    RETURN
};