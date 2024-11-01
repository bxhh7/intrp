#include <assert.h>
#include <iostream>
#include "Interpreter.h"

void Interpreter::error(const std::string& error)
{
	throw std::runtime_error(error);
}

int64_t Interpreter::visit(const PrimaryExpr& p) 
{
	error("Interpreter : primary doenst mean shit.\n");
	return 0; // unreachable
}

int64_t Interpreter::visit(const BinaryExpr& b) 
{
	/* TODO: type checking */
	int64_t lhs = b.lhs->accept(*this);
	int64_t rhs = b.rhs->accept(*this);
	switch(b.op)
	{
		case TOK_LOR:  					return lhs || rhs; break;
		case TOK_LAND: 					return lhs && rhs; break;
		case TOK_EQ: 					return lhs == rhs; break;
		case TOK_GT: 					return lhs > rhs;  break;
		case TOK_GE: 					return lhs >= rhs; break;
		case TOK_LE: 					return lhs <= rhs;  break;
		case TOK_LT: 					return lhs < rhs; break;
		case TOK_PLUS: 					return lhs + rhs;  break;
		case TOK_MINUS: 				return lhs - rhs;  break;
		case TOK_STAR: 					return lhs * rhs;  break;
		case TOK_SLASH: 				return lhs / rhs;  break;
		default:
			std::cerr << "invalid binary operator " << b.op << std::endl;
			error("invaid binary operaotr");
			return 0;
	}
}

int64_t Interpreter::visit(const ExprList&) 
{
	error("Exprlist not implemented");
	return 0;
}

int64_t Interpreter::visit(const UnaryExpr&) 
{
	error("UnaryExpr not implemented");
	return 0;
}

int64_t Interpreter::visit(const AssignmentExpr& ass) 
{
	if (!std::dynamic_pointer_cast<IdentifierExpr>(ass.lhs))
	{
		error("expected lvalue for assignment.\n"); 
		return 0;
	}

	std::shared_ptr<IdentifierExpr> id = std::dynamic_pointer_cast<IdentifierExpr>(ass.lhs);
	int64_t rhs = ass.rhs->accept(*this);
	env[id->tok.lexeme()] = ass.rhs->accept(*this);
	return rhs;
}

int64_t Interpreter::visit(const LiteralIntExpr& li) 
{
	return atoi(li.tok.lexeme().c_str());
}

int64_t Interpreter::visit(const LiteralStringExpr& ls)
{
	return 1;
}

int64_t Interpreter::visit(const IdentifierExpr& id) 
{
	auto it = this->env.find(id.tok.lexeme());
	if (it == this->env.end())
	{
		std::cerr << "undeclared identifier " << id.tok.lexeme() << std::endl;
		error("undeclared identifier");
	}
	return this->env[id.tok.lexeme()];
}

int64_t Interpreter::visit(const TautExpr&) 
{
	return true;
}
int64_t Interpreter::visit(const NilExpr&) 
{
	return false;
}
int64_t Interpreter::visit(const IfExpr& cond) 
{
	int64_t condition_eval = cond.condition->accept(*this);
	if (condition_eval)
	{
		for (int i = 0; i < cond.body.size(); i++)
		{
			cond.body[i]->accept(*this);
		}
	}
	else 
	{
		for (int i = 0; i < cond.else_cluse.size(); i++)
		{
			cond.else_cluse[i]->accept(*this);
		}
	}
	return 0;
}
int64_t Interpreter::visit(const WhileExpr& whilex)
{
	while (whilex.condition->accept(*this))
	{
		for (int i = 0; i < whilex.body.size(); i++)
		{
			whilex.body[i]->accept(*this);
		}
	}
	return 0;
}

int64_t Interpreter::visit(const PrintExpr& p) 
{
	if (p.tok.type() == TOK_IDENTIFIER)
	{

		auto it = this->env.find(p.tok.lexeme());
		if (it == this->env.end())
		{
			std::cout << "nil\n";
		}
		std::cout << this->env[p.tok.lexeme()];
	}
	else if (p.tok.type() == TOK_NUMBER) 
	{
		std::cout << p.tok.lexeme();
	}
	else if (p.tok.type() == TOK_STRING)
	{
		std::cout << p.tok.lexeme();
	}
	return 0;
}

int64_t Interpreter::visit(const FuncDeclExpr& fd) 
{
	this->func_env[fd.name.lexeme()] = fd.body;
	return 0;
}

int64_t Interpreter::visit(const FuncCallExpr& fc) 
{
	auto it = this->func_env.find(fc.name.lexeme());
	if (it == this->func_env.end())
	{
		std::cerr << "undeclared identifier '" << fc.name.lexeme()  << "'"<< std::endl;
		error("undeclared identifier");
	}
	/*return this->env[id.tok.lexeme()];*/

	it->second->accept(*this);

	return 0;
}

int64_t Interpreter::visit(const BlockExpr& bl) 
{
	for (int i = 0; i < bl.body.size(); i++)
	{
		bl.body[i]->accept(*this);
	}
	return 0;
}

