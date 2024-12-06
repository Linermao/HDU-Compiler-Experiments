# build/compiler -koopa test/hello.c -o test/hello.koopa

for file in *.c; do
    echo "Processing $file"
    ../../build/compiler -ast $file -o $(basename $file .c)_ast.txt
done