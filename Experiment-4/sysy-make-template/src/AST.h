#pragma once

#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
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

class BlockItemAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> number;
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "BlockItem { ";
    if (number){
      number->Dump();
    }
    if (exp){
      exp->Dump();
    }
    std::cout << " }";
  }
};

// Exp ::= UnaryExp | AddExp;

class ExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> add_exp;
  void Dump() const override {
    std::cout << "ExpAST { ";
    if (unary_exp){
      unary_exp->Dump();
    }
    if (add_exp) {
      add_exp->Dump();
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
        std::cout << "UnaryOp { " + add_op + " } ";
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
      std::cout << "UnaryOp { " + mul_op + " } ";
      unary_exp->Dump();
    }
    else if(unary_exp){
      unary_exp->Dump();
    }
    std::cout << " }";
  }
};

// PrimaryExp ::= "(" Exp ")" | Number;
class PrimaryExpAST : public BaseAST
{
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> number;
  void Dump() const override {
    std::cout << "PrimaryExpAST { ";
    if (exp){
      exp->Dump();
    }
    if (number){
      number->Dump();
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
      std::cout << "UnaryOp { " + unary_op + " } ";
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