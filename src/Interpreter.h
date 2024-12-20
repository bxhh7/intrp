#pragma once
#include <unordered_map>
#include <vector>
#include "Parser.h"
#include "Object.h"
#include "Env.h"

struct Interpreter : public ASTVisitor
{
	public:
	Interpreter() {}
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
	Object visit(const ReturnExpr&) override;
	Object visit(const VarDefExpr&) override;

	private:
	void print_object(const Object&);
	void error(const std::string& error);
	/* TODO: support other types */
	/* TODO custom class for Env */
		/* std::vector<Env> env_vec; */
		/* std::unordered_map<std::string, Object> env; */
	Env env;
	std::unordered_map<std::string, std::shared_ptr<BlockExpr>> func_env;
};

