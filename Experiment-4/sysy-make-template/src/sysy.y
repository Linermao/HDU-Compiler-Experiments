%code requires {
  #include <memory>
  #include <string>
  #include "AST.h"
}

%{

#include <iostream>
#include <memory>
#include <string>
#include "AST.h"

// 声明 lexer 函数和错误处理函数
int yylex();
void yyerror(std::unique_ptr<BaseAST> &ast, const char *s);


using namespace std;

%}

// 定义 parser 函数和错误处理函数的附加参数
// 我们需要返回一个字符串作为 AST, 所以我们把附加参数定义成字符串的智能指针
// 解析完成后, 我们要手动修改这个参数, 把它设置成解析得到的字符串
%parse-param { std::unique_ptr<BaseAST> &ast }

// yylval 的定义, 我们把它定义成了一个联合体 (union)
// 因为 token 的值有的是字符串指针, 有的是整数
// 之前我们在 lexer 中用到的 str_val 和 int_val 就是在这里被定义的
// 至于为什么要用字符串指针而不直接用 string 或者 unique_ptr<string>?
// 请自行 STFW 在 union 里写一个带析构函数的类会出现什么情况
%union {
  std::string *str_val;
  int int_val;
  BaseAST *ast_val;
}

// lexer 返回的所有 token 种类的声明
// 注意 IDENT 和 INT_CONST 会返回 token 的值, 分别对应 str_val 和 int_val
%token <str_val> IDENT INT RETURN CONST VOID LE GE EQ NE AND OR IF THEN ELSE WHILE CONTINUE BREAK
%token <int_val> INT_CONST

// 非终结符的类型定义
%type <ast_val> CompUnits FuncDef FuncDef_ FuncDefOrVarDecl FuncType FuncFParams FuncFParam FuncRParams Block Stmt BlockItem Number Exp PrimaryExp UnaryExp AddExp MulExp RelExp EqExp LAndExp LOrExp Decl ConstDecl BType ConstDef ConstInitVal LVal ConstExp VarDecl VarDecl_ VarDef InitVal
%type <str_val> UnaryOp

%%

// 开始符, CompUnit ::= FuncDef, 大括号后声明了解析完成后 parser 要做的事情
// 之前我们定义了 FuncDef 会返回一个 str_val, 也就是字符串指针
// 而 parser 一旦解析完 CompUnit, 就说明所有的 token 都被解析了, 即解析结束了
// 此时我们应该把 FuncDef 返回的结果收集起来, 作为 AST 传给调用 parser 的函数
// $1 指代规则里第一个符号的返回值, 也就是 FuncDef 的返回值
CompUnit
  : CompUnits {
    auto comp_unit = make_unique<CompUnitAST>();
    comp_unit->comp_units = unique_ptr<BaseAST>($1);
    ast = move(comp_unit);
  }
  ;

CompUnits 
  : FuncDefOrVarDecl {
    auto ast = new CompUnitsAST();
    ast->funcdef_or_vardecl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | ConstDecl {
    auto ast = new CompUnitsAST();
    ast->const_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | CompUnits FuncDefOrVarDecl {
    auto ast = new CompUnitsAST();
    ast->comp_units = unique_ptr<BaseAST>($1);
    ast->funcdef_or_vardecl = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | CompUnits ConstDecl {
    auto ast = new CompUnitsAST();
    ast->comp_units = unique_ptr<BaseAST>($1);
    ast->const_decl = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | VOID FuncDef_ {
    auto ast = new CompUnitsAST();
    ast->func_def = unique_ptr<BaseAST>($2);
    $$ = ast;
  }  
  | CompUnits VOID FuncDef_ {
    auto ast = new CompUnitsAST();
    ast->comp_units = unique_ptr<BaseAST>($1);
    ast->func_def = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;
  ;

FuncDefOrVarDecl
  : BType FuncDef_ {
    auto ast = new FuncDefOrVarDeclAST();
    ast->func_def = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | BType VarDecl_ {
    auto ast = new FuncDefOrVarDeclAST();
    ast->var_decl = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

FuncDef
  : FuncType IDENT '(' ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  | FuncType IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST();
    ast->func_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->func_f_params = unique_ptr<BaseAST>($4);
    ast->block = unique_ptr<BaseAST>($6);
    $$ = ast;
  }
  ;

FuncDef_
  : IDENT '(' ')' Block {
    auto ast = new FuncDefAST_();
    ast->ident = *unique_ptr<string>($1);
    ast->block = unique_ptr<BaseAST>($4);
    $$ = ast;
  }
  | IDENT '(' FuncFParams ')' Block {
    auto ast = new FuncDefAST_();
    ast->ident = *unique_ptr<string>($1);
    ast->func_f_params = unique_ptr<BaseAST>($3);
    ast->block = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

// 同上, 不再解释
FuncType
  : INT {
    auto ast = new FuncTypeAST();
    ast->type = *new string("int");
    $$ = ast;
  } 
  | VOID {
    auto ast = new FuncTypeAST();
    ast->type = *new string("void");
    $$ = ast;
  }
  ;

FuncFParams
  : FuncFParam {
    auto ast = new FuncFParamsAST();
    ast->func_f_param = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

FuncFParam
  : BType IDENT {
    auto ast = new FuncFParamAST();
    ast->b_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    $$ = ast;
  }
  | BType IDENT ',' FuncFParam {
    auto ast = new FuncFParamAST();
    ast->b_type = unique_ptr<BaseAST>($1);
    ast->ident = *unique_ptr<string>($2);
    ast->func_f_param = unique_ptr<BaseAST>($4);
    $$ = ast;
  }
  ;

FuncRParams
  : Exp {
    auto ast = new FuncRParamsAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

Block
  : '{' BlockItem '}' {
    auto ast = new BlockAST();
    ast->blockitem = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

BlockItem
  : Decl {
    auto ast = new BlockItemAST();
    ast->decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Stmt {
    auto ast = new BlockItemAST();
    ast->stmt = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Decl BlockItem{
    auto ast = new BlockItemAST();
    ast->decl = unique_ptr<BaseAST>($1);
    ast->block_item = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | Stmt BlockItem{
    auto ast = new BlockItemAST();
    ast->stmt = unique_ptr<BaseAST>($1);
    ast->block_item = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

Stmt
  : LVal '=' Exp ';' {
    auto ast = new StmtAST();
    ast->l_val = unique_ptr<BaseAST>($1);
    ast->exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | ';'{
    auto ast = new StmtAST();
    $$ = ast;
  }
  | Exp ';'{
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | Block {
    auto ast = new StmtAST();
    ast->block = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | RETURN ';' {
    auto ast = new StmtAST();
    ast->symbol = *new string("return");
    $$ = ast;
  }
  | RETURN Exp ';' {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($2);
    ast->symbol = *new string("return");
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt_1 = unique_ptr<BaseAST>($5);
    ast->symbol = *new string("if");
    $$ = ast;
  }
  | IF '(' Exp ')' Stmt ELSE Stmt {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt_1 = unique_ptr<BaseAST>($5);
    ast->stmt_2 = unique_ptr<BaseAST>($7);
    ast->symbol = *new string("if");
    $$ = ast;
  }
  | WHILE '(' Exp ')' Stmt {
    auto ast = new StmtAST();
    ast->exp = unique_ptr<BaseAST>($3);
    ast->stmt_1 = unique_ptr<BaseAST>($5);
    ast->symbol = *new string("while");
    $$ = ast;
  }
  | BREAK ';' {
    auto ast = new StmtAST();
    ast->symbol = *new string("break");
    $$ = ast;
  }
  | CONTINUE ';' {
    auto ast = new StmtAST();
    ast->symbol = *new string("continue");
    $$ = ast;
  }
  ;

Decl
  : ConstDecl {
    auto ast = new DeclAST();
    ast->const_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | VarDecl {
    auto ast = new DeclAST();
    ast->var_decl = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

ConstDecl
  : CONST BType ConstDef ';'{
    auto ast = new ConstDeclAST();
    ast->b_type = unique_ptr<BaseAST>($2);
    ast->const_def = unique_ptr<BaseAST>($3);
    $$ = ast;
  }

BType
  : INT {
    auto ast = new BTypeAST();
    ast->type = *unique_ptr<string>(new string("int"));
    $$ = ast;
  }

ConstDef
  : IDENT '=' ConstInitVal {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_init_val = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | IDENT '=' ConstInitVal ',' ConstDef {
    auto ast = new ConstDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->const_init_val = unique_ptr<BaseAST>($3);
    ast->const_def = unique_ptr<BaseAST>($5);
    $$ = ast;
  }
  ;

ConstInitVal
  : ConstExp {
    auto ast = new ConstInitValAST();
    ast->const_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

ConstExp
  : Exp {
    auto ast = new ConstExpAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

LVal
  : IDENT {
    auto ast = new LValAST();
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }
  ;

VarDecl
  : BType VarDef ';' {
    auto ast = new VarDeclAST();
    ast->b_type = unique_ptr<BaseAST>($1);
    ast->var_def = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;
  
VarDecl_
  : VarDef ';' {
    auto ast = new VarDeclAST_();
    ast->var_def = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

VarDef 
  : IDENT {
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    $$ = ast;
  }  
  | IDENT '=' InitVal{
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->init_val = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | IDENT ',' VarDef{
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->var_def = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | IDENT '=' InitVal ',' VarDef{ 
    auto ast = new VarDefAST();
    ast->ident = *unique_ptr<string>($1);
    ast->init_val = unique_ptr<BaseAST>($3);
    ast->var_def = unique_ptr<BaseAST>($5);
    $$ = ast;
  }

InitVal
  : Exp {
    auto ast = new InitValAST();
    ast->exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

Exp 
  : LOrExp {
    auto ast = new ExpAST();
    ast->l_or_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  } 
  | ',' Exp {
    auto ast = new ExpAST();
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  ;

AddExp
  : MulExp {
    auto ast = new AddExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | AddExp '+' MulExp {
    auto ast = new AddExpAST();
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->add_op = *unique_ptr<string>(new string("+"));
    ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | AddExp '-' MulExp {
    auto ast = new AddExpAST();
    ast->add_exp = unique_ptr<BaseAST>($1);
    ast->add_op = *unique_ptr<string>(new string("-"));
    ast->mul_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

MulExp
  : UnaryExp {
    auto ast = new MulExpAST();
    ast->unary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | MulExp '*' UnaryExp {
    auto ast = new MulExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->mul_op = *unique_ptr<string>(new string("*"));
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | MulExp '/' UnaryExp {
    auto ast = new MulExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->mul_op = *unique_ptr<string>(new string("/"));
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | MulExp '%' UnaryExp {
    auto ast = new MulExpAST();
    ast->mul_exp = unique_ptr<BaseAST>($1);
    ast->mul_op = *unique_ptr<string>(new string("%"));
    ast->unary_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

// RelExp ::= AddExp | RelExp ( "<" | ">" | "<=" | ">=" ) AddExp;
RelExp
  : AddExp {
    auto ast = new RelExpAST();
    ast->add_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | RelExp '<' AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->rel_op = *unique_ptr<string>(new string("<"));
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp '>' AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->rel_op = *unique_ptr<string>(new string(">"));
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp LE AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->rel_op = *unique_ptr<string>(new string("<="));
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | RelExp GE AddExp {
    auto ast = new RelExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    ast->rel_op = *unique_ptr<string>(new string(">="));
    ast->add_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

// EqExp ::= RelExp | EqExp ( "==" | "!=" ) RelExp;
EqExp
  : RelExp {
    auto ast = new EqExpAST();
    ast->rel_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | EqExp EQ RelExp {
    auto ast = new EqExpAST();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->eq_op = *unique_ptr<string>($2);
    ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  | EqExp NE RelExp {
    auto ast = new EqExpAST();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    ast->eq_op = *unique_ptr<string>($2);
    ast->rel_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

// LAndExp ::= EqExp | LAndExp "&&" EqExp;
LAndExp
  : EqExp {
    auto ast = new LAndExpAST();
    ast->eq_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LAndExp AND EqExp {
    auto ast = new LAndExpAST();
    ast->l_and_exp = unique_ptr<BaseAST>($1);
    ast->l_and_op = *unique_ptr<string>($2);
    ast->eq_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

// LOrExp ::= LAndExp | LOrExp "||" LAndExp;
LOrExp
  : LAndExp {
    auto ast = new LOrExpAST();
    ast->l_and_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LOrExp OR LAndExp {
    auto ast = new LOrExpAST();
    ast->l_or_exp = unique_ptr<BaseAST>($1);
    ast->l_or_op = *unique_ptr<string>($2);
    ast->l_and_exp = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

PrimaryExp
  : '(' Exp ')' {
    auto ast = new PrimaryExpAST();
    ast->exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | Number {
    auto ast = new PrimaryExpAST();
    ast->number = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | LVal {
    auto ast = new PrimaryExpAST();
    ast->l_val = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  ;

UnaryExp
  : PrimaryExp {
    auto ast = new UnaryExpAST();
    ast->primary_exp = unique_ptr<BaseAST>($1);
    $$ = ast;
  }
  | UnaryOp UnaryExp{
    auto ast = new UnaryExpAST();
    ast->unary_op = *unique_ptr<string>($1);
    ast->unary_exp = unique_ptr<BaseAST>($2);
    $$ = ast;
  }
  | IDENT '(' ')'{
    auto ast = new UnaryExpAST();
    $$ = ast;
  }
  | IDENT '(' FuncRParams ')'{
    auto ast = new UnaryExpAST();
    ast->func_r_params = unique_ptr<BaseAST>($3);
    $$ = ast;
  }
  ;

UnaryOp 
  : '+'{
    $$ = new string("+");
  }
  | '-'{
    $$ = new string("-");
  }
  | '!'{
    $$ = new string("!");
  }
  ;

Number
  : INT_CONST {
    auto ast = new NumberAST();
    ast->number = *new string(to_string($1));
    $$ = ast;
  }
  ;

%%

// 定义错误处理函数, 其中第二个参数是错误信息
// parser 如果发生错误 (例如输入的程序出现了语法错误), 就会调用这个函数
void yyerror(unique_ptr<BaseAST> &ast, const char *s) {
  cerr << "error: " << s << endl;
}

