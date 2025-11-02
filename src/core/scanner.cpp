#include "core/core.hpp"

Scanner::Scanner(std::string src)
{
	this->src = src;
}

const std::map<std::string, TokenType> Scanner::keywords =
{
	{"and",    	  	TokenType::AND},
	{"class",  	  	TokenType::CLASS},
	{"else",        TokenType::ELSE},
	{"false",       TokenType::FALSE},
	{"for",         TokenType::FOR},
	{"function",    TokenType::FUNCTION},
	{"if",     	  	TokenType::IF},
	{"null",    	TokenType::NULL},
	{"or",     	  	TokenType::OR},
	{"print",  	  	TokenType::PRINT},
	{"return", 	  	TokenType::RETURN},
	{"super",  	  	TokenType::SUPER},
	{"this",   	 	TokenType::THIS},
	{"true",   	  	TokenType::TRUE},
	{"var",    	  	TokenType::VAR},
	{"while",  	  	TokenType::WHILE},
};

std::vector<Token> Scanner::scan_tokens()
{
	while(!at_end())
	{
		this->start = current;
		this->scan_single_token();
	}

	tokens.emplace_back(END_OF_FILE, "", nullptr, this->line);
	return tokens;
}

void Scanner::scan_single_token()
{
	char c = advance();
	switch (c)
	{
		case '(': this->add_token(LEFT_PAREN); break;
		case ')': this->add_token(RIGHT_PAREN); break;
		case '{': this->add_token(LEFT_BRACE); break;
		case '}': this->add_token(RIGHT_BRACE); break;
		case ',': this->add_token(COMMA); break;
		case '.': this->add_token(DOT); break;
		case '-': this->add_token(MINUS); break;
		case '+': this->add_token(PLUS); break;
		case ';': this->add_token(SEMICOLON); break;
		case '*': this->add_token(STAR); break;
		case '!':
			this->add_token(this->match('=') ? BANG_EQUAL : BANG);
			break;
		case '=':
			this->add_token(this->match('=') ? EQUAL_EQUAL : EQUAL);
			break;
		case '<':
			this->add_token(this->match('=') ? LESS_EQUAL : LESS);
			break;
		case '>':
			this->add_token(this->match('=') ? GREATER_EQUAL : GREATER);
			break;

		case '/':
			if(this->match('/'))
			{
			  while(peek() != '\n' && !this->at_end()) advance();
			}
			else
			{
			  this->add_token(SLASH);
			}
			break;

		case ' ':
		case '\r':
		case '\t':
			break;

		case '\n':
			++this->line;
			break;

		case '"': this->string(); break;

		default:
			if(this->is_digit(c))
			{
			  this->number();
			}
			else if(this->is_alpha(c))
			{
			  this->identifier();
			}
			else
			{
			  error(line, "Unexpected character.");
			}
			break;
	};
}

void Scanner::identifier()
{
	while(this->is_alpha_numeric(this->peek()))
	{
		this->advance();
	}

	std::string text = this->src.substr(this->start, this->current - this->start);

	TokenType type;
	auto match = this->keywords.find(text);
	if(match == this->keywords.end())
	{
		type = IDENTIFIER;	
	}
	else
	{
		type = match->second;
	}

	this->add_token(type);
}

void Scanner::number()
{
	while(this->is_digit(this->peek())) this->advance();

	if(this->peek() == '.' && this->is_digit(this->peek_next()))
	{
		this->advance();
		while(this->is_digit(this->peek())) this->advance();
	}

	this->add_token(NUMBER, std::stod(this->src.substr(this->start, this->current - this->start)));
  }

void Scanner::string()
{
	while(this->peek() != '"' && !this->at_end())
	{
		if(this->peek() == '\n') ++this->line;
		this->advance();
	}

	if(this->at_end())
	{
	  error(this->line, "Unterminated string");
	  return;
	}

	this->advance();

	std::string value = this->src.substr(this->start + 1, this->current - 2 - this->start);
	this->add_token(STRING, value);
}

bool Scanner::match(char expected)
{
	if (this->at_end()) return false;
	if (this->src[this->current] != expected) return false;

	++this->current;
	return true;
}

char Scanner::peek()
{
	if(this->at_end()) return '\0';
	return this->src[this->current];
}

char Scanner::peek_next()
{
	if(this->current + 1 >= this->src.length()) return '\0';
	return this->src[this->current + 1];
}

bool Scanner::is_alpha(char c)
{
	return (c >= 'a' && c <= 'z') ||
		   (c >= 'A' && c <= 'Z') ||
			c == '_';
}

bool Scanner::is_alpha_numeric(char c)
{
	return this->is_alpha(c) || this->is_digit(c);
}

bool Scanner::is_digit(char c)
{
	return c >= '0' && c <= '9';
}

bool Scanner::at_end()
{
	return this->current >= this->src.length();
}

char Scanner::advance() {
	return this->src[this->current++];
}

void Scanner::add_token(TokenType type)
{
	this->add_token(type, std::any());
}

void Scanner::add_token(TokenType type, std::any literal)
{
	std::string text = this->src.substr(this->start, this->current - this->start);
	this->tokens.emplace_back(type, std::move(text), std::move(literal), this->line);
}