#pragma once

#include <string>
#include <cstdint>
#include <variant>
#include <unordered_map>
#include <functional>

enum TokenType
{
	TOK_NUMBER,
	TOK_STRING,
	TOK_OPAREN,
	TOK_CPAREN,
	TOK_PLUS,
	TOK_MINUS,
	TOK_STAR,
	TOK_SLASH,
	TOK_IDENTIFIER,
	TOK_LT, /* Less than '<' */
	TOK_GT, /* Greater Than '>' */
	TOK_LE, /* <=*/
	TOK_GE, /* >= */
	TOK_EQ, /* Equality '==' */
	TOK_LNOT, /* Logical Not ! */
	TOK_INEQ, /* Inequality '!=' */
	TOK_ASSIGN, /* Assign '=' */
	TOK_LOR, /* Logical Or 'or'*/
	TOK_LAND, /* Logical And 'and'*/
	TOK_COMMA,
	TOK_OCURLY,
	TOK_CCURLY,
	TOK_CALL,
	TOK_TAUT, 
	TOK_NIL,

	TOK_IF,
	TOK_ELSE,
	TOK_WHILE,
	TOK_PRINT,
	TOK_RETURN,

	TOK_EOF,
	TOK_NONE, 

	TOK_COUNT,
};

using TokenValue = std::variant<std::string, int64_t>;


/* https://en.cppreference.com/w/cpp/utility/variant/visit */
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


class Token
{
	public:
		Token(){};
		Token(TokenType);
		Token(TokenType, const std::string& lexeme);
		Token(const TokenType type, TokenValue val, const std::string& lexeme);
		
		const std::string lexeme() const;

		void set_lexeme(std::string);

		void set_coordinates(uint64_t x, uint64_t y);
		
		void set_value(TokenValue val);

		void set_type(TokenType type);

		const TokenType type() const;
		
		const TokenValue value() const;
		
		operator bool();
		
		std::string printable() const;

		static std::unordered_map<TokenType, std::string> type_str_map;
	
	private:
		std::string m_lexeme;
		TokenValue m_value;
		TokenType m_type;
		uint64_t m_x;
		uint64_t m_y;

};


class Lexer
{
	public:
		using Predicate = std::function<bool(char)>;
		
		Lexer(std::string);
		Lexer() {};
	
		
		void set_buffer(std::string);

		Token match(TokenType);
		Token match(const std::initializer_list<TokenType>&);

		Token peek() /* TODO: const */;
		
		Token next();

		bool eof();
	
		/* Predicates */
		static bool is_space(char) ;
		static bool is_alphabetic(char);
		static bool is_numeric(char);
		static bool is_identifier(char x);
		static bool is_not_string_end(char x); /* is not " */

		static void detect_keyword(Token& tok);
	private:
		char _peek();
		char _peek(uint8_t n); /* peek n chars ahead.*/
		char _next();
		void _skip(uint8_t n);
		bool _match(const std::string& str);
		bool _match(char x);
		uint64_t m_skip_while(Predicate);

		
		/* returns a token of t, with the lexeme determined by the predicate*/
		Token lex_while(TokenType t, Predicate, bool inclusive = false);
		
		void m_undo(Token);
		Token m_temp {TOK_NONE}; 
		size_t m_cursor {0};
		std::string m_buffer;
};
