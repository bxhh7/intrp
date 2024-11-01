#include <cassert>
#include <cctype>
#include <cstdlib>
#include "Lexer.h"


Lexer::Lexer(std::string buffer):
	m_buffer {std::move(buffer)}
{

}

Token Lexer::match(const std::initializer_list<TokenType>& l)
{
	for (const TokenType type : l)
	{
		Token tok;
		if ((tok = match(type)))
			return tok;
	}
	return Token{TOK_NONE};
}

Token Lexer::match(TokenType type)
{
	Token tok = next();
	if (tok.type() == type)
	{
		return tok;
	}
	else
	{
		m_undo(tok);
		return Token{TOK_NONE};
	}
}

void Lexer::m_undo(Token tok)
{
	assert(!m_temp && "Lexer::m_undo() a token is already in the mem.");
	m_temp = tok;
}

Token Lexer::peek()
{
	Token tok = next();
	m_undo(tok);
	return tok;
}

uint64_t Lexer::m_skip_while(Lexer::Predicate p)
{
	uint64_t offt = 0;
	while (m_cursor < m_buffer.size() && p(m_buffer[m_cursor]))
	{
		offt++;
		m_cursor++;
	}
	return offt;
}

Token Lexer::lex_while(TokenType type, Predicate p)
{
	uint64_t begin = m_cursor - 1;
	uint64_t len = m_skip_while(p) + 1;
	std::string lexeme = m_buffer.substr(begin, len);
	Token tok {type, lexeme};
	return tok;
}

Token Lexer::next()
{
	if (m_temp)
	{
		Token ret = std::move(m_temp);
		m_temp = Token{TOK_NONE};
		return ret;
	}

	Token eof{TOK_EOF}, none {TOK_NONE};
	
	m_skip_while(is_space);

	char c = _next();
	if (!c)
		return eof;
	while (c && c == '\n')
		c = _next();
	
	Token ident_keyword_tok;

	switch(c)
	{
		case '0':case '1':case '2': case '3': case '4':case '5':case '6':case'7': case '8': case '9': 
			{
				Token tok = lex_while(TOK_NUMBER, is_numeric);
				tok.set_value((int64_t)std::stod(tok.lexeme()));
				return tok;
			}
			break;
		case '(': 
			if (_match(')'))
			{
				return Token{TOK_CALL, "()"};
			}

			return Token{TOK_OPAREN, "("}; break;
		case ')': return Token{TOK_CPAREN, ")"}; break;
		case '{': return Token{TOK_OCURLY, "{"}; break;
		case '}': return Token{TOK_CCURLY, "}"}; break;

		case '+': return Token{TOK_PLUS, "+"}; break;
		case '-': return Token{TOK_MINUS, "-"}; break;
		case '/': return Token{TOK_SLASH, "/"}; break;
		case '*': return Token{TOK_STAR, "*"}; break;
		case ',': return Token{TOK_COMMA, ","}; break;
		case '>': 
				  if (_match('='))
					  return Token{TOK_GE, ">="};
			return Token{TOK_GT, ">"}; 
			break;
		
		case '<': 
				  if (_match('='))
					  return Token{TOK_LE, "<="};
			return Token{TOK_LT, "<"}; 
			break;

		case '!':
			if (_match('='))
				return Token{TOK_INEQ, "!="};
			return Token{TOK_LNOT};

		case '=':
			if (_match('='))
				return Token {TOK_EQ, "=="};
			return Token{TOK_ASSIGN, "="};
			break;

			/* Identifiers and keywords */
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': 
		case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
		case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': 
		case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
		case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case '_':
				  {
					  /* TODO: refactor this with case 'NUMBERS' */
					  ident_keyword_tok = lex_while(TOK_IDENTIFIER, is_identifier);
					  ident_keyword_tok.set_value(ident_keyword_tok.lexeme());
				  } break;
		default:
				  fprintf(stderr, "unknown token: %c\n", c);
				  exit(EXIT_FAILURE);
	}

	detect_keyword(ident_keyword_tok);
	return ident_keyword_tok;
}

void Lexer::detect_keyword(Token& tok)
{
	/* TODO: make this a hashtable or something */
	if (tok.lexeme() == "and")
	{
		tok.set_type(TOK_LAND);
	}
	if (tok.lexeme() == "or")
	{
		tok.set_type(TOK_LOR);
	}
	if (tok.lexeme() == "taut")
	{
		tok.set_type(TOK_TAUT);
	}
	if (tok.lexeme() == "nil")
	{
		tok.set_type(TOK_NIL);
	}
	if (tok.lexeme() == "if")
	{
		tok.set_type(TOK_IF);
	}
	if (tok.lexeme() == "else")
	{
		tok.set_type(TOK_ELSE);
	}
	if (tok.lexeme() == "while")
	{
		tok.set_type(TOK_WHILE);
	}
	if (tok.lexeme() == "print")
	{
		tok.set_type(TOK_PRINT);
	}
}
bool Lexer::_match(char x)
{
	if (_peek() == x) {
		m_cursor++;
		return true;
	}
	return false;
}

bool Lexer::_match(const std::string& str)
{
	if (m_buffer.compare(m_cursor, str.size(), str) == 0)
	{
		m_cursor += str.size();
		return true;
	}
	return false;
}
char Lexer::_peek()
{
	if (eof())
	{
		return '\0';
	}
	return m_buffer[m_cursor];
}

char Lexer::_peek(uint8_t n)
{
	if (eof())
	{
		return '\0';
	}
	if (m_cursor + n < m_buffer.size())
		return '\0';
	return m_buffer[m_cursor + n];
}

void Lexer::_skip(uint8_t n)
{
	if (m_cursor + n < m_buffer.size())
	{
		m_cursor = m_buffer.size();
	}
	m_cursor += n;
}


char Lexer::_next()
{
	if (eof())
	{
		return '\0';
	}
	return m_buffer[m_cursor++];
}

bool Lexer::eof() 
{
	while (m_cursor < m_buffer.size()) 
	{
		if (m_buffer[m_cursor] == ' ' || m_buffer[m_cursor] == '\n' || m_buffer[m_cursor] == '\t')
			m_cursor++;
		else
			break;
	}
	if (m_cursor >= m_buffer.size())
		return true;
	return false;
}


bool Lexer::is_space(char x)
{
	return std::isspace(x);
}

bool Lexer::is_alphabetic(char x)
{
	return std::isalpha(x);
}

bool Lexer::is_identifier(char x)
{
	return (std::isalnum(x) || x == '_')&& !isspace(x);
}
bool Lexer::is_numeric(char x)
{
	return (x >= '0' && x <= '9');
}

void Lexer::set_buffer(std::string buf)
{
	m_buffer = std::move(buf);
}

Token::Token(TokenType type)
	:m_type { type }
{}


const TokenType Token::type() const
{
	return m_type;
}

const TokenValue Token::value() const
{
	return m_value;
}

Token::operator bool()
{
	return !(m_type == TOK_NONE || m_type == TOK_EOF);
}

std::string Token::printable() const
{
	/* {TOK_TYPE, TOK_LEXEME, TOK_VALUE, X, Y} */
	std::string ret = "";
	std::string val;
	std::visit(
			overloaded
			{
				[&](const int64_t i) { val = std::to_string(i); },
				[&](const std::string& s) { val = s; }
			}, m_value
			);
	ret += "{" + type_str_map[m_type] + ", " + m_lexeme +  ", " + val + "}";
	return ret;
}

Token:: Token(TokenType type, const std::string& lexeme):
	m_type {type}, m_lexeme {lexeme}
{}

Token::Token(const TokenType type, TokenValue val, const std::string& lexeme):
	m_type {type}, m_value {val}, m_lexeme {lexeme} 
{}

void Token::set_lexeme(std::string str)
{
	m_lexeme = std::move(str);
}

void Token::set_coordinates(uint64_t x, uint64_t y)
{
	m_x = x, m_y = y;
}

void Token::set_value(TokenValue val)
{
	m_value = val;
}

void Token::set_type(TokenType type)
{
	m_type = type;
}

const std::string Token::lexeme() const
{
	return m_lexeme;
}


std::unordered_map<TokenType, std::string> Token::type_str_map = 
{
	{TOK_NUMBER, 			"TOK_NUMBER" },
	{TOK_OPAREN,            "TOK_OPAREN"},
	{TOK_CPAREN,            "TOK_CPAREN"},
	{TOK_PLUS,              "TOK_PLUS"},
	{TOK_MINUS,             "TOK_MINUS"},
	{TOK_STAR,              "TOK_STAR"},
	{TOK_GT,             	"TOK_GT"},
	{TOK_LT,             	"TOK_LT"},
	{TOK_LE,				"TOK_LE"},
	{TOK_GE,				"TOK_GE"},
	{TOK_EQ,				"TOK_EQ"},
	{TOK_INEQ,				"TOK_INEQ"},
	{TOK_LNOT,				"TOK_LNOT"},
	{TOK_LAND, 				"TOK_LAND"},
	{TOK_LOR,				"TOK_LOR"},
	{TOK_ASSIGN,			"TOK_ASSIGN"},
	{TOK_SLASH,             "TOK_SLASH"},
	{TOK_COMMA,				"TOK_COMMA"},
	{TOK_IDENTIFIER,        "TOK_IDENTIFIER",},
	{TOK_TAUT, 				"TOK_TAUT"},
	{TOK_NIL,				"TOK_NIL"},
	{TOK_EOF,               "TOK_EOF"},
	{TOK_NONE,              "TOK_NONE"},
	{TOK_COUNT,             "TOK_COUNT"},
	{TOK_WHILE,             "TOK_WHILE"},
	{TOK_IF,             "TOK_IF"},
	{TOK_CALL,             "TOK_CALL"},

};
