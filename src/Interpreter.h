#pragma once
#include <unordered_map>
#include "Parser.h"

struct Interpreter : public ASTVisitor
{
	public:
	Interpreter() {}
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
	void error(const std::string& error);
	/* TODO: support other types */
	/* TODO custom class for Env */
		std::unordered_map<std::string, uint64_t> env;
		std::unordered_map<std::string, std::shared_ptr<BlockExpr>> func_env;
};

