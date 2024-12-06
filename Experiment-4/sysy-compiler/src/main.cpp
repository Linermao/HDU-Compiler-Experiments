#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include <cstring>
#include <unistd.h>
#include "assert.h"  
#include "AST.h"

using namespace std;

// 声明 lexer 的输入, 以及 parser 函数
// 为什么不引用 sysy.tab.hpp 呢? 因为首先里面没有 yyin 的定义
// 其次, 因为这个文件不是我们自己写的, 而是被 Bison 生成出来的
// 你的代码编辑器/IDE 很可能找不到这个文件, 然后会给你报错 (虽然编译不会出错)
// 看起来会很烦人, 于是干脆采用这种看起来 dirty 但实际很有效的手段
extern FILE *yyin;
extern int yyparse(unique_ptr<BaseAST> &ast);
extern int PRINT_TOKEN;
extern void print_token(const string& token, const string& name);

// Lex 词法分析输出导致代码非常 dirty，有时间再优化吧
const char * mode;
const char * input;
const char * output;

void print_token(const string& token, const string& name){
  if (PRINT_TOKEN){
    int old = dup(1);
    freopen(output, "a", stdout);
    // std::cout << token << ": " << name << endl;
    std::cout << token << ": " << name << endl;
    dup2(old, 1);
  }
}

int main(int argc, const char *argv[]) {
  // 解析命令行参数. 测试脚本/评测平台要求你的编译器能接收如下参数:
  // compiler 模式 输入文件 -o 输出文件
  // compiler mode input_file -o output_file

  if (argc == 2 && strcmp(argv[1], "-help") == 0){
    printf("Usage: ./compiler -koopa | -lex | -ast | -semantic input_file -o output_file\n");
    exit(0);
  }
  else if (argc != 5){
    printf("ERROR! Usage: ./compiler -koopa | -lex | -ast | -semantic input_file -o output_file\n");
    exit(0);
  }

  mode = argv[1];
  input = argv[2];
  output = argv[4];

  // 非常 dirty
  if (strcmp(mode, "-lex") == 0)
  {
    // 清空
    int old = dup(1);
    freopen(output, "w", stdout);
    dup2(old, 1);
    PRINT_TOKEN=1;
  }  
  
  // 打开输入文件, 并且指定 lexer 在解析的时候读取这个文件
  yyin = fopen(input, "r");
  assert(yyin);

  // 调用 parser 函数, parser 函数会进一步调用 lexer 解析输入文件的
  // parse input file
  unique_ptr<BaseAST> ast;
  auto ret = yyparse(ast);
  assert(!ret);

  int old = dup(1);

  if (strcmp(mode, "-koopa") == 0)
  {
    // Dump Koopa IR
    freopen(output, "w", stdout);
    ast->Dump();
    dup2(old, 1);
  }
  else if (strcmp(mode, "-ast") == 0)
  {
    freopen(output, "w", stdout);
    ast->Print_AST();
    dup2(old, 1);
  }  
  else if (strcmp(mode, "-semantic") == 0)
  {
    ast->Semantic_Analysis();
  }  
  else if (strcmp(mode, "-lex") == 0)
  {
    PRINT_TOKEN=1;
  }  
  else
  {
    printf("ERROR! Usage: ./compiler -koopa | -lex | -ast | -semantic input_file -o output_file\n");
  }
  return 0;
}