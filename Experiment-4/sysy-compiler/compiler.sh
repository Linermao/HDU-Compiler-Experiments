# build/compiler -koopa test/hello.c -o test/hello.koopa
build/compiler -lex test/hello.c -o test/hello_lex.txt
build/compiler -ast test/hello.c -o test/hello_ast.txt
build/compiler -semantic test/hello.c -o test/hello_semantic.txt