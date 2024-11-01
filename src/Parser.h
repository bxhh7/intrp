#pragma once

#include <memory>
#include "Lexer.h"

struct BinaryExpr;
struct ExprList;
struct UnaryExpr;
struct AssignmentExpr;
struct PrimaryExpr;
struct LiteralIntExpr;
struct LiteralStringExpr;
struct IdentifierExpr;
struct IfExpr;
struct WhileExpr;
struct TautExpr;
struct NilExpr;
struct PrintExpr;
struct FuncDeclExpr;
struct FuncCallExpr;
struct BlockExpr;

struct ASTVisitor
{
	public:
	/* TODO: replace int64_t with some kind of representaton of an object. */
	virtual int64_t visit(const IfExpr&) = 0;
	virtual int64_t visit(const WhileExpr&) = 0;
	virtual int64_t visit(const PrintExpr&) = 0;
	virtual int64_t visit(const FuncDeclExpr&) = 0;
	virtual int64_t visit(const FuncCallExpr&) = 0;
	virtual int64_t visit(const BlockExpr&) = 0;

	virtual int64_t visit(const PrimaryExpr&) = 0;
	virtual int64_t visit(const BinaryExpr&) = 0;
	virtual int64_t visit(const ExprList&) = 0;
	virtual int64_t visit(const UnaryExpr&) = 0;
	virtual int64_t visit(const AssignmentExpr&) = 0;
	virtual int64_t visit(const LiteralIntExpr&) = 0;
	virtual int64_t visit(const LiteralStringExpr&) = 0;
	virtual int64_t visit(const IdentifierExpr&) = 0;
	virtual int64_t visit(const TautExpr&) = 0;
	virtual int64_t visit(const NilExpr&) = 0;
};

struct ASTNode 
{
	virtual int64_t accept(ASTVisitor& ) const  = 0; 
	/* why is this a pure virtual function? 
	 * cause we want the appropriate visit() function called for every kind of 
	 * ASTNode, so we should basically make it pure virtual, so that the subtype gets
	 * "resolved" and the correct visit() method would be called */

};
struct Expr : public ASTNode {};

using ASTNodePtr = std::shared_ptr<ASTNode>;
using  ExprNodePtr = std::shared_ptr<Expr>;


struct BinaryExpr : public Expr
{
	BinaryExpr() {}
	BinaryExpr(ASTNodePtr& _lhs, TokenType _op, ASTNodePtr& _rhs) : lhs {_lhs}, op {_op}, rhs {_rhs} {}

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
	ASTNodePtr lhs;
	TokenType op;
	ASTNodePtr rhs;
};

struct BlockExpr : public Expr
{
	std::vector<ASTNodePtr> body;
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
};

struct FuncDeclExpr : public Expr
{
	Token name;
	std::shared_ptr<BlockExpr> body;
	std::vector<Token> parameters; /* Unused at the time */
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
};

struct FuncCallExpr : public Expr
{
	Token name;
	std::vector<ASTNodePtr> arguments;
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
};

struct IfExpr : public Expr
{
	ASTNodePtr condition;
	std::vector<ASTNodePtr> body;
	std::vector<ASTNodePtr> else_cluse;

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
};

struct WhileExpr : public Expr
{
	ASTNodePtr condition;
	std::vector<ASTNodePtr> body;
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
};

/* TODO: make a more robust print */
struct PrintExpr : public Expr
{
	Token tok;
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
};

struct AssignmentExpr : public BinaryExpr
{ 
	using BinaryExpr::BinaryExpr;
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this); } 
};

struct UnaryExpr : public Expr
{
	UnaryExpr() {}
	UnaryExpr(TokenType _op, ASTNodePtr& _expr) : op {_op}, expr {_expr} {}

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}

	TokenType op;
	ASTNodePtr expr;
};

struct ExprList : public Expr
{
	ExprList() {}
	ExprList(std::vector<ASTNodePtr> _vec) : list {std::move(_vec)} {}

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}
	
	std::vector<ASTNodePtr> list;
};

struct PrimaryExpr : public Expr
{
	PrimaryExpr() {}
	PrimaryExpr(Token& t) : tok {t}{}

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}

	Token tok;
};

struct LiteralIntExpr : public PrimaryExpr
{
	LiteralIntExpr() {}
	LiteralIntExpr(Token& t) : tok {t}{}

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}

	Token tok;
};

struct LiteralStringExpr : public PrimaryExpr
{
	LiteralStringExpr() {}
	LiteralStringExpr(Token& t) : tok {t}{}

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}

	Token tok;
};


struct IdentifierExpr : public PrimaryExpr
{
	IdentifierExpr() {}
	IdentifierExpr(Token& t) : tok {t}{}

	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}

	Token tok;
};

struct TautExpr : public PrimaryExpr
{
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}
};

struct NilExpr : public PrimaryExpr
{
	int64_t accept(ASTVisitor& v) const override { return v.visit(*this);}
};


class Parser
{
	public:
		/* I think it makes sense to move the Lexer. 
		 * after all we don't really want anybody else
		 * mutating the lexer. */
		Parser(Lexer&& l) : m_lexer {l} {};
		Parser() {} ;

		void set_lexer(Lexer&&);
		
		std::vector<ASTNodePtr> parse();

	private:
		ASTNodePtr decl();
		ASTNodePtr func();
		ASTNodePtr block();
		ASTNodePtr expression();
		ASTNodePtr condition();
		ASTNodePtr loop();
		ASTNodePtr print();

		ASTNodePtr assignment();
		ASTNodePtr logic_or();
		ASTNodePtr logic_and();
		ASTNodePtr equality();
		ASTNodePtr comparison();
		ASTNodePtr term();
		ASTNodePtr factor();
		ASTNodePtr unary();
		ASTNodePtr call();
		ASTNodePtr primary();

		ASTNodePtr arguments();

		void expect(TokenType);
		void expect(const std::string&);
		Lexer m_lexer;
};
