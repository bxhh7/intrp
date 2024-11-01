#pragma once
#include "Parser.h"

struct PrettyPrinter : public ASTVisitor
{
	PrettyPrinter(std::ostream& os) : m_ostream {os} {}
	int64_t visit(const PrimaryExpr&) override;
	int64_t visit(const BinaryExpr&) override;
	int64_t visit(const ExprList&) override;
	int64_t visit(const UnaryExpr&) override;
	int64_t visit(const AssignmentExpr&) override;
	int64_t visit(const LiteralIntExpr&) override;
	int64_t visit(const LiteralStringExpr&) override;
	int64_t visit(const IdentifierExpr&) override;
	int64_t visit(const TautExpr&) override;
	int64_t visit(const NilExpr&) override;
	int64_t visit(const IfExpr&) override;
	int64_t visit(const WhileExpr&) override;
	int64_t visit(const PrintExpr&) override;
	int64_t visit(const FuncDeclExpr&) override;
	int64_t visit(const FuncCallExpr&) override;
	int64_t visit(const BlockExpr&) override;
	private:
		std::ostream& m_ostream;
};

