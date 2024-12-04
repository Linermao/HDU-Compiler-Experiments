%option noyywrap
%option nounput
%option noinput
%option yylineno

%{

#include <cstdlib>
#include <string>

// 因为 Flex 会用到 Bison 中关于 token 的定义
// 所以需要 include Bison 生成的头文件
#include "sysy.tab.hpp"
#include "AST.h"

using namespace std;

static int PRINT_TOKEN = 0;

void print_token(const string& token, const string& name){
    if (PRINT_TOKEN){
        cout << token << ": " << name << endl;
    }
}

void print_error(const string& msg){
    cout << "Error: " << msg << "at line " << yylineno << endl;
    exit(0);
}

%}

/* 空白符和注释 */
WhiteSpace    [ \t\n\r]*
LineComment   "//".*
BlockComment  "/*"([^\*]|(\*)*[^\*/])*(\*)*"*/"

/* 标识符 */
Identifier    [a-zA-Z_][a-zA-Z0-9_]*

/* 整数字面量 */
Decimal       [1-9][0-9]*
Octal         0[0-7]*
Hexadecimal   0[xX][0-9a-fA-F]+

%%

{WhiteSpace}    { /* 忽略, 不做任何操作 */ }
{LineComment}   { /* 忽略, 不做任何操作 */ }
{BlockComment}   { /* 忽略, 不做任何操作 */ }

"if"            { print_token("IF", "if"); return IF; }
"then"          { print_token("THEN", "then"); return THEN; }
"else"          { print_token("ELSE", "else"); return ELSE; }
"while"         { print_token("WHILE", "while"); return WHILE; }
"break"         { print_token("BREAK", "break"); return BREAK; }
"continue"      { print_token("CONTINUE", "continue"); return CONTINUE; }
"int"           { print_token("INT", "int"); return INT; }
"return"        { print_token("RETURN", "return"); return RETURN; }
"const"         { print_token("CONST", "const"); return CONST; }
"void"          { print_token("VOID", "void"); return VOID; }
"<="            { print_token("LE", "<="); return LE; }
">="            { print_token("GE", ">="); return GE; }
"=="            { print_token("EQ", "=="); return EQ; }
"!="            { print_token("NE", "!="); return NE; }
"&&"            { print_token("AND", "&&"); return AND; }
"||"            { print_token("OR", "||"); return OR; }
"+"             { print_token("ADD", "+"); return ADD; }
"-"             { print_token("SUB", "-"); return SUB; }
"*"             { print_token("MUL", "*"); return MUL; }
"/"             { print_token("DIV", "/"); return DIV; }
"%"             { print_token("MOD", "%"); return MOD; }
"="             { print_token("ASSIGN", "="); return ASSIGN; }
";"             { print_token("SEMI", ";"); return SEMI; }
","             { print_token("COMMA", ","); return COMMA; }
"("             { print_token("LPAREN", "("); return LPAREN; }
")"             { print_token("RPAREN", ")"); return RPAREN; }
"{"             { print_token("LBRACE", "{"); return LBRACE; }
"}"             { print_token("RBRACE", "}"); return RBRACE; }
"["             { print_token("LBRACKET", "["); return LBRACKET; }
"]"             { print_token("RBRACKET", "]"); return RBRACKET; }



{Identifier}    { yylval.str_val = new string(yytext); print_token("IDENT", yytext); return IDENT; }

{Decimal}       { yylval.int_val = strtol(yytext, nullptr, 0); print_token("INT_CONST", yytext); return INT_CONST; }
{Octal}         { yylval.int_val = strtol(yytext, nullptr, 0); print_token("INT_CONST", yytext); return INT_CONST; }
{Hexadecimal}   { yylval.int_val = strtol(yytext, nullptr, 0); print_token("INT_CONST", yytext); return INT_CONST; }

.               { cout << "Error: Invalid characterat \"" << yytext[0] << "\" at line " << yylineno << endl; exit(0); }

%%

