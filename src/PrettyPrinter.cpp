#include <iostream>

#include "PrettyPrinter.h"

Object PrettyPrinter::visit(const UnaryExpr& unary)
{
	m_ostream << "(" << Token::type_str_map[unary.op];
	unary.expr->accept(*this);
	m_ostream << ")";
	return Object{};
}

Object PrettyPrinter::visit(const ExprList& expr_list)
{
	m_ostream << "(";
	for (const ASTNodePtr& node : expr_list.list)
	{
		node->accept(*this);
	}
	m_ostream << ")";
	return Object{};
}

Object PrettyPrinter::visit(const BinaryExpr& binary)
{
	m_ostream << "(" << Token::type_str_map[binary.op] << " ";
	binary.lhs->accept(*this);
	m_ostream << ", ";
	binary.rhs->accept(*this);
	m_ostream << ") ";
	return Object{};
}

Object PrettyPrinter::visit(const PrimaryExpr& primary)
{
	m_ostream << "(PRIMARY??)";
	/*std::string val;*/
	/*std::visit(*/
	/*		overloaded*/
	/*		{*/
	/*			[&](const Object i) { val = std::to_string(i); },*/
	/*			[&](const std::string& s) { val = s; }*/
	/*		}, primary.tok.value()*/
	/*		);*/
	/*m_ostream << " "  << val << " ";*/
	return Object{}; /* we're not really evaluating anything here */
}


Object PrettyPrinter::visit(const AssignmentExpr& ass) 
{
	m_ostream << "("; 
	ass.rhs->accept(*this);
	m_ostream << "<-";
	ass.lhs->accept(*this);
	m_ostream << ")";
	return Object{};
};
Object PrettyPrinter::visit(const LiteralIntExpr& li) 
{
	m_ostream << "[li" << li.tok.lexeme() << "]";
	return 0;
};

Object PrettyPrinter::visit(const LiteralStringExpr& ls) 
{
	m_ostream << "[ls" << ls.tok.lexeme() << "]";
	return 0;
};

Object PrettyPrinter::visit(const IdentifierExpr& id)  
{
	m_ostream << "[I " << id.tok.lexeme() << "]";
	return Object{};
};

Object PrettyPrinter::visit(const TautExpr&) 
{
	m_ostream << "Taut";
	return Object{};
};
Object PrettyPrinter::visit(const NilExpr&) 
{
	m_ostream << "Nil";
	return Object{};
};
Object PrettyPrinter::visit(const IfExpr& cond)  
{
	m_ostream << "(IF [";
	cond.condition->accept(*this);
	m_ostream << "] {";
	for (int i = 0; i < cond.body.size(); i++)
	{
		cond.body[i]->accept(*this);
	}
	m_ostream << "} else {";
	for (int i = 0; i < cond.else_cluse.size(); i++)
	{
		cond.else_cluse[i]->accept(*this);
	}
	m_ostream << "}";
	return Object{};
}

Object PrettyPrinter::visit(const WhileExpr& whilex) 
{
	m_ostream << "(WHILE[";
	whilex.condition->accept(*this);
	m_ostream << "] {";
	for (int i = 0; i < whilex.body.size(); i++)
	{
		whilex.body[i]->accept(*this);
	}
	m_ostream << "}";
	return Object{};
}
Object PrettyPrinter::visit(const PrintExpr& p) 
{
	return Object{};
}

Object PrettyPrinter::visit(const FuncDeclExpr& fd) 
{
	/* TODO print parameters */
	m_ostream << "FD "<< fd.name.lexeme() << "(){";
	fd.body->accept(*this);
	m_ostream << "}";
	return Object{};
}

Object PrettyPrinter::visit(const FuncCallExpr& fc) 
{
	m_ostream << "FC " << fc.name.lexeme() << "();";
	return Object{};
}

Object PrettyPrinter::visit(const BlockExpr& be) 
{
	m_ostream << "BLOCK{";
	for (int i = 0; i < be.body.size(); i++)
	{
		be.body[i]->accept(*this);
	}
	m_ostream << "}ENDBLOCK";
	return Object{};
}
Object PrettyPrinter::visit(const ReturnExpr& re) 
{
	m_ostream << "RETURN ("; 
	re.expr->accept(*this);
	m_ostream << ");\n";
	return Object{};
}

Object PrettyPrinter::visit(const VarDefExpr& vd) 
{
	m_ostream << "VARDEF (" << vd.identifier.lexeme();
	if (vd.rhs){
		m_ostream << "<- ";
		vd.rhs->accept(*this);
	}
	m_ostream << ")\n";
	return Object{};
}