
class Parser
{
public:
    Parser(Tree& tree) : the_tree(tree)
    {
        the_parser = ParseAlloc(::malloc);
    }
    ~Parser()
    {
        ParseFree(the_parser, ::free);
    }
    void put(int token_id, Ret<Token> token = {0})
    {
        Parse(the_parser, token_id, token, &the_tree);
    }
private:
    Tree& the_tree;
    void* the_parser;
};

class KeywordMap
{
public:
    KeywordMap() : the_map(0xF)
    {
        the_map["this"] = TOK_THIS;
        the_map["that"] = TOK_THAT;
        the_map["in"] = TOK_IN;
        the_map["out"] = TOK_OUT;
        the_map["if"] = TOK_IF;
        the_map["for"] = TOK_FOR;
        the_map["repeat"] = TOK_REPEAT;
        the_map["until"] = TOK_UNTIL;
    }
    core::int64 token(core::String& key)
    {
        auto value = the_map.lookup(key);
        return value != core::nil
            ? core::int64(value)
            : TOK_ID;
    }
private:
    core::HashMap<core::String, core::int64> the_map;
};

class Decode
{
public:
    Decode(io::Input& input) : the_input(input)
    {

    }
    void read(core::uint8& arg)
    {
        if(the_input.available())
            the_input.input(&arg, sizeof(arg));
        else
            arg = 0;
    }
private:
    io::Input& the_input;
};

class Reader
{
public:
    static void execute(Tree& tree, Decode& decode, core::String& filename)
    {
        core::int64 line_cnt;
        try
        {
            execute(tree, decode, line_cnt);
        }
        catch(SyntaxException)
        {
            throw_exception(decode, filename, line_cnt);
        }
    }
private:
    static void execute(Tree& tree, Decode& decode, core::int64& line_cnt)
    {
        line_cnt = 1;
        auto space_cnt = 0;
        core::uint8 ch = 0;
        Token token;
        Parser parser(tree);
        KeywordMap key_map;
        core::List<Token> token_list(0x20);

start:
        decode.read(ch);
start_erase:
        token.erase();
start_switch:
        switch(ch)
        {
        case '\0':
            parser.put(0);
            return;
        case '#':
            while('\0' != ch && '\n' != ch)
                decode.read(ch);
            goto start_switch;
        case '\n':
            line_cnt++;
            decode.read(ch);
            if('\0' == ch || '\n' == ch)
                goto start_switch;
            while(' ' == ch)
            {
                token.attach(ch);
                decode.read(ch);
            }
            parser.put(TOK_IND, ret(token_list.append(token)));
            goto start_erase;
        case ' ':
            space_cnt++;
            goto start;
        case ':':
            parser.put(TOK_COLON);
            goto start;
        case '{':
            parser.put(TOK_LP);
            goto start;
        case '}':
            parser.put(TOK_RP);
            goto start;
        case '(':
            parser.put(TOK_LB);
            goto start;
        case ')':
            parser.put(TOK_RB);
            goto start;
        case '[':
            parser.put(TOK_LS);
            goto start;
        case ']':
            parser.put(TOK_RS);
            goto start;
        case '*':
            parser.put(TOK_MUL);
            goto start;
        case '/':
            parser.put(TOK_DIV);
            goto start;
        case '%':
            parser.put(TOK_MOD);
            goto start;
        case '+':
            parser.put(TOK_ADD);
            goto start;
        case '-':
            decode.read(ch);
            if(' ' == ch)
                parser.put(TOK_SUB);
            else if(is_digit(ch))
                token.attach('-');
            else
                throw SyntaxException();
            goto start_switch;
        case '=':
            parser.put(TOK_EQ);
            goto start;
        case '<':
            decode.read(ch);
            if('=' == ch)
                parser.put(TOK_LE);
            else if('>' == ch)
                parser.put(TOK_NE);
            else if('<' == ch)
                parser.put(TOK_SHL);
            else
            {
                parser.put(TOK_LT);
                goto start_switch;
            }
            goto start;
        case '>':
            decode.read(ch);
            if('=' == ch)
                parser.put(TOK_GE);
            else if('>' == ch)
                parser.put(TOK_SHR);
            else
            {
                parser.put(TOK_GT);
                goto start_switch;
            }
            goto start;
        case '&':
            parser.put(TOK_AND);
            goto start;
        case '|':
            parser.put(TOK_OR);
            goto start;
        case '^':
            parser.put(TOK_XOR);
            goto start;
        case '!':
            parser.put(TOK_NOT);
            goto start;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
            do
            {
                token.attach(ch);
                decode.read(ch);
            }
            while(is_digit(ch));
            if('i' == ch)
            {
                core::String width;
                do
                {
                    width.attach(ch);
                    decode.read(ch);
                }
                while(is_digit(ch));
                if(width.equal("i8"))
                    parser.put(TOK_INT8, ret(token_list.append(token)));
                else if(width.equal("i16"))
                    parser.put(TOK_INT16, ret(token_list.append(token)));
                else if(width.equal("i32"))
                    parser.put(TOK_INT32, ret(token_list.append(token)));
                else if(width.equal("i64"))
                    parser.put(TOK_INT64, ret(token_list.append(token)));
                else
                    throw SyntaxException();
                goto start_erase;
            }
            if('.' != ch)
            {
                parser.put(TOK_INT32, ret(token_list.append(token)));
                goto start_erase;
            }
        case '.':
            decode.read(ch);
            if(!is_digit(ch) && core::nil == token)
            {
                parser.put(TOK_DOT);
                goto start_switch;
            }
            token.attach('.');
            while(is_digit(ch))
            {
                token.attach(ch);
                decode.read(ch);
            }
            if('e' == ch)
            {
                token.attach(ch);
                decode.read(ch);
                if('-' == ch || '+' == ch)
                {
                    token.attach(ch);
                    decode.read(ch);
                }
                while(is_digit(ch))
                {
                    token.attach(ch);
                    decode.read(ch);
                }
            }
            if('f' == ch)
            {
                core::String width;
                do
                {
                    width.attach(ch);
                    decode.read(ch);
                }
                while(is_digit(ch));
                if(width.equal("f32"))
                    parser.put(TOK_FLOAT32, ret(token_list.append(token)));
                else if(width.equal("f64"))
                    parser.put(TOK_FLOAT64, ret(token_list.append(token)));
                else
                    throw SyntaxException();
                goto start_erase;
            }
            parser.put(TOK_FLOAT64, ret(token_list.append(token)));
            goto start_erase;
        default:
            if(!is_id(ch))
                throw SyntaxException();
            do
            {
                token.attach(ch);
                decode.read(ch);
            }
            while(is_id(ch));
            auto keyword_id = key_map.token(token);
            if(TOK_ID == keyword_id)
            {
                parser.put(TOK_ID, ret(token_list.append(token)));
            }
            else
            {
                parser.put(keyword_id);
            }
            goto start_erase;
        }
    }
    static void throw_exception(Decode& decode, core::String& filename, core::int64 line_cnt)
    {
        core::uint8 ch = 0;
        core::String line;
        while(true)
        {
            decode.read(ch);
            if('\n' == ch || '\0' == ch)
            {
                throw env::Format("syntax error before: '%1'\n\tlocation: %2:%3")
                    % line
                    % filename
                    % line_cnt
                    % env::exception;
            }
            line.attach(ch);
        }
    }
    /**
     * If X is a character that can be used in an identifier then
     * is_id[X] will be true. Otherwise is_id[X] will be false.
     *
     * In this implementation, an identifier can be a string of
     * alphabetic characters, digits, and "_" plus any character
     * with the high-order bit set. The latter rule means that
     * any sequence of UTF-8 characters or characters taken from
     * an extended ISO8859 character set can form an identifier.
     */
    static bool is_id(core::uint8 ch)
    {
        static const bool chars[] =
        {
            /* x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF */
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0x */
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 1x */
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 2x */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,    /* 3x */
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* 4x */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1,    /* 5x */
            0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* 6x */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,    /* 7x */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* 8x */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* 9x */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* Ax */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* Bx */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* Cx */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* Dx */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* Ex */
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,    /* Fx */
        };
        return chars[ch];
    }
    static bool is_digit(core::uint8 ch)
    {
        return ('0' <= ch && ch <= '9');
    }
};
