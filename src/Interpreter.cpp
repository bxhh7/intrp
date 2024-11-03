#include <assert.h>
#include <iostream>
#include "Interpreter.h"

void Interpreter::error(const std::string& error)
{
	throw std::runtime_error(error);
}

Object Interpreter::visit(const PrimaryExpr& p) 
{
	error("Interpreter : primary doenst mean shit.\n");
	return Object{}; // unreachable
}

Object Interpreter::visit(const BinaryExpr& b) 
{
	Object lhs = b.lhs->accept(*this);
	if (!(lhs.is_primary  && lhs.m_primary_value.type == PRIMARYTYPE_INT))
	{
		/* TODO throw expcetion */
		std::cerr << "TEMPORARY:lhs is expected to be int" << std::endl;
		return Object{};
	}
	Object rhs = b.rhs->accept(*this);
	if (!(rhs.is_primary  && rhs.m_primary_value.type == PRIMARYTYPE_INT))
	{
		/* TODO throw expcetion */
		std::cerr << "TEMPORARY:rhs is expected to be int" << std::endl;
		return Object{};
	}

	int64_t rhs_value = rhs.m_primary_value.integer;
	int64_t lhs_value = lhs.m_primary_value.integer;
	switch(b.op)
	{
		case TOK_LOR:  					return lhs_value || rhs_value; break;
		case TOK_LAND: 					return lhs_value && rhs_value; break;
		case TOK_EQ: 					return lhs_value == rhs_value; break;
		case TOK_GT: 					return lhs_value > rhs_value;  break;
		case TOK_GE: 					return lhs_value >= rhs_value; break;
		case TOK_LE: 					return lhs_value <= rhs_value;  break;
		case TOK_LT: 					return lhs_value < rhs_value; break;
		case TOK_PLUS: 					return lhs_value + rhs_value;  break;
		case TOK_MINUS: 				return lhs_value - rhs_value;  break;
		case TOK_STAR: 					return lhs_value * rhs_value;  break;
		case TOK_SLASH: 				return lhs_value / rhs_value;  break;
		default:
			std::cerr << "invalid binary operator " << b.op << std::endl;
			error("invaid binary operaotr");
			return Object{};
	}
}

Object Interpreter::visit(const ExprList&) 
{
	error("Exprlist not implemented");
	return Object{};
}

Object Interpreter::visit(const UnaryExpr&) 
{
	error("UnaryExpr not implemented");
	return Object{};
}

Object Interpreter::visit(const AssignmentExpr& ass) 
{
	if (!std::dynamic_pointer_cast<IdentifierExpr>(ass.lhs))
	{
		error("expected lvalue for assignment.\n"); 
		return Object{};
	}

	std::shared_ptr<IdentifierExpr> id = std::dynamic_pointer_cast<IdentifierExpr>(ass.lhs);
	Object rhs = ass.rhs->accept(*this);
	env[id->tok.lexeme()] = ass.rhs->accept(*this);
	return rhs;
}

Object Interpreter::visit(const LiteralIntExpr& li) 
{
	return atoi(li.tok.lexeme().c_str());
}

Object Interpreter::visit(const LiteralStringExpr& ls) 
{
	return ls.tok.lexeme();
}

Object Interpreter::visit(const IdentifierExpr& id) 
{
	auto it = this->env.find(id.tok.lexeme());
	if (it == this->env.end())
	{
		std::cerr << "undeclared identifier " << id.tok.lexeme() << std::endl;
		error("undeclared identifier");
	}
	return this->env[id.tok.lexeme()];
}

Object Interpreter::visit(const TautExpr&) 
{
	return true;
}
Object Interpreter::visit(const NilExpr&) 
{
	return false;
}
Object Interpreter::visit(const IfExpr& cond) 
{
	Object condition_eval = cond.condition->accept(*this);
	/* TODO: add boolean as primary type */
	if (!(condition_eval.is_primary && condition_eval.m_primary_value.type == PRIMARYTYPE_INT))
	{
		std::cerr << "TEMPORARY: condition must evaluate to integer\n";
		return Object{};
	}
	if (condition_eval.m_primary_value.integer)
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
	return Object{};
}
Object Interpreter::visit(const WhileExpr& whilex)
{

	Object condition_eval = whilex.condition->accept(*this);
	/* TODO: add boolean as primary type */
	if (!(condition_eval.is_primary && condition_eval.m_primary_value.type == PRIMARYTYPE_INT))
	{
		std::cerr << "TEMPORARY: condition must evaluate to integer\n";
		return Object{};
	}
	while (condition_eval.m_primary_value.integer)
	{
		condition_eval = whilex.condition->accept(*this);
		/* TODO: add boolean as primary type */
		if (!(condition_eval.is_primary && condition_eval.m_primary_value.type == PRIMARYTYPE_INT))
		{
			std::cerr << "TEMPORARY: condition must evaluate to integer\n";
			return Object{};
		}
		for (int i = 0; i < whilex.body.size(); i++)
		{
			whilex.body[i]->accept(*this);
		}
	}
	return Object{};
}

Object Interpreter::visit(const PrintExpr& p) 
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
	return Object{};
}

Object Interpreter::visit(const FuncDeclExpr& fd) 
{
	this->func_env[fd.name.lexeme()] = fd.body;
	return Object{};
}

Object Interpreter::visit(const FuncCallExpr& fc) 
{
	auto it = this->func_env.find(fc.name.lexeme());
	if (it == this->func_env.end())
	{
		std::cerr << "undeclared identifier '" << fc.name.lexeme()  << "'"<< std::endl;
		error("undeclared identifier");
	}
	/*return this->env[id.tok.lexeme()];*/

	it->second->accept(*this);

	return Object{};
}

Object Interpreter::visit(const BlockExpr& bl) 
{
	for (int i = 0; i < bl.body.size(); i++)
	{
		bl.body[i]->accept(*this);
	}
	return Object{};
}
