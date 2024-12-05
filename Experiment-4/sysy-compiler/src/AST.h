#pragma once

#include <cassert>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stack>
#include <string.h>
#include <string>
#include <vector>

typedef struct{
  int type;
  int value;
  std::string name;
}Symbol;

typedef std::map<std::string, Symbol> SymbolMap;

typedef struct func_symbol{
  int depth;
  int block_end;
  std::stack<int> loop_stack;
  std::vector<SymbolMap> symbol_maps;
  std::set<std::string> nameset;
  func_symbol(){
    depth = 0;
    block_end = 0;
  }
} FuncSymbol;

typedef std::map<std::string, std::unique_ptr<func_symbol>> FuncSymbolMap;

typedef struct{
  SymbolMap symbol_map;
  FuncSymbolMap func_symbol_map;
} SymbolTable;

static SymbolTable symbol_table;
static func_symbol* current_func_symbol_table = NULL;
static std::string current_func_name = "";

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
	virtual void Dump() const = 0;
  virtual void Semantic_Analysis(){
    return;
  }
  virtual void Print_AST(){
    return;
  }
};	

// CompUnit 是 BaseAST
// CompUnit ::= CompUnits
class CompUnitAST : public BaseAST{
public:
  std::unique_ptr<BaseAST> comp_units;
	void Dump() const override {
      if (comp_units){
        comp_units->Dump();
      }
	}
  void Print_AST() override {
		std::cout << "CompUnitAST:" << std::endl;
    comp_units->Print_AST();
  }
  void Semantic_Analysis() override {
    comp_units->Semantic_Analysis();
  }
};

// CompUnits ::= [CompUnits] (FuncDefOrVarDecl | ConstDecl)
class CompUnitsAST : public BaseAST{
public:
  std::unique_ptr<BaseAST> comp_units;
  std::unique_ptr<BaseAST> funcdef_or_vardecl;
  std::unique_ptr<BaseAST> const_decl;
  std::unique_ptr<BaseAST> func_def;
	void Dump() const override {
    if(comp_units){
      comp_units->Dump();
    }
    if (funcdef_or_vardecl){
      funcdef_or_vardecl->Dump();
    }
    if (const_decl){
      const_decl->Dump();
    }
    if (func_def){
      func_def->Dump();
    }
	}
  void Print_AST() override {
		std::cout << "CompUnitsAST:" << std::endl;
    if(comp_units){
      comp_units->Print_AST();
    }
    if (funcdef_or_vardecl){
      funcdef_or_vardecl->Print_AST();
    }
    if (const_decl){
      const_decl->Print_AST();
    }
    if (func_def){
      func_def->Print_AST();
    }
	}
  void Semantic_Analysis() override {
    if(comp_units){
      comp_units->Semantic_Analysis();
    }
    if (funcdef_or_vardecl){
      funcdef_or_vardecl->Semantic_Analysis();
    }
    if (const_decl){
      const_decl->Semantic_Analysis();
    }
    if (func_def){
      func_def->Semantic_Analysis();
    }
	}
};

// FuncDef_ ::= IDENT '(' [FuncFParams] ')' Block
class FuncDefAST_ : public BaseAST {
 public:
  std::string func_type;
  std::string ident;
  std::unique_ptr<BaseAST> func_f_params;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    if(func_f_params){
      func_f_params->Dump();
    }
    block->Dump();
  }
  void Print_AST() override {
    std::cout << "FuncDefAST: " << std::endl;
    std::cout << "TYPE: " << func_type << std::endl;
    std::cout << "IDENT: " << ident << std::endl;
    if(func_f_params){
      func_f_params->Print_AST();
    }
    block->Print_AST();
  }
  void Semantic_Analysis() override{
    // redefinition check
    if (symbol_table.func_symbol_map.find(ident) != symbol_table.func_symbol_map.end()){
      std::cout << "Error: redefinition of function " << ident << std::endl;
      exit(1);
    }

    if(func_f_params){
      func_f_params->Semantic_Analysis();
    }

    symbol_table.func_symbol_map[ident] = std::make_unique<func_symbol>();

    current_func_symbol_table = symbol_table.func_symbol_map[ident].get();
    SymbolMap m;
    current_func_symbol_table->symbol_maps.push_back(m);
    current_func_symbol_table->block_end = 0;


    block->Semantic_Analysis();

    current_func_symbol_table->symbol_maps.pop_back();
    current_func_symbol_table = NULL;
  }
};

// BType ::= "int"
class BTypeAST : public BaseAST{
  public:
    std::string type;
  void Dump() const override {
  }
  void Print_AST() override {
    std::cout << "BTypeAST:" << std::endl;
    std::cout << "TYPE: " << type << std::endl;
    std::cout << type;
  }
};

// FuncDefOrVarDecl ::= BType FuncDef_ | BType VarDecl_
class FuncDefOrVarDeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::unique_ptr<BaseAST> func_def;
    std::unique_ptr<BaseAST> var_decl;
  void Dump() const override {
    if (func_def){
      ((FuncDefAST_ *) func_def.get())->func_type = ((BTypeAST *) b_type.get())->type;
      func_def->Dump();
    }else if (var_decl){
      var_decl->Dump();
    }
    if(b_type){
      b_type->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "FuncDefOrVarDeclAST:" << std::endl;
    if (func_def){
      ((FuncDefAST_ *) func_def.get())->func_type = ((BTypeAST *) b_type.get())->type;
      func_def->Print_AST();
    }
    if (var_decl){
      var_decl->Print_AST();
    }
  }
  void Semantic_Analysis() override {
    if (func_def){
      ((FuncDefAST_ *) func_def.get())->func_type = ((BTypeAST *) b_type.get())->type;
      func_def->Semantic_Analysis();
    }
    if (var_decl){
      var_decl->Semantic_Analysis();
    }
  }
};

// Decl ::= ConstDecl | VarDecl
class DeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> const_decl;
    std::unique_ptr<BaseAST> var_decl;
  void Dump() const override {
    if (const_decl){
      const_decl->Dump();
    }
    if (var_decl){
      var_decl->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "DeclAST:" << std::endl;
    if (const_decl){
      const_decl->Print_AST();
    }
    if (var_decl){
      var_decl->Print_AST();
    }
  }
  void Semantic_Analysis() override {
    if (const_decl){
      const_decl->Semantic_Analysis();
    }
    if (var_decl){
      var_decl->Semantic_Analysis();
    }
  }
};

// ConstDecl ::= "const" BType ConstDef { ',' ConstDef } ';'
class ConstDeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::unique_ptr<BaseAST> const_def;
  void Dump() const override {
    b_type->Dump();
    const_def->Dump();
  }
  void Print_AST() override {
    std::cout << "ConstDeclAST:" << std::endl;
    std::cout << "Const ";
    b_type->Print_AST();
    const_def->Print_AST();
  }
  void Semantic_Analysis() override {
    b_type->Semantic_Analysis();
    const_def->Semantic_Analysis();
  }
};

// ConstDef :: IDENT "=" ConstInitVal
class ConstDefAST : public BaseAST{
  public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    std::unique_ptr<BaseAST> const_def;
  void Dump() const override {
    const_init_val->Dump();
    if (const_def){
      const_def->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "ConstDefAST:" << std::endl;
    std::cout << "IDENT:" << ident << std::endl;
    const_init_val->Print_AST();
    if (const_def){
      const_def->Print_AST();
    }
  }
  void Semantic_Analysis() override {
    const_init_val->Semantic_Analysis();
    if (const_def){
      const_def->Semantic_Analysis();
    }
  }
};

// ConstInitVal ::= ConstExp
class ConstInitValAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> const_exp;
  void Dump() const override {
    const_exp->Dump();
  }
  void Print_AST() override {
    std::cout << "ConstInitValAST:" << std::endl;
    const_exp->Print_AST();
  }
  void Semantic_Analysis() override{
    const_exp->Semantic_Analysis();
  }
};

// ConstExp ::= Exp
class ConstExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    exp->Dump();
  }
  void Print_AST() override {
    std::cout << "ConstExpAST:" << std::endl;
    exp->Print_AST();
  }
  void Semantic_Analysis() override{
    exp->Semantic_Analysis();
  }
};

// VarDecl ::= BType VarDef { ',' VarDef } ';'
class VarDeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::unique_ptr<BaseAST> var_def;
  void Dump() const override {
    if (b_type){
      b_type->Dump();
    }
    if (var_def){
      var_def->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "VarDeclAST:" << std::endl;
    b_type->Print_AST();
    var_def->Print_AST();
  }
  void Semantic_Analysis() override {
    b_type->Semantic_Analysis();
    var_def->Semantic_Analysis();
  }
};

// VarDecl_ ::= VarDef { ',' VarDef } ';'
class VarDeclAST_ : public BaseAST{
  public:
    std::unique_ptr<BaseAST> var_def;
  void Dump() const override {
    var_def->Dump();
  }
  void Print_AST() override {
    std::cout << "VarDeclAST_:" << std::endl;
    var_def->Print_AST();
  }
  void Semantic_Analysis() override{
    var_def->Semantic_Analysis();
  }
};

// VarDef ::= IDENT | IDENT "=" InitVal
class VarDefAST : public BaseAST{
  public:
    std::string ident;
    std::unique_ptr<BaseAST> init_val;
    std::unique_ptr<BaseAST> var_def;
  void Dump() const override {
    if (init_val){
      init_val->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "VarDefAST:" << std::endl;
    std::cout << "IDENT: " << ident << std::endl;
    if (init_val){
      init_val->Print_AST();
    }
    if (var_def){
      var_def->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    // redefinition
    if (current_func_symbol_table == NULL){
      if (symbol_table.symbol_map.find(ident) != symbol_table.symbol_map.end()){
        std::cout << "Error: redefinition of global variable " << ident << std::endl;
        exit(1);
      }

      if (init_val){
        symbol_table.symbol_map[ident] = {0, 0, ident + "_00"};
      }else {
        symbol_table.symbol_map[ident] = {0, 0, ident + "_00"};
      }
    }else {
      if (current_func_symbol_table->nameset.count(ident + std::string("_") + std::to_string(current_func_symbol_table->depth)) != 0){
        std::cout << "Error: redefinition of variable " << ident << std::endl;
        exit(1);
      }

      if (init_val){
        current_func_symbol_table->nameset.insert(ident + std::string("_") + std::to_string(current_func_symbol_table->depth));
        current_func_symbol_table->symbol_maps[current_func_symbol_table->depth][ident] = {0, 0, ident + std::string("_") + std::to_string(current_func_symbol_table->depth)};
      }else {
        current_func_symbol_table->nameset.insert(ident + std::string("_") + std::to_string(current_func_symbol_table->depth));
        current_func_symbol_table->symbol_maps[current_func_symbol_table->depth][ident] = {0, 0, ident + std::string("_") + std::to_string(current_func_symbol_table->depth)};
      }
    }

    if (init_val){
      init_val->Semantic_Analysis();
    }
    if (var_def){
      var_def->Semantic_Analysis();
    }
  }
};

// InitVal ::= Exp
class InitValAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    exp->Dump();
  }
  void Print_AST() override {
    std::cout << "InitValAST:" << std::endl;
    exp->Print_AST();
  }
  void Semantic_Analysis() override{
    if (exp){
      exp->Semantic_Analysis();
    }
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
    func_type->Dump();
    if(func_f_params){
      func_f_params->Dump();
    }
    block->Dump();
  }
  void Print_AST() override {
    std::cout << "FuncDefAST:" << std::endl;
    func_type->Print_AST();
    std::cout << "IDENT:" << ident << std::endl;
    if(func_f_params){
      func_f_params->Print_AST();
    }
    block->Print_AST();
  }
  void Semantic_Analysis() override {
    func_type->Semantic_Analysis();
    if(func_f_params){
      func_f_params->Semantic_Analysis();
    }
    block->Semantic_Analysis();
  }
};

// FuncType ::= "int" | "void"
class FuncTypeAST : public BaseAST{
  public:
    std::string type;
  void Print_AST() override {
    std::cout << "FunctypeAST:" << std::endl;
    std::cout << type;
  }

};

// FuncFParams ::= FuncFParam { ',' FuncFParam }
class FuncFParamsAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> func_f_param;
  void Dump() const override {
    func_f_param->Dump();
  }
  void Print_AST() override {
    std::cout << "FuncFParamsAST:" << std::endl;
    func_f_param->Print_AST();
  }
  void Semantic_Analysis() override{
    func_f_param->Semantic_Analysis();
  }
};

// FuncFParam ::= BType IDENT
class FuncFParamAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::string ident;
    std::unique_ptr<BaseAST> func_f_param;
  void Dump() const override {
    b_type->Dump();
    if (func_f_param){
      func_f_param->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "FuncFParamAST:" << std::endl;
    b_type->Print_AST();
    std::cout << "IDENT: " << ident << std::endl;
    if (func_f_param){
      func_f_param->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    b_type->Semantic_Analysis();
    if (func_f_param){
      func_f_param->Semantic_Analysis();
    }
  }
};

// Block ::= '{' { BlockItem } '}'
class BlockAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> blockitem;

  void Dump() const override {
    blockitem->Dump();
  }
  void Print_AST() override {
    std::cout << "BlockAST:" << std::endl;
    blockitem->Print_AST();
  }
  void Semantic_Analysis() override{
    current_func_symbol_table->depth++;
    SymbolMap m;
    current_func_symbol_table->symbol_maps.push_back(m);
    blockitem->Semantic_Analysis();
    current_func_symbol_table->symbol_maps.pop_back();
    current_func_symbol_table->depth--;
  }
};

// BlockItem ::= Decl | Stmt
class BlockItemAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> decl;
    std::unique_ptr<BaseAST> stmt;
    std::unique_ptr<BaseAST> block_item;
  void Dump() const override {
    if (decl){
      decl->Dump();
    }
    if (stmt){
      stmt->Dump();
    }
    if (block_item){
      block_item->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "BlockItemAST:" << std::endl;
    if (decl){
      decl->Print_AST();
    }
    if (stmt){
      stmt->Print_AST();
    }
    if (block_item){
      block_item->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (decl){
      decl->Semantic_Analysis();
    }
    if (stmt){
      stmt->Semantic_Analysis();
    }
    if (block_item){
      block_item->Semantic_Analysis();
    }
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
    if (symbol == "if"){
      if(stmt_2){
        exp->Dump();
        stmt_1->Dump();
        stmt_2->Dump();
      }else {
        exp->Dump();
        stmt_1->Dump();
      }
    }else if (symbol == "while"){
      exp->Dump();
      stmt_1->Dump();
    }else if (symbol == "return"){
      if (exp){
        exp->Dump();
      }
    }else if (symbol == "break"){
     
    }else if (symbol == "continue"){
     
    }else if (l_val && exp){
      l_val->Dump();
      exp->Dump();
    }else if (block){
      block->Dump();
    }else if(exp){
      exp->Dump();
    }
  }

  void Print_AST() override{
    std::cout << "StmtAST:" << std::endl;
    if (symbol == "if"){
      if(stmt_2){
        exp->Print_AST();
        stmt_1->Print_AST();
        stmt_2->Print_AST();
      }else {
        exp->Print_AST();
        stmt_1->Print_AST();
      }
    }else if (symbol == "while"){
      exp->Print_AST();
      stmt_1->Print_AST();
    }else if (symbol == "return"){
      if (exp){
        exp->Print_AST();
      }
    }else if (symbol == "break"){
    }else if (symbol == "continue"){
    }else if (l_val && exp){
      l_val->Print_AST();
      exp->Print_AST();
    }else if (block){
      block->Print_AST();
    }else if(exp){
      exp->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (symbol == "if"){
      if(stmt_2){
        exp->Semantic_Analysis();
        stmt_1->Semantic_Analysis();
        stmt_2->Semantic_Analysis();
      }else {
        exp->Semantic_Analysis();
        stmt_1->Semantic_Analysis();
      }
    }else if (symbol == "while"){
      exp->Semantic_Analysis();
      stmt_1->Semantic_Analysis();
    }else if (symbol == "return"){
      if (exp){
        exp->Semantic_Analysis();
      }
    }else if (symbol == "break"){
    }else if (symbol == "continue"){
    }else if (l_val && exp){
      l_val->Semantic_Analysis();
      exp->Semantic_Analysis();
    }else if (block){
      block->Semantic_Analysis();
    }else if(exp){
      exp->Semantic_Analysis();
    }
  }
};

// Exp ::= LOrExp;
class ExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> l_or_exp;
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    if (l_or_exp){
      l_or_exp->Dump();
    }
    if (exp){
      exp->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "ExpAST:" << std::endl;
    if (l_or_exp){
      l_or_exp->Print_AST();
    }
    if (exp){
      exp->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (l_or_exp){
      l_or_exp->Semantic_Analysis();
    }
    if (exp){
      exp->Semantic_Analysis();
    }
  }
};

// LVal ::= IDENT
class LValAST : public BaseAST{
  public:
    std::string ident;
  void Dump() const override {
  }
  void Print_AST() override {
    std::cout << "LValAST:" << std::endl;
    std::cout << "IDENT: " << ident << std::endl;
  }
  void Semantic_Analysis() override{
    // undefinition
    if (current_func_symbol_table){
      // for i int vector symbol_map
      if (current_func_symbol_table->symbol_maps[1].find(ident) == current_func_symbol_table->symbol_maps[1].end()
          && symbol_table.symbol_map.find(ident) == symbol_table.symbol_map.end()){
        std::cout << "Error: undefined variable " << ident << "." << std::endl;
        exit(1);
      }else {
        return;
      }
    }else if(symbol_table.symbol_map.find(ident) == symbol_table.symbol_map.end()){
      std::cout << "Error: undefined global variable " << ident << "." << std::endl;
      exit(1);
    }
  }
};

// PrimaryExp ::= "(" Exp ")" | Number | LVal;
class PrimaryExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
    std::unique_ptr<BaseAST> number;
    std::unique_ptr<BaseAST> l_val;
  void Dump() const override {
    if (exp){
      exp->Dump();
    }
    if (number){
      number->Dump();
    }
    if (l_val) {
      l_val->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "PrimaryExpAST:" << std::endl;
    if (exp){
      exp->Print_AST();
    }
    if (number){
      number->Print_AST();
    }
    if (l_val) {
      l_val->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (exp){
      exp->Semantic_Analysis();
    }
    if (number){
      number->Semantic_Analysis();
    }
    if (l_val) {
      l_val->Semantic_Analysis();
    }
  }
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST{
  public:
    std::string number;
  void Dump() const override {
  }
  void Print_AST() override {
    std::cout << "INT_CONST: " << number << std::endl;
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
    std::string ident;
    std::unique_ptr<BaseAST> unary_exp;
    std::unique_ptr<BaseAST> func_r_params;
  void Dump() const override {
    if (primary_exp){
      primary_exp->Dump();
    }
    if (unary_exp){
      unary_exp->Dump();
    }
    if (func_r_params) {
      func_r_params->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "UnaryExpAST" << std::endl;
    if (primary_exp){
      primary_exp->Print_AST();
    }
    if (unary_exp){
      unary_exp->Print_AST();
    }
    if (ident != ""){
      std::cout << "IDENT: " << ident << std::endl;
    }
    if (func_r_params) {
      func_r_params->Print_AST();
    }
  }
  void Semantic_Analysis() override {
    // undefiniton check
    if (ident != ""){
      if (symbol_table.func_symbol_map.find(ident) == symbol_table.func_symbol_map.end()){
        std::cout << "Error: undefiniton of function " << ident << "." << std::endl;
        exit(1);
      }    
    }
    if (primary_exp){
      primary_exp->Semantic_Analysis();
    }
    if (unary_exp){
      unary_exp->Semantic_Analysis();
    }
    if (func_r_params) {
      func_r_params->Semantic_Analysis();
    }
  }
};

// UnaryOp ::= '+' | '-' | '!' ;
class UnaryOpAST : public BaseAST{
  public:
    std::string unary_op;
  void Print_AST() override {
    std::cout << unary_op;
  }
};

// FuncRParams ::= Exp { ',' Exp }
class FuncRParamsAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    exp->Dump();
  }
  void Print_AST() override {
    std::cout << "FuncRParamsAST:" << std::endl;
    exp->Print_AST();
  }
  void Semantic_Analysis() override{
    exp->Semantic_Analysis();
  }
};

// MulExp ::= UnaryExp | MulExp ( '*' | '/' | '%' ) UnaryExp;
class MulExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> mul_exp;
    std::string mul_op;
    std::unique_ptr<BaseAST> unary_exp;
  void Dump() const override {
    if (mul_exp){
      mul_exp->Dump();
      unary_exp->Dump();
    }
    else if(unary_exp){
      unary_exp->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "MulExpAST:" << std::endl;
    if (mul_exp){
      mul_exp->Print_AST();
      unary_exp->Print_AST();
    }
    else if(unary_exp){
      unary_exp->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (mul_exp){
      mul_exp->Semantic_Analysis();
      unary_exp->Semantic_Analysis();
    }
    else if(unary_exp){
      unary_exp->Semantic_Analysis();
    }
  }
};

// AddExp ::= MulExp | AddExp ( '+' | '-') MulExp;
class AddExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> add_exp;
    std::string add_op;
    std::unique_ptr<BaseAST> mul_exp;
  void Dump() const override {
    if (add_exp){      
        add_exp->Dump();
        mul_exp->Dump();
    }
    else if (mul_exp){
      mul_exp->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "AddExpAST:" << std::endl;
    if (add_exp){      
        add_exp->Print_AST();
        mul_exp->Print_AST();
    }
    else if (mul_exp){
      mul_exp->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (add_exp){
      add_exp->Semantic_Analysis();
      mul_exp->Semantic_Analysis();
    }
    else if (mul_exp){
      mul_exp->Semantic_Analysis();
    }
  }
};

// RelExp ::= AddExp | RelExp ( "<" | ">" | "<=" | ">=" ) AddExp;
class RelExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> rel_exp;
    std::string rel_op;
    std::unique_ptr<BaseAST> add_exp;
  void Dump() const override {
    if (rel_exp){
      rel_exp->Dump();
      add_exp->Dump();
    }
    else if(add_exp){
      add_exp->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "RelExpAST:" << std::endl;
    if (rel_exp){
      rel_exp->Print_AST();
      add_exp->Print_AST();
    }
    else if(add_exp){
      add_exp->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (rel_exp){
      rel_exp->Semantic_Analysis();
      add_exp->Semantic_Analysis();
    }
    else if(add_exp){
      add_exp->Semantic_Analysis();
    }
  }
};

// EqExp ::= RelExp | EqExp ( "==" | "!=" ) RelExp;
class EqExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> eq_exp;
    std::string eq_op;
    std::unique_ptr<BaseAST> rel_exp;
  void Dump() const override {
    if (eq_exp){
      eq_exp->Dump();
      rel_exp->Dump();
    }
    else if(rel_exp){
      rel_exp->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "EqExpAST:" << std::endl;
    if (eq_exp){
      eq_exp->Print_AST();
      rel_exp->Print_AST();
    }
    else if(rel_exp){
      rel_exp->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (eq_exp){
      eq_exp->Semantic_Analysis();
      rel_exp->Semantic_Analysis();
    }
    else if(rel_exp){
      rel_exp->Semantic_Analysis();
    }
  }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp;
class LAndExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> l_and_exp;
    std::string l_and_op;
    std::unique_ptr<BaseAST> eq_exp;
  void Dump() const override {
    if (l_and_exp){
      l_and_exp->Dump();
      eq_exp->Dump();
    } else if(eq_exp){
      eq_exp->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "LAndExpAST:" << std::endl;
    if (l_and_exp){
      l_and_exp->Print_AST();
      eq_exp->Print_AST();
    } else if(eq_exp){
      eq_exp->Print_AST();
    }
  }  
  void Semantic_Analysis() override {
    if (l_and_exp){
      l_and_exp->Semantic_Analysis();
      eq_exp->Semantic_Analysis();
    } else if(eq_exp){
      eq_exp->Semantic_Analysis();
    }
  }
};

// LOrExp ::= LAndExp | LOrExp "||" LAndExp;
class LOrExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> l_or_exp;
    std::string l_or_op;
    std::unique_ptr<BaseAST> l_and_exp;
  void Dump() const override {

    if (l_or_exp){
      l_or_exp->Dump();
      l_and_exp->Dump();
    } else if(l_and_exp){
      l_and_exp->Dump();
    }
  }
  void Print_AST() override {
    std::cout << "LOrExpAST:" << std::endl;
    if (l_or_exp){
      l_or_exp->Print_AST();
      l_and_exp->Print_AST();
    } else if(l_and_exp){
      l_and_exp->Print_AST();
    }
  }
  void Semantic_Analysis() override{
    if (l_or_exp){
      l_or_exp->Semantic_Analysis();
      l_and_exp->Semantic_Analysis();
    } else if(l_and_exp){
      l_and_exp->Semantic_Analysis();
    }
  }
};