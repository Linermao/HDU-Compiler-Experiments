#pragma once

#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string.h>
#include <string>

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
	virtual void Dump() const = 0;
};	

// CompUnit 是 BaseAST
class CompUnitAST : public BaseAST{
public:
  std::unique_ptr<BaseAST> func_def;

	void Dump() const override {
			std::cout << "CompUnitAST { ";
			func_def->Dump();
			std::cout << " }";
		}
};

class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    block->Dump();
    std::cout << " }";
  }
};

class FuncTypeAST : public BaseAST
{
  public:
    std::string type;
  void Dump() const override {
    std::cout << "FunctypeAST { ";
    std::cout << type;
    std::cout << " }";
  }
};

// Block ::= '{' { BlockItem } '}'
class BlockAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> blockitem;

  void Dump() const override {
    std::cout << "BlockAST { ";
    blockitem->Dump();
    std::cout << " }";
  }
};

// BlockItem ::= Decl | Stmt
class BlockItemAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> block_item;
  void Dump() const override {
    std::cout << "BlockItemAST { ";
    if (decl){
      decl->Dump();
    }
    if (stmt){
      stmt->Dump();
    }
    if (block_item){
      block_item->Dump();
    }
    std::cout << " }";
  }
};

class StmtAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> number;
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "StmtAST { ";
    if (number){
      number->Dump();
    }
    if (exp){
      exp->Dump();
    }
    std::cout << " }";
  }
};

// Decl ::= ConstDecl
class DeclAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> const_decl;
  void Dump() const override {
    std::cout << "DeclAST { ";
    const_decl->Dump();
    std::cout << " }";
  }
};

// ConstDecl ::= "const" BType ConstDef { ',' ConstDef } ';'
class ConstDeclAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::unique_ptr<BaseAST> const_def;
  void Dump() const override {
    std::cout << "ConstDeclAST { ";
    std::cout << "{ const } ";
    b_type->Dump();
    const_def->Dump();
    std::cout << " }";
  }
};

// BType ::= "int"
class BTypeAST : public BaseAST
{
  public:
    std::string type;
  void Dump() const override {
    std::cout << "BTypeAST { ";
    std::cout << type;
    std::cout << " }";
  }
};

// ConstDef :: IDENT "=" ConstInitVal
class ConstDefAST : public BaseAST
{
  public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    std::unique_ptr<BaseAST> const_def;
  void Dump() const override {
    std::cout << "ConstDefAST { ";
    std::cout << ident << ", ";
    const_init_val->Dump();
    std::cout << " }";
  }
};

// ConstInitVal ::= ConstExp
class ConstInitValAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> const_exp;
  void Dump() const override {
    std::cout << "ConstInitValAST { ";
    const_exp->Dump();
    std::cout << " }";
  }
};

// ConstExp ::= Exp
class ConstExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "ConstExpAST { ";
    exp->Dump();
    std::cout << " }";
  }
};

// LVal ::= IDENT
class LValAST : public BaseAST
{
  public:
    std::string ident;
  void Dump() const override {
    std::cout << "LValAST { ";
    std::cout << ident;
    std::cout << " }";
  }
};

// Exp ::= UnaryExp | AddExp | LOrExp;
class ExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> add_exp;
    std::unique_ptr<BaseAST> l_or_exp;
  void Dump() const override {
    std::cout << "ExpAST { ";
    if (unary_exp){
      unary_exp->Dump();
    }
    if (add_exp) {
      add_exp->Dump();
    }
    if (l_or_exp){
      l_or_exp->Dump();
    }
    std::cout << " }";
  }
};

// AddExp ::= MulExp | AddExp ( '+' | '-') MulExp;
class AddExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> add_exp;
    std::string add_op;
    std::unique_ptr<BaseAST> mul_exp;
  void Dump() const override {
    std::cout << "AddExpAST { ";
    if (add_exp){      
        add_exp->Dump();
        std::cout << " AddOp { " + add_op + " } ";
        mul_exp->Dump();
    }
    else if (mul_exp){
      mul_exp->Dump();
    }
    
    std::cout << " }";
  }
};

// MulExp ::= UnaryExp | MulExp ( '*' | '/' | '%' ) UnaryExp;
class MulExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> mul_exp;
    std::string mul_op;
    std::unique_ptr<BaseAST> unary_exp;
  void Dump() const override {
    std::cout << "MulExpAST { ";
    if (mul_exp){
      mul_exp->Dump();
      std::cout << " UnaryOp { " + mul_op + " } ";
      unary_exp->Dump();
    }
    else if(unary_exp){
      unary_exp->Dump();
    }
    std::cout << " }";
  }
};

// RelExp ::= AddExp | RelExp ( "<" | ">" | "<=" | ">=" ) AddExp;
class RelExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> rel_exp;
    std::string rel_op;
    std::unique_ptr<BaseAST> add_exp;
  void Dump() const override {
    std::cout << "RelExpAST { ";
    if (rel_exp){
      rel_exp->Dump();
      std::cout << " RelOp { " + rel_op + " } ";
      add_exp->Dump();
    }
    else if(add_exp){
      add_exp->Dump();
    }
    std::cout << " }";
  }
};

// EqExp ::= RelExp | EqExp ( "==" | "!=" ) RelExp;
class EqExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> eq_exp;
    std::string eq_op;
    std::unique_ptr<BaseAST> rel_exp;
  void Dump() const override {
    std::cout << "EqExpAST { ";
    if (eq_exp){
      eq_exp->Dump();
      std::cout << " EqOp { " + eq_op + " } ";
      rel_exp->Dump();
    }
    else if(rel_exp){
      rel_exp->Dump();
    }
    std::cout << " }";
  }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp;
class LAndExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> l_and_exp;
    std::string l_and_op;
    std::unique_ptr<BaseAST> eq_exp;
  void Dump() const override {
    std::cout << "LAndExpAST { ";
    if (l_and_exp){
      l_and_exp->Dump();
      std::cout << " LAndOp { " + l_and_op + " } ";
      eq_exp->Dump();
    } else if(eq_exp){
      eq_exp->Dump();
    }
    std::cout << " }";
  }
};

// LOrExp ::= LAndExp | LOrExp "||" LAndExp;
class LOrExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> l_or_exp;
    std::string l_or_op;
    std::unique_ptr<BaseAST> l_and_exp;
  void Dump() const override {
    std::cout << "LOrExpAST { ";
    if (l_or_exp){
      l_or_exp->Dump();
      std::cout << " UnaryOp { " + l_or_op + " } ";
      l_and_exp->Dump();
    } else if(l_and_exp){
      l_and_exp->Dump();
    }
    std::cout << " }";
  }
};

// PrimaryExp ::= "(" Exp ")" | Number | LVal;
class PrimaryExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> number;
    std::unique_ptr<BaseAST> l_val;
  void Dump() const override {
    std::cout << "PrimaryExpAST { ";
    if (exp){
      exp->Dump();
    }
    if (number){
      number->Dump();
    }
    if (l_val) {
      l_val->Dump();
    }
    std::cout << " }";
  }
};

// UnaryExp ::= PrimaryExp | UnaryOp UnaryExp;
class UnaryExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> primary_exp;
    std::string unary_op;
    std::unique_ptr<BaseAST> unary_exp;
  void Dump() const override {
    std::cout << "UnaryExpAST { ";
    if (primary_exp){
      primary_exp->Dump();
    }
    if (unary_exp){
      std::cout << " UnaryOp { " + unary_op + " } ";
      unary_exp->Dump();
    }
    std::cout << " }";
  }
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST
{
  public:
    std::string number;
  void Dump() const override {
    std::cout << number;
  }
};