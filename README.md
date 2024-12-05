## This is HDU complier principle experiment homework

此为杭州电子科技大学编译原理课程实践作业，分为四个实验：

- 仓颉语言环境配置与使用
- 词法分析器
- 语法分析器
- SysY语言编译器

### 0. 前言

参考文件：
- 北大编译原理实践在线文档：[https://pku-minic.github.io/online-doc/#/preface/](https://pku-minic.github.io/online-doc/#/preface/)
- Gitee  Herbert/SysYCompilerPKU2022Spring: [https://gitee.com/zhuoyuan_he/sysy-compiler-pku2022-spring](https://gitee.com/zhuoyuan_he/sysy-compiler-pku2022-spring)

### 1. 仓颉语言环境配置与使用

略，见华为仓颉语言官网配置即可

### 2. 词法分析器 - Python

### 3. 语法分析器 - Python

### 4. SysY语言编译器 - Flex + Bison + c++

（注意，本项目并未完善成功，可能存在未知bug，并且代码结构还未优化 - 24.12.5）

项目启动：

```bash
# 封装 docker 启动脚本，自动挂载文件目录与启动 bash
.Experiment-4/start_compiler.sh
```

```bash
# 位于 docker 环境下的 bash
cd Experiment-4/sysy-compiler
# 清除 make 缓存 
make clean
# 生成
make
# 快速测试
./compiler.sh
# 编译器测试
build/compiler -lex file -o file
build/compiler -ast file -o file
build/compiler -semantic file -o file
```

#### 4.1 文件目录结构
```
sysy-compiler/
│
├── src/
│   ├── AST.h - AST 树定义
│   ├── main.cpp - 主程序
│   ├── sysy.l - flex 文件
│   └── sysy.y - bison 文件
│
├── test/
│   ├── Lexical_Analysis/ - 词法分析测试
│   ├── Semantic_Analysis/ - 语义分析测试
│   ├── Syntax_Analysis/ - 语法分析测试
│   └── hello.* ... - 快速测试文件
│
├── bison.sh - DEBUG 快速生成 bison 输出文件
│
├── complier.sh - DEBUG 快速生成 bison 输出文件
│
└── other files ...
```

#### 4.2 词法分析 Flex实现



#### 4.3 语法分析 Bison实现



#### 4.4 语义分析


