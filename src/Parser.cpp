#include <memory>
#include <cassert>
#include <exception>
#include <stdexcept>
// #include <runtime_error>

#include "Parser.h"
#include "Lexer.h"

std::vector<ASTNodePtr> Parser::parse()
{
	std::vector<ASTNodePtr> ret;
	while (!m_lexer.eof())
	{
		ret.push_back(stmt());
	}
	return ret;
}

ASTNodePtr Parser::stmt()
{
	return decl();
}

ASTNodePtr Parser::decl()
{
	return vardef();
}

ASTNodePtr Parser::vardef()
{
	if (m_lexer.match(TOK_VAR))
	{
		/* This is a variable declaration/definition*/
		std::shared_ptr<VarDefExpr> ret = std::make_shared<VarDefExpr>();
		if (m_lexer.peek().type() == TOK_IDENTIFIER) {
			ret->identifier = m_lexer.next();
		} else
		{
			std::cerr << "TEMPORARY : Error: expected identifier after var.\n";
			throw std::runtime_error{""};			
		}
		
		if (m_lexer.match(TOK_ASSIGN))
		{
			ret->rhs = expression(); 
			return ret;
		} else{
			return ret;
		}
	} else
	{
		return func();
	}
}

ASTNodePtr Parser::func()
{
	ASTNodePtr lhs = ret();
	if (m_lexer.match(TOK_CALL))
	{
		if (!std::dynamic_pointer_cast<IdentifierExpr>(lhs)) /* TODO
																	  for now we don't support call chaining ()() */
		{
			expect("expected identifier before ()");
		}
		
		std::shared_ptr<IdentifierExpr> fname = std::dynamic_pointer_cast<IdentifierExpr>(lhs);

		if (m_lexer.peek().type() == TOK_OCURLY)
		{
			/* FUNCTION DECLARATION */
			FuncDeclExpr ret;
			ASTNodePtr body = block();
			if (!std::dynamic_pointer_cast<BlockExpr>(body))
			{
				expect("Expected a body for the function.");
			}

			ret.body = std::dynamic_pointer_cast<BlockExpr>(body);
			//ret.parameters; TODO; we don't support parameters for now

			ret.name = fname->tok;

			return std::make_shared<FuncDeclExpr>(ret);
		}
		else
		{
			/* It is a function call */
			FuncCallExpr ret;
			ret.name = fname->tok;
			// ret.arguments; TODO not supported for now
			return std::make_shared<FuncCallExpr>(ret);
		}
	}
	return lhs;
}

ASTNodePtr Parser::ret()
{
	if (m_lexer.match(TOK_RETURN))
	{

		std::shared_ptr<ReturnExpr> re = std::make_shared<ReturnExpr>();
		re->expr = expression();
		return re;
	}
	else
	{
		return print();
	}
}
/* A block is a vector of stmts */
ASTNodePtr Parser::block()
{
	BlockExpr ret;
	if (m_lexer.match(TOK_OCURLY))
	{
		while (m_lexer.peek().type() != TOK_CCURLY)
		{
			ret.body.push_back(stmt());
		}

		if (!m_lexer.match(TOK_CCURLY))
		{
			expect("expected '}' at the end of the block.");
		}
		return std::make_shared<BlockExpr>(ret);
	}
	return print();

}

ASTNodePtr Parser::print()
{
	PrintExpr ret;
	if (m_lexer.match(TOK_PRINT))
	{
		ret.expr = this->expression();
		return std::make_shared<PrintExpr>(ret);
	}
	return condition();
}

ASTNodePtr Parser::condition() /* if (expr) {expr...} */
{
	IfExpr ret;
	if (m_lexer.match(TOK_IF))
	{
		if (!m_lexer.match(TOK_OPAREN))
			expect("expeceted '('");
		
		ret.condition = assignment();

		if (!m_lexer.match(TOK_CPAREN))
			expect("expected ')'");
		
		if (!m_lexer.match(TOK_OCURLY))
			expect("expected {");

		while (m_lexer.peek().type() != TOK_CCURLY) 
		{
			ret.body.push_back(stmt());
		}

		if (!m_lexer.match(TOK_CCURLY))
		{
			expect("expected }");
		}

		if (m_lexer.match(TOK_ELSE))
		{
			if (!m_lexer.match(TOK_OCURLY))
				expect("expected {");
			while (m_lexer.peek().type() != TOK_CCURLY) 
			{
				ret.else_cluse.push_back(stmt());
			}
			if (!m_lexer.match(TOK_CCURLY))
			{
				expect("expected }");
			}
		}
		return std::make_shared<IfExpr>(ret);
	}
	else 
		return loop();
}

ASTNodePtr Parser::loop()
{
	WhileExpr ret;
	if (m_lexer.match(TOK_WHILE))
	{
		if (!m_lexer.match(TOK_OPAREN))
			expect("expeceted '('");

		ret.condition = assignment();

		if (!m_lexer.match(TOK_CPAREN))
			expect("expected ')'");

		if (!m_lexer.match(TOK_OCURLY))
			expect("expected {");

		while (m_lexer.peek().type() != TOK_CCURLY) 
		{
			ret.body.push_back(stmt());
		}

		if (!m_lexer.match(TOK_CCURLY))
		{
			expect("expected }");
		}

		return std::make_shared<WhileExpr>(ret);
	}
	else 
		return assignment();

}

ASTNodePtr Parser::expression()
{
	return assignment();
}

/* 
assignment     → IDENTIFIER "=" assignment
               | logic_or ;
*/
ASTNodePtr Parser::assignment()
{
	Token t;
	ASTNodePtr rhs;
	ASTNodePtr lhs;
	ASTNodePtr ret;


	lhs = logic_or();
	if (m_lexer.match(TOK_ASSIGN))
	{
		rhs = logic_or();
		return std::make_shared<AssignmentExpr>(lhs, TOK_ASSIGN, rhs);
	}
	return lhs;

	if ((t = m_lexer.match(TOK_IDENTIFIER)) && m_lexer.match(TOK_ASSIGN))
	{
		ASTNodePtr lhs = std::make_shared<IdentifierExpr>(t);
		rhs = assignment();
		//rhs = expression();
		return std::make_shared<AssignmentExpr>(lhs, TOK_ASSIGN, rhs);
	}
	else 
	{
		return logic_or();
	}
}

/* logic_or       → logic_and ( "or" logic_and )* ; */
ASTNodePtr Parser::logic_or()
{
	ASTNodePtr lhs = logic_and();
	ASTNodePtr rhs;
	ASTNodePtr ret = lhs; /* LOR is left-assosicative */

	/* This ain't really a left-recursion issue. I think i can easily recurse */
	while (m_lexer.match(TOK_LOR))
	{
		rhs = logic_and();
		ret = std::make_shared<BinaryExpr>(ret, TOK_LOR, rhs);
	}
	return ret;
}

/* logic_and      → equality ( "and" equality )* ; */
ASTNodePtr Parser::logic_and()
{
	ASTNodePtr lhs = equality();
	ASTNodePtr rhs;
	ASTNodePtr ret = lhs; /* LOR is left-assosicative */

	/* TODO: use while (match()) instead of recursing */
	/* This ain't really a left-recursion issue. I think i can easily recurse */
	while (m_lexer.match(TOK_LAND))
	{
		rhs = equality();
		ret = std::make_shared<BinaryExpr>(ret, TOK_LAND, rhs);
	}
	return ret;

}

/* equality       → comparison ( ( "!=" | "==" ) comparison )* ; */
ASTNodePtr Parser::equality()
{
	ASTNodePtr lhs = comparison();
	ASTNodePtr rhs;
	ASTNodePtr ret = lhs; /* LOR is left-assosicative */
	
	Token op;
	while ((op = m_lexer.match({TOK_EQ, TOK_INEQ})))
	{
		rhs = comparison();
		ret = std::make_shared<BinaryExpr>(ret, op.type(), rhs);
	}
	return ret;
}

/* comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ; */
ASTNodePtr Parser::comparison()
{
	ASTNodePtr lhs = term();
	ASTNodePtr rhs;
	ASTNodePtr ret = lhs;

	Token op;
	while ((op = m_lexer.match({TOK_GT, TOK_LT, TOK_GE, TOK_LE})))
	{
		rhs = term();
		ret = std::make_shared<BinaryExpr>(ret, op.type(), rhs);
	}
	return ret;
}

/* term           → factor ( ( "-" | "+" ) factor )* ; */
ASTNodePtr Parser::term()
{
	ASTNodePtr lhs = factor();
	ASTNodePtr rhs;
	ASTNodePtr ret = lhs;
	
	Token op;
	while ((op = m_lexer.match({TOK_PLUS, TOK_MINUS})))
	{
		rhs = factor();
		ret = std::make_shared<BinaryExpr>(ret, op.type(), rhs);
	}
	return ret;
}

/* factor         → unary ( ( "/" | "*" ) unary )* ; */
ASTNodePtr Parser::factor()
{
	ASTNodePtr lhs = unary();
	ASTNodePtr rhs;
	ASTNodePtr ret = lhs;

	Token op;
	while ((op = m_lexer.match({TOK_STAR, TOK_SLASH})))
	{
		rhs = unary();
		ret = std::make_shared<BinaryExpr>(ret, op.type(), rhs);
	}
	return ret;
}

/* unary          → ( "!" | "-" ) unary | call ; */
ASTNodePtr Parser::unary()
{
	ASTNodePtr expr;
	Token op;
	if ((op = m_lexer.match({TOK_LNOT, TOK_MINUS})))
	{
		expr = unary();
		return std::make_shared<UnaryExpr>(op.type(), expr);
	}
	else
	{
		return call();
	}
}

/* call           → primary ( "(" arguments? ")" | "." IDENTIFIER )* ; */
ASTNodePtr Parser::call() /* same thing used for grouping */
{
	ASTNodePtr lhs = primary();
	if (m_lexer.match(TOK_OPAREN))
	{
		ASTNodePtr rhs = arguments();
		expect(TOK_CPAREN);
		return std::make_shared<BinaryExpr>(lhs, TOK_CPAREN, rhs);
	}
	return lhs;
}

/* arguments      → expression ( "," expression )* ; */
ASTNodePtr Parser::arguments()
{
	std::vector<ASTNodePtr> exprlist;
	if (m_lexer.match(TOK_CPAREN)) /* empty arg list: <primary>(); */
		return std::make_shared<ExprList>(std::move(exprlist));
	do
	{
		exprlist.push_back(expression());
	} while(m_lexer.match(TOK_COMMA));
	return std::make_shared<ExprList>(std::move(exprlist)); 
}

/* primary        → "taut" | "nil" 
               | NUMBER | STRING | IDENTIFIER | "(" expression ")"
*/
ASTNodePtr Parser::primary()
{
	Token tok = m_lexer.next();
	TokenType type = tok.type();


	switch(tok.type())
	{
		case TOK_TAUT: 						return std::make_shared<TautExpr>();
		case TOK_NIL:						return std::make_shared<NilExpr>();
		case TOK_NUMBER:					return std::make_shared<LiteralIntExpr>(tok);
		case TOK_IDENTIFIER:				return std::make_shared<IdentifierExpr>(tok);
		case TOK_STRING: 					return std::make_shared<LiteralStringExpr>(tok);
		default:
											expect("PrimaryExpression.");
	}


	/*if (type == TOK_TAUT || type == TOK_NIL || type == TOK_NUMBER || type == TOK_IDENTIFIER)*/
	/*	return std::make_shared<PrimaryExpr>(tok);*/
	/*if (m_lexer.match(TOK_OPAREN))*/
	/*{*/
	/*	ASTNodePtr ret = expression();*/
	/*	expect(TOK_CPAREN);*/
	/*	return ret;*/
	/*}*/
	expect("Primary Expression");

	assert(false && "Unreachable");
	/* return std::make_shared<UnaryExpr>(TOK_NONE, std::make_shared<PrimaryExpr>()); /1* Unreachable *1/ */
}


void Parser::expect(TokenType t)
{
	if (!m_lexer.match(t)) {
		Token tok{t};
		fprintf(stderr, "Expected a token of type %s, instead of %s\n", tok.printable().c_str(), m_lexer.peek().printable().c_str());
		/* TODO: implement panic mode and stuff like that */
		exit(EXIT_FAILURE);
	}
}
void Parser::expect(const std::string& msg)
{
	fprintf(stderr, "Expected: %s\n", msg.c_str());
	exit(EXIT_FAILURE);
}

