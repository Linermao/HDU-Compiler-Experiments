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
// CompUnit ::= CompUnits
class CompUnitAST : public BaseAST{
public:
  std::unique_ptr<BaseAST> comp_units;
	void Dump() const override {
			std::cout << "CompUnitAST { ";
      comp_units->Dump();
			std::cout << " }";
		}
};

// CompUnits ::= [CompUnits] (Decl | FuncDef)
class CompUnitsAST : public BaseAST{
public:
  std::unique_ptr<BaseAST> comp_units;
  std::unique_ptr<BaseAST> decl;
  std::unique_ptr<BaseAST> func_def;
	void Dump() const override {
			std::cout << "CompUnitsAST { ";
      if(comp_units){
        comp_units->Dump();
      }
      if (decl){
        decl->Dump();
      }
      if (func_def){
        func_def->Dump();
      }
			std::cout << " }";
		}
};

// Decl ::= ConstDecl | VarDecl
class DeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> const_decl;
    std::unique_ptr<BaseAST> var_decl;
  void Dump() const override {
    std::cout << "DeclAST { ";
    if (const_decl){
      const_decl->Dump();
    }
    if (var_decl){
      var_decl->Dump();
    }
    std::cout << " }";
  }
};

// ConstDecl ::= "const" BType ConstDef { ',' ConstDef } ';'
class ConstDeclAST : public BaseAST{
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
class BTypeAST : public BaseAST{
  public:
    std::string type;
  void Dump() const override {
    std::cout << "BTypeAST { ";
    std::cout << type;
    std::cout << " }";
  }
};

// ConstDef :: IDENT "=" ConstInitVal
class ConstDefAST : public BaseAST{
  public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    std::unique_ptr<BaseAST> const_def;
  void Dump() const override {
    std::cout << "ConstDefAST { ";
    std::cout << ident << " = ";
    const_init_val->Dump();
    if (const_def){
      const_def->Dump();
    }
    std::cout << " }";
  }
};

// ConstInitVal ::= ConstExp
class ConstInitValAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> const_exp;
  void Dump() const override {
    std::cout << "ConstInitValAST { ";
    const_exp->Dump();
    std::cout << " }";
  }
};

// ConstExp ::= Exp
class ConstExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "ConstExpAST { ";
    exp->Dump();
    std::cout << " }";
  }
};

// VarDecl ::= BType VarDef { ',' VarDef } ';'
class VarDeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::unique_ptr<BaseAST> var_def;
  void Dump() const override {
    std::cout << "VarDeclAST { ";
    b_type->Dump();
    var_def->Dump();
    std::cout << " }";
  }
};

// VarDef ::= IDENT | IDENT "=" InitVal
class VarDefAST : public BaseAST{
  public:
    std::string ident;
    std::unique_ptr<BaseAST> init_val;
    std::unique_ptr<BaseAST> var_def;
  void Dump() const override {
    std::cout << "VarDefAST { ";
    std::cout << ident;
    if (init_val){
      std::cout << " = ";
      init_val->Dump();
    }
    if (var_def){
      var_def->Dump();
    }
    std::cout << " }";
  }
};

// InitVal ::= Exp
class InitValAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "InitValAST { ";
    exp->Dump();
    std::cout << " }";
  }
};

// FuncDef ::= FuncType IDENT '(' [FuncFParams] ')' Block
class FuncDefAST : public BaseAST {
 public:
  std::unique_ptr<BaseAST> func_type;
  std::string ident;
  std::unique_ptr<BaseAST> func_f_params;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    std::cout << "FuncDefAST { ";
    func_type->Dump();
    std::cout << ", " << ident << ", ";
    if(func_f_params){
      func_f_params->Dump();
    }
    block->Dump();
    std::cout << " }";
  }
};

// FuncType ::= "int" | "void"
class FuncTypeAST : public BaseAST{
  public:
    std::string type;
  void Dump() const override {
    std::cout << "FunctypeAST { ";
    std::cout << type;
    std::cout << " }";
  }
};

// FuncFParams ::= FuncFParam { ',' FuncFParam }
class FuncFParamsAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> func_f_param;
  void Dump() const override {
    std::cout << "FuncFParamsAST { ";
    func_f_param->Dump();
    std::cout << " }";
  }
};

// FuncFParam ::= BType IDENT
class FuncFParamAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::string ident;
    std::unique_ptr<BaseAST> func_f_param;
  void Dump() const override {
    std::cout << "FuncFParamAST { ";
    b_type->Dump();
    std::cout << ", " << ident << ", ";
    if (func_f_param){
      func_f_param->Dump();
    }
    std::cout << " }";
  }
};

// Block ::= '{' { BlockItem } '}'
class BlockAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> blockitem;

  void Dump() const override {
    std::cout << "BlockAST { ";
    blockitem->Dump();
    std::cout << " }";
  }
};

// BlockItem ::= Decl | Stmt
class BlockItemAST : public BaseAST{
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

/* Stmt ::= LVal '=' Exp ';' 
          | [Exp] ';' 
          | Block 
          | "return" [Exp] ';'
          | "if" '(' Exp ')' Stmt [ "else" Stmt ] 
          | "while" '(' Exp ')' Stmt
          | "break" ';'
          | "continue" ';'
*/
class StmtAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> l_val;
    std::unique_ptr<BaseAST> block;
    std::unique_ptr<BaseAST> stmt_1;
    std::unique_ptr<BaseAST> stmt_2;
    std::string symbol;
  void Dump() const override {
    std::cout << "StmtAST { ";
    if (symbol == "if"){
      if(stmt_2){
        std::cout << "if ( ";
        exp->Dump();
        std::cout << " ) { ";
        stmt_1->Dump();
        std::cout << " } else { ";
        stmt_2->Dump();
        std::cout << " }";
      }else {
        std::cout << "if ( ";
        exp->Dump();
        std::cout << " ) { ";
        stmt_1->Dump();
        std::cout << " }";
      }
    }else if (symbol == "while"){
      std::cout << "while ( ";
      exp->Dump();
      std::cout << " ) { ";
      stmt_1->Dump();
      std::cout << " }";
    }else if (symbol == "return"){
      std::cout << "return { ";
      if (exp){
        exp->Dump();
      }
      std::cout << " }";
    }else if (symbol == "break"){
      std::cout << "break { }";
    }else if (symbol == "continue"){
      std::cout << "continue { }";
    }else if (l_val && exp){
      l_val->Dump();
      exp->Dump();
    }else if (block){
      block->Dump();
    }else if(exp){
      exp->Dump();
    }

    std::cout << " }";
  }
};

// Exp ::= LOrExp;
class ExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> l_or_exp;
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "ExpAST { ";
    if (l_or_exp){
      l_or_exp->Dump();
    }
    if (exp){
      exp->Dump();
    }
    std::cout << " }";
  }
};

// LVal ::= IDENT
class LValAST : public BaseAST{
  public:
    std::string ident;
  void Dump() const override {
    std::cout << "LValAST { ";
    std::cout << ident;
    std::cout << " }";
  }
};

// PrimaryExp ::= "(" Exp ")" | Number | LVal;
class PrimaryExpAST : public BaseAST{
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

// Number ::= INT_CONST;
class NumberAST : public BaseAST{
  public:
    std::string number;
  void Dump() const override {
    std::cout << number;
  }
};

/* UnaryExp ::= PrimaryExp 
              | UnaryOp UnaryExp 
              | IDENT '(' [FuncRParams] ')'
*/
class UnaryExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> primary_exp;
    std::string unary_op;
    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> func_r_params;
  void Dump() const override {
    std::cout << "UnaryExpAST { ";
    if (primary_exp){
      primary_exp->Dump();
    }
    if (unary_exp){
      std::cout << " UnaryOp { " + unary_op + " } ";
      unary_exp->Dump();
    }
    if (func_r_params) {
      func_r_params->Dump();
    }
    std::cout << " }";
  }
};

// UnaryOp ::= '+' | '-' | '!' ;
class UnaryOpAST : public BaseAST{
  public:
    std::string unary_op;
  void Dump() const override {
    std::cout << unary_op;
  }
};

// FuncRParams ::= Exp { ',' Exp }
class FuncRParamsAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "FuncRParamsAST { ";
    exp->Dump();
    std::cout << " }";
  }
};

// MulExp ::= UnaryExp | MulExp ( '*' | '/' | '%' ) UnaryExp;
class MulExpAST : public BaseAST{
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

// AddExp ::= MulExp | AddExp ( '+' | '-') MulExp;
class AddExpAST : public BaseAST{
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

// RelExp ::= AddExp | RelExp ( "<" | ">" | "<=" | ">=" ) AddExp;
class RelExpAST : public BaseAST{
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
class EqExpAST : public BaseAST{
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
class LAndExpAST : public BaseAST{
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
class LOrExpAST : public BaseAST{
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



