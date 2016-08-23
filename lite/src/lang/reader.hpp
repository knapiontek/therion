
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
    void put(int token_id, Ref<Token> token = {0})
    {
        Parse(the_parser, token_id, token, the_tree);
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
        the_map["bool"] = TOK_BOOL;
        the_map["int8"] = TOK_INT8;
        the_map["int16"] = TOK_INT16;
        the_map["int32"] = TOK_INT32;
        the_map["int64"] = TOK_INT64;
        the_map["float32"] = TOK_FLOAT32;
        the_map["float64"] = TOK_FLOAT64;
        the_map["float128"] = TOK_FLOAT128;
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

class Reader
{
public:
    Reader() : the_token_list(0x20)
    {

    }
    bool execute(Tree& tree, io::Decode& decode, core::String filename)
    {
        try
        {
            execute(tree, decode);
            return true;
        }
        catch(SyntaxException)
        {
            core::uint8 ch = 0;
            core::String line;
            while(decode.available())
            {
                decode.read(ch);
                line.append(ch);
            }
            env::Con("syntax error before: '$1'\n$2:$3")
                .arg(line)
                .arg(filename)
                .arg(the_line_cnt)
                .end();
            return false;
        }
    }
private:
    void execute(Tree& tree, io::Decode& decode)
    {
        the_line_cnt = 1;
        the_token_list.erase_all();
        core::uint8 ch = 0;
        Token token;
        Parser parser(tree);

        while(decode.available())
        {
            decode.read(ch);
            start:
            token.erase();
            switch(ch)
            {
            case '#':
                while(ch != '\n')
                    decode.read(ch);
            case '\n':
                the_line_cnt++;
            case ' ': case '\t': case '\f': case '\r':
                break;
            case ':':
                parser.put(TOK_COLON);
                break;
            case '{':
                parser.put(TOK_LP);
                break;
            case '}':
                parser.put(TOK_RP);
                break;
            case '(':
                parser.put(TOK_LB);
                break;
            case ')':
                parser.put(TOK_RB);
                break;
            case '[':
                parser.put(TOK_LS);
                break;
            case ']':
                parser.put(TOK_RS);
                break;
            case '&':
                parser.put(TOK_AND);
                break;
            case '|':
                parser.put(TOK_OR);
                break;
            case '=':
                parser.put(TOK_EQ);
                break;
            case '!':
                parser.put(TOK_NE);
                break;
            case '<':
                decode.read(ch);
                if(ch == '=')
                {
                    parser.put(TOK_LE);
                }
                else
                {
                    parser.put(TOK_LT);
                    goto start;
                }
                break;
            case '>':
                decode.read(ch);
                if(ch == '=')
                {
                    parser.put(TOK_GE);
                }
                else
                {
                    parser.put(TOK_GT);
                    goto start;
                }
                break;
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                while(is_digit(ch))
                {
                    token.attach(ch);
                    decode.read(ch);
                }
                if(ch != '.')
                {
                    parser.put(TOK_INT64, ref(the_token_list.append(token)));
                    goto start;
                }
            case '.':
                decode.read(ch);
                if(!token.size() && !is_digit(ch))
                {
                    parser.put(TOK_DOT);
                    goto start;
                }
                else
                {
                    token.attach('.');
                    while(is_digit(ch))
                    {
                        token.attach(ch);
                        decode.read(ch);
                    }
                    parser.put(TOK_FLOAT, ref(the_token_list.append(token)));
                    goto start;
                }
                break;
            default:
                while(is_id_char(ch))
                {
                    token.attach(ch);
                    decode.read(ch);
                }
                int keyword_id = the_key_map.token(token);
                if(TOK_ID == keyword_id)
                {
                    parser.put(TOK_ID, ref(the_token_list.append(token)));
                }
                else
                {
                    parser.put(keyword_id);
                }
                goto start;
            }
        }
        parser.put(0);
    }
    /**
     * If X is a character that can be used in an identifier then
     * is_id_char[X] will be true. Otherwise is_id_char[X] will be false.
     *
     * In this implementation, an identifier can be a string of
     * alphabetic characters, digits, and "_" plus any character
     * with the high-order bit set. The latter rule means that
     * any sequence of UTF-8 characters or characters taken from
     * an extended ISO8859 character set can form an identifier.
     */
    static bool is_id_char(int ch)
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
    static bool is_digit(int ch)
    {
        return ('0' <= ch && ch <= '9');
    }
private:
    core::List<Token> the_token_list;
    KeywordMap the_key_map;
    int the_line_cnt;
};
