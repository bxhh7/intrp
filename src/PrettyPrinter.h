#pragma once
#include "Parser.h"

struct PrettyPrinter : public ASTVisitor
{
	PrettyPrinter(std::ostream& os) : m_ostream {os} {}
	Object visit(const PrimaryExpr&) override;
	Object visit(const BinaryExpr&) override;
	Object visit(const ExprList&) override;
	Object visit(const UnaryExpr&) override;
	Object visit(const AssignmentExpr&) override;
	Object visit(const LiteralIntExpr&) override;
	Object visit(const LiteralStringExpr&) override;
	Object visit(const IdentifierExpr&) override;
	Object visit(const TautExpr&) override;
	Object visit(const NilExpr&) override;
	Object visit(const IfExpr&) override;
	Object visit(const WhileExpr&) override;
	Object visit(const PrintExpr&) override;
	Object visit(const FuncDeclExpr&) override;
	Object visit(const FuncCallExpr&) override;
	Object visit(const BlockExpr&) override;
	private:
		std::ostream& m_ostream;
};

