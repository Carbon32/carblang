class Parser
{
    struct ParseError : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    const std::vector<Token> &tokens;
    int current = 0;

    public:
        Parser(const std::vector<Token> &tokens);

        std::vector<std::shared_ptr<Stmt>> parse();

    private:
        std::shared_ptr<Expression> expression();
        std::shared_ptr<Stmt> declaration();
        std::shared_ptr<Stmt> statement();

        std::shared_ptr<Stmt> print_statement();
        std::shared_ptr<Stmt> println_statement();
        std::shared_ptr<Stmt> for_statement();
        std::shared_ptr<Stmt> if_statement();
        std::shared_ptr<Stmt> while_statement();
        std::shared_ptr<Stmt> var_declaration();
        std::shared_ptr<Stmt> function_declaration();
        std::shared_ptr<Stmt> expression_statement();
        std::shared_ptr<Stmt> return_statement();
        std::shared_ptr<Stmt> include_statement();

        std::vector<std::shared_ptr<Stmt>> block();
        std::shared_ptr<Expression> assignment();
        std::shared_ptr<Expression> and_expression();
        std::shared_ptr<Expression> or_expression();
        std::shared_ptr<Expression> equality();
        std::shared_ptr<Expression> comparison();
        std::shared_ptr<Expression> term();
        std::shared_ptr<Expression> factor();
        std::shared_ptr<Expression> unary();
        std::shared_ptr<Expression> primary();
        std::shared_ptr<Expression> call();
        std::shared_ptr<Expression> finish_call(std::shared_ptr<Expression> callee);

        template<class...T>
        bool match(T...type);

        Token consume(TokenType type, std::string message);
        bool check(TokenType type);
        Token advance();
        bool at_end();
        Token peek();
        Token previous();
        ParseError error(const Token &token, std::string message);
        void synchronize();
};