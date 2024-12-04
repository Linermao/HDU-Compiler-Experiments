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

static int debug = 0;
static int now = 0;


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
  std::vector<SymbolMap> symbol_map;
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

static int ENTRY_COUNT = 0;
static int IF_COUNT=0;
static int WHILE_COUNT=0;
static int OTHER_COUNT=0;

// 所有 AST 的基类
class BaseAST {
 public:
  virtual ~BaseAST() = default;
	virtual void Dump() const = 0;
  virtual int Calculate(){
    return 0;
  }
};	

// CompUnit 是 BaseAST
// CompUnit ::= CompUnits
class CompUnitAST : public BaseAST{
public:
  std::unique_ptr<BaseAST> comp_units;
	void Dump() const override {
      if (debug){
        std::cout << "<!!!! DEBUG MODE ON !!!!>" << std::endl;
        std::cout << "CompUnitAST:" << std::endl;
      }

      std::cout << "decl @getint(): i32" << std::endl;
      std::cout << "decl @getch(): i32" << std::endl;
      std::cout << "decl @getarray(*i32): i32" << std::endl;
      std::cout << "decl @putint(i32)" << std::endl;
      std::cout << "decl @putch(i32)" << std::endl;
      std::cout << "decl @putarray(i32, *i32)" << std::endl;
      std::cout << "decl @starttime()" << std::endl;
      std::cout << "decl @stoptime()" << std::endl;

      symbol_table.symbol_map["getint"] = {0, 0, "getint"};
      symbol_table.symbol_map["getch"] = {0, 0, "getch"};
      symbol_table.symbol_map["getarray"] = {0, 0, "getarray"};
      symbol_table.symbol_map["putint"] = {0, 0, "putint"};
      symbol_table.symbol_map["putch"] = {0, 0, "putch"};
      symbol_table.symbol_map["putarray"] = {0, 0, "putarray"};
      symbol_table.symbol_map["starttime"] = {0, 0, "starttime"};
      symbol_table.symbol_map["stoptime"] = {0, 0, "stoptime"};

      if (comp_units){
        comp_units->Dump();
      }

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
      if (debug){
        std::cout << "CompUnitsAST:" << std::endl;
      }
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
};

// FuncDef_ ::= IDENT '(' [FuncFParams] ')' Block
class FuncDefAST_ : public BaseAST {
 public:
  std::string func_type;
  std::string ident;
  std::unique_ptr<BaseAST> func_f_params;
  std::unique_ptr<BaseAST> block;

  void Dump() const override {
    if (debug){
      std::cout << "FuncDefAST_:" << std::endl;
    }
    // redefinition check
    if (symbol_table.func_symbol_map.find(ident) != symbol_table.func_symbol_map.end()){
      std::cout << "Error: redefinition of function " << ident << std::endl;
      exit(1);
    }

    std::cout << "fun @" << ident << "(";
    if(func_f_params){
      func_f_params->Dump();
    }
    std::cout << ")" ;
    if(func_type == "int"){
      std::cout << ": i32 ";
    }else{
      std::cout << " ";
    }
    std::cout << "{" << std::endl;
    std::cout << "%entry_" << ENTRY_COUNT++ << ":" << std::endl;

    
    symbol_table.func_symbol_map[ident] = std::make_unique<func_symbol>();

    current_func_symbol_table = symbol_table.func_symbol_map[ident].get();
    SymbolMap m;
    current_func_symbol_table->symbol_map.push_back(m);
    current_func_symbol_table->block_end = 0;
    // 
    block->Dump();

    if (current_func_symbol_table->block_end == 0){
      if (func_type == "int"){
        now++;
        std::cout << "  %" << now << " = add 0, 0" << std::endl;
        std::cout << "  ret %" << now << std::endl;
      }else {
        std::cout << "  ret" << std::endl;
      }
      current_func_symbol_table->block_end = 1;
    }

    std::cout << "}" << std::endl;
    std::cout << std::endl;
    current_func_symbol_table->symbol_map.pop_back();
    current_func_symbol_table = NULL;
  }
};

// BType ::= "int"
class BTypeAST : public BaseAST{
  public:
    std::string type;
  void Dump() const override {
    if (debug) {
      std::cout << "BTypeAST:" << std::endl;
    }
    // std::cout << type << std::endl;
  }
};

// FuncDefOrVarDecl ::= BType FuncDef_ | BType VarDecl_
class FuncDefOrVarDeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::unique_ptr<BaseAST> func_def;
    std::unique_ptr<BaseAST> var_decl;
  void Dump() const override {
    if (debug){
      std::cout << "FuncDefOrVarDeclAST:" << std::endl;
    }
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
};

// Decl ::= ConstDecl | VarDecl
class DeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> const_decl;
    std::unique_ptr<BaseAST> var_decl;
  void Dump() const override {
    if (debug){
      std::cout << "DeclAST:" << std::endl;
    }
    if (const_decl){
      const_decl->Dump();
    }
    if (var_decl){
      var_decl->Dump();
    }
  }
};

// ConstDecl ::= "const" BType ConstDef { ',' ConstDef } ';'
class ConstDeclAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> b_type;
    std::unique_ptr<BaseAST> const_def;
  void Dump() const override {
    if (debug){
      std::cout << "ConstDeclAST:" << std::endl;
    }
    b_type->Dump();
    const_def->Dump();
  }
};

// ConstDef :: IDENT "=" ConstInitVal
class ConstDefAST : public BaseAST{
  public:
    std::string ident;
    std::unique_ptr<BaseAST> const_init_val;
    std::unique_ptr<BaseAST> const_def;
  void Dump() const override {
    if (debug){
      std::cout << "ConstDefAST:" << std::endl;
    }
    std::cout << "IDENT:" << ident << std::endl;
    const_init_val->Dump();
    if (const_def){
      const_def->Dump();
    }
  }
};

// ConstInitVal ::= ConstExp
class ConstInitValAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> const_exp;
  void Dump() const override {
    if (debug){
      std::cout << "ConstInitValAST:" << std::endl;
    }
    const_exp->Dump();
  }
};

// ConstExp ::= Exp
class ConstExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    std::cout << "ConstExpAST { " << std::endl;
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
    if (debug){
      std::cout << "VarDeclAST:" << std::endl;
    }
    if (b_type){
      b_type->Dump();
    }
    if (var_def){
      var_def->Dump();
    }
  }
};

// VarDecl_ ::= VarDef { ',' VarDef } ';'
class VarDeclAST_ : public BaseAST{
  public:
    std::unique_ptr<BaseAST> var_def;
  void Dump() const override {
    if (debug){
      std::cout << "VarDeclAST_:" << std::endl;
    }
    var_def->Dump();
  }
};

// VarDef ::= IDENT | IDENT "=" InitVal
class VarDefAST : public BaseAST{
  public:
    std::string ident;
    std::unique_ptr<BaseAST> init_val;
    std::unique_ptr<BaseAST> var_def;
  void Dump() const override {
    if (debug){
      std::cout << "VarDefAST:" << std::endl;
    }
    // global var
    if (current_func_symbol_table == NULL){
      // redefinition
      if (symbol_table.symbol_map.find(ident) != symbol_table.symbol_map.end()){
        std::cout << "Error: redefinition of global variable " << ident << std::endl;
        exit(1);
      }

      if (init_val){
        symbol_table.symbol_map[ident] = {0, 0, ident + "_00"};
        std::cout << "global @" << ident + "_00" << " = alloc i32, " << init_val->Calculate() << std::endl;
      }else {
        symbol_table.symbol_map[ident] = {0, 0, ident + "_00"};
        std::cout << "global @" << ident + "_00" << " = alloc i32, zeroinit" << std::endl;
      }
    }else {
      if (current_func_symbol_table->nameset.count(ident + std::string("_") + std::to_string(current_func_symbol_table->depth)) != 0){
        std::cout << "Error: redefinition of variable " << ident << std::endl;
        exit(1);
      }

      if (init_val){
        current_func_symbol_table->nameset.insert(ident + std::string("_") + std::to_string(current_func_symbol_table->depth));
        current_func_symbol_table->symbol_map[current_func_symbol_table->depth][ident] = {0, 0, ident + std::string("_") + std::to_string(current_func_symbol_table->depth)};
        
        std::cout << "  @" << ident << "_" << current_func_symbol_table->depth << " = alloc i32" << std::endl;
        init_val->Dump();
        std::cout << "  store %" << now << ", @" << ident << "_" << current_func_symbol_table->depth << std::endl;
      }else {
        if (current_func_symbol_table->nameset.count(ident + std::string("_") + std::to_string(current_func_symbol_table->depth)) == 0) {
          current_func_symbol_table->nameset.insert(ident + std::string("_") + std::to_string(current_func_symbol_table->depth));
          std::cout << "  @" << ident << "_" << current_func_symbol_table->depth << " = alloc i32" << std::endl;
        }
        current_func_symbol_table->symbol_map[current_func_symbol_table->depth][ident] = {0, 0, ident + std::string("_") + std::to_string(current_func_symbol_table->depth)};
        std::cout << "  store 0, @" << ident << "_" << current_func_symbol_table->depth << std::endl;
      }
    }
  }
};

// InitVal ::= Exp
class InitValAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    if (debug){
      std::cout << "InitValAST:" << std::endl;
    }
    exp->Dump();
  }
  int Calculate() override{
    if (exp){
      return exp->Calculate();
    }
    return 0;
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
    std::cout << "FuncDefAST { " << std::endl;
    func_type->Dump();
    std::cout << "IDENT : " << ident << std::endl;
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
    if (debug){
      std::cout << "FuncTypeAST:" << std::endl;
    }
    if (type == "int"){
      std::cout << ": i32" << std::endl;
    } else{
      std::cout << " " << std::endl;
    }
  }
};

// FuncFParams ::= FuncFParam { ',' FuncFParam }
class FuncFParamsAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> func_f_param;
  void Dump() const override {
    std::cout << "FuncFParamsAST { " << std::endl;
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
    std::cout << "FuncFParamAST { " << std::endl;
    b_type->Dump();
    std::cout << "IDENT : " << ident << std::endl;
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
    if (debug){
      std::cout << "BlockAST:" << std::endl;
    }
    current_func_symbol_table->depth++;
    SymbolMap m;
    current_func_symbol_table->symbol_map.push_back(m);
    blockitem->Dump();
    current_func_symbol_table->symbol_map.pop_back();
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
    if (debug){
      std::cout << "BlockItemAST:" << std::endl;
    }
    if (decl || stmt){
      if (current_func_symbol_table->block_end == 1){
        current_func_symbol_table->block_end = 0;
        std::cout << "%other_" << OTHER_COUNT << ":" << std::endl;
      }
    }
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
    if (debug){
      std::cout << "StmtAST:" << std::endl;
    }
    if (symbol == "if"){
      if(stmt_2){
        int ifcount = IF_COUNT++;
        exp->Dump();
        std::cout << "  br "  << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;
        std::cout << "%" << "then_" << ifcount << ":" << std::endl;
        current_func_symbol_table->block_end = 0;

        stmt_1->Dump();
        if (current_func_symbol_table->block_end == 0){
          current_func_symbol_table->block_end = 1;
          std::cout << "  jump " << "%" << "end_" << ifcount << std::endl;
        }
        std::cout << "%" << "else_" << ifcount << ":" << std::endl;
        current_func_symbol_table->block_end = 0;
        stmt_2->Dump();

        if (current_func_symbol_table->block_end == 0){
          current_func_symbol_table->block_end = 1;
          std::cout << "  jump " << "%" << "end_" << ifcount << std::endl;
        }
        std::cout << "%" << "end_" << ifcount << ":" << std::endl;
        current_func_symbol_table->block_end = 0;

      }else {
        exp->Dump();
        stmt_1->Dump();
      }
    }else if (symbol == "while"){
      int whilecount = WHILE_COUNT++;
      current_func_symbol_table->loop_stack.push(whilecount);
      std::cout << "  jump %while_entry_" << whilecount << std::endl;
      std::cout << "%while_entry_" << whilecount << ":" << std::endl;
      current_func_symbol_table->block_end = 0;
      exp->Dump();

      std::cout << "  br " << ", %while_body_" << whilecount << ", %while_end_" << whilecount << std::endl;
      std::cout << "%while_body_" << whilecount << ":" << std::endl;
      current_func_symbol_table->block_end = 0;
      stmt_1->Dump();

      if (current_func_symbol_table->block_end == 0){
        std::cout << "  jump %while_entry_" << whilecount << std::endl;
      }

      std::cout << "%while_end_" << whilecount << ":" << std::endl;
      current_func_symbol_table->block_end = 0;
      current_func_symbol_table->loop_stack.pop();
    }else if (symbol == "return"){
      if (exp){
        exp->Dump();
        std::cout << "  ret" << std::endl;
        current_func_symbol_table->block_end = 1;
      }else {
        std::cout << "  ret" << std::endl;
        current_func_symbol_table->block_end = 1;
      }
    }else if (symbol == "break"){
      if (current_func_symbol_table->block_end == 0){
        current_func_symbol_table->block_end = 1;
        std::cout << "  jump %while_end_" << current_func_symbol_table->loop_stack.top() << std::endl;
      }
    }else if (symbol == "continue"){
      if (current_func_symbol_table->block_end == 0){
        current_func_symbol_table->block_end = 1;
        std::cout << "  jump %while_entry_" << current_func_symbol_table->loop_stack.top() << std::endl;
      }
    }else if (l_val && exp){
      l_val->Dump();
      exp->Dump();
    }else if (block){
      block->Dump();
    }else if(exp){
      exp->Dump();
    }
  }
};

// Exp ::= LOrExp;
class ExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> l_or_exp;
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    if (debug){
      std::cout << "ExpAST:" << std::endl;
    }
    if (l_or_exp){
      l_or_exp->Dump();
    }
    if (exp){
      exp->Dump();
    }
  }
  int Calculate() override{
    if (l_or_exp){
      return l_or_exp->Calculate();
    }else if (exp){
      return exp->Calculate();
    }
    return 0;
  } 
};

// LVal ::= IDENT
class LValAST : public BaseAST{
  public:
    std::string ident;
  void Dump() const override {
    if(debug){
      std::cout << "LValAST:" << std::endl;
    }
    std::map<std::string, Symbol>::iterator it;
    if (current_func_symbol_table == NULL){
      it = symbol_table.symbol_map.find(ident);
    }else {
      for (int d = current_func_symbol_table->depth; d>=0; d--){
        if ((it = current_func_symbol_table->symbol_map[d].find(ident)) != current_func_symbol_table->symbol_map[d].end()){
          break;
        }
      }
      if (it == current_func_symbol_table->symbol_map[0].end()){
        it = symbol_table.symbol_map.find(ident);
      }
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
    if (debug){
      std::cout << "PrimaryExpAST:" << std::endl;
    }
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
  int Calculate() override {
    if (exp){
      return exp->Calculate();
    }else if (number){
      return number->Calculate();
    }
    return 0;
  }
};

// Number ::= INT_CONST;
class NumberAST : public BaseAST{
  public:
    std::string number;
  void Dump() const override {
    if (debug){
      std::cout << "NumberAST:" << std::endl;
    }
    std::cout << "  %" << ++now;
    std::cout << " = add 0, " << number << std::endl;
  }
  int Calculate() override {
    return std::stoi(number);
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
    if(debug){
      std::cout << "UnaryExpAST:" << std::endl;
    }
    if (primary_exp){
      primary_exp->Dump();
    }
    if (unary_exp){
      unary_exp->Dump();
      if (unary_op == "-"){
        std::cout << "  %" << now + 1 << " = sub 0, %" << now++ << std::endl;
      }else if (unary_op == "!"){
        std::cout << "  %" << now + 1 << " = eq %" << now++ << ", 0" << std::endl;
      }
    }
    if (func_r_params) {
      func_r_params->Dump();
    }
  }
  int Calculate() override {
    if(primary_exp){
      return primary_exp->Calculate();
    }else if (unary_exp){
      return unary_exp->Calculate();
    }else if (func_r_params){
      return func_r_params->Calculate();
    }
    return 0;
  }
};

// UnaryOp ::= '+' | '-' | '!' ;
class UnaryOpAST : public BaseAST{
  public:
    std::string unary_op;
  void Dump() const override {
    if (debug){
      std::cout << "UnaryOpAST:" << std::endl;
    }
    std::cout << "UnaryOp : " << unary_op << std::endl;
  }
};

// FuncRParams ::= Exp { ',' Exp }
class FuncRParamsAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> exp;
  void Dump() const override {
    if (debug){
      std::cout << "FuncRParamsAST:" << std::endl;
    }
    exp->Dump();
  }
};

// MulExp ::= UnaryExp | MulExp ( '*' | '/' | '%' ) UnaryExp;
class MulExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> mul_exp;
    std::string mul_op;
    std::unique_ptr<BaseAST> unary_exp;
  void Dump() const override {
    if (debug){
      std::cout << "MulExpAST:" << std::endl;
    }
    if (mul_exp){
      mul_exp->Dump();
      unary_exp->Dump();
      if (mul_op == "*"){
        std::cout << "  %" << now + 1 << " = mul " << "l" << ", " << "r" << std::endl;
      }else if (mul_op == "/"){
        std::cout << "  %" << now + 1 << " = div " << "l" << ", " << "r" << std::endl;
      }else if (mul_op == "%"){
        std::cout << "  %" << now + 1 << " = mod " << "l" << ", " << "r" << std::endl;
      }
    }
    else if(unary_exp){
      unary_exp->Dump();
    }
  }
  int Calculate() override {
    if(unary_exp){
      return unary_exp->Calculate();
    }
    return 0;
  }
};

// AddExp ::= MulExp | AddExp ( '+' | '-') MulExp;
class AddExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> add_exp;
    std::string add_op;
    std::unique_ptr<BaseAST> mul_exp;
  void Dump() const override {
    if (debug){
      std::cout << "AddExpAST:" << std::endl;
    }
    if (add_exp){      
        add_exp->Dump();
        mul_exp->Dump();
        if (add_op == "+"){
          std::cout << "  %" << now + 1 << " = add " << "l" << ", " << "r" << std::endl;
        }else if (add_op == "-"){
          std::cout << "  %" << now + 1 << " = sub " << "l" << ", " << "r" << std::endl;
        }
    }
    else if (mul_exp){
      mul_exp->Dump();
    }
  }
  int Calculate() override {
    if(mul_exp){
      return mul_exp->Calculate();
    }
    return 0;
  }
};

// RelExp ::= AddExp | RelExp ( "<" | ">" | "<=" | ">=" ) AddExp;
class RelExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> rel_exp;
    std::string rel_op;
    std::unique_ptr<BaseAST> add_exp;
  void Dump() const override {
    if (debug){
      std::cout << "RelExpAST:" << std::endl;
    }
    if (rel_exp){
      rel_exp->Dump();
      add_exp->Dump();
      if (rel_op == "<"){
        std::cout << "  %" << now + 1 << " = lt " << "l" << ", " << "r" << std::endl;
      }else if (rel_op == ">"){
        std::cout << "  %" << now + 1 << " = gt " << "l" << ", " << "r" << std::endl;
      }else if (rel_op == "<="){
        std::cout << "  %" << now + 1 << " = le " << "l" << ", " << "r" << std::endl;
      }else{
        std::cout << "  %" << now + 1 << " = ge " << "l" << ", " << "r" << std::endl;
      }
      now++;
    }
    else if(add_exp){
      add_exp->Dump();
    }
  }
  int Calculate() override {
    if(add_exp){
      return add_exp->Calculate();
    }
    return 0;
  }
};

// EqExp ::= RelExp | EqExp ( "==" | "!=" ) RelExp;
class EqExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> eq_exp;
    std::string eq_op;
    std::unique_ptr<BaseAST> rel_exp;
  void Dump() const override {
    if (debug){
      std::cout << "EqExpAST:" << std::endl;
    }
    if (eq_exp){
      eq_exp->Dump();
      rel_exp->Dump();
      if (eq_op == "=="){
        std::cout << "  %" << now + 1 << " = eq " << "l" << ", " << "r" << std::endl;
      }else{
        std::cout << "  %" << now + 1 << " = ne " << "l" << ", " << "r" << std::endl;
      }
      now++;
    }
    else if(rel_exp){
      rel_exp->Dump();
    }
  }
  int Calculate() override {
    if(rel_exp){
      return rel_exp->Calculate();
    }
    return 0;
  }
};

// LAndExp ::= EqExp | LAndExp "&&" EqExp;
class LAndExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> l_and_exp;
    std::string l_and_op;
    std::unique_ptr<BaseAST> eq_exp;
  void Dump() const override {
    if (debug){
      std::cout << "LAndExpAST:" << std::endl;
    }
    if (l_and_exp){
      int ifcount = IF_COUNT++;
      l_and_exp->Dump();
      std::cout << "  @int" << ifcount << " = alloc i32" << std::endl;
      std::cout << "  br " << "l" << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      current_func_symbol_table->block_end = 0;
      eq_exp->Dump();
      
      std::cout << "  %" << ++now << " = ne " << ", 0" << std::endl;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;
      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;

      current_func_symbol_table->block_end = 0;
      std::cout << "  %" << ++now << " = ne " << "l" << ", 0" << std::endl;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      current_func_symbol_table->block_end = 0;
      std::cout << "  %" << ++now << " = load @int" << ifcount << std::endl;
    } else if(eq_exp){
      eq_exp->Dump();
    }
  }
  int Calculate() override {
    if(eq_exp){
      return eq_exp->Calculate();
    }
    return 0;
  }
};

// LOrExp ::= LAndExp | LOrExp "||" LAndExp;
class LOrExpAST : public BaseAST{
  public:
    std::unique_ptr<BaseAST> l_or_exp;
    std::string l_or_op;
    std::unique_ptr<BaseAST> l_and_exp;
  void Dump() const override {
    if (debug){
      std::cout << "LOrExpAST:" << std::endl;
    }
    if (l_or_exp){
      int ifcount = IF_COUNT++;
      l_or_exp->Dump();
      std::cout << "  @int" << ifcount << " = alloc i32" << std::endl;
      std::cout << "  br " << "l" << ", %" << "then_" << ifcount << ", %" << "else_" << ifcount << std::endl;

      std::cout << "%" << "then_" << ifcount << ":" << std::endl;
      current_func_symbol_table->block_end = 0;
      
      std::cout << "  %" << ++now << " = ne " << "l" << ", 0" << std::endl;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;
      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "else_" << ifcount << ":" << std::endl;

      current_func_symbol_table->block_end = 0;
      std::cout << "  %" << now + 1 << " = ne " << "r" << ", 0" << std::endl;
      now++;
      std::cout << "  store %" << now << ", @int" << ifcount << std::endl;

      std::cout << "  jump %end_" << ifcount << std::endl;

      std::cout << "%" << "end_" << ifcount << ":" << std::endl;
      current_func_symbol_table->block_end = 0;
      std::cout << "  %" << ++now << " = load @int" << ifcount << std::endl;

    } else if(l_and_exp){
      l_and_exp->Dump();
    }
  }

  int Calculate () override {
    if(l_and_exp){
      return l_and_exp->Calculate();
    }
    return 0;
  }
};



