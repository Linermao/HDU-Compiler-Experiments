## This is HDU compiler principle experiment homework

- [This is HDU compiler principle experiment homework](#this-is-hdu-compiler-principle-experiment-homework)
  - [前言](#前言)
  - [1. 仓颉语言环境配置与使用](#1-仓颉语言环境配置与使用)
  - [2. 词法分析器 - Python](#2-词法分析器---python)
    - [2.1 正则表达式转后缀表达式](#21-正则表达式转后缀表达式)
    - [2.2 后缀表达式转 NFA](#22-后缀表达式转-nfa)
    - [2.3 NFA 转为 DFA](#23-nfa-转为-dfa)
    - [2.4 DFA 转最小 DFA](#24-dfa-转最小-dfa)
  - [3. 语法分析器 - Python](#3-语法分析器---python)
    - [3.1 消除左递归](#31-消除左递归)
    - [3.2 提取左公因子](#32-提取左公因子)
    - [3.3 计算 FIRST 集与 FOLLOW 集](#33-计算-first-集与-follow-集)
    - [3.4 构造预测分析表并判断LL1文法](#34-构造预测分析表并判断ll1文法)
    - [3.5 输出解析过程](#35-输出解析过程)
  - [4. SysY语言编译器 - Flex + Bison + c++ （12.12 - IR分析未实现，if-else悬挂未解决，AST树生成还缺少分层）](#4-sysy语言编译器---flex--bison--c-1212---ir分析未实现if-else悬挂未解决ast树生成还缺少分层)
    - [4.1 文件目录结构](#41-文件目录结构)
    - [4.2 词法分析 Flex实现](#42-词法分析-flex实现)
    - [4.3 语法分析 Bison实现](#43-语法分析-bison实现)
    - [4.4 语义分析](#44-语义分析)

### 前言

此为杭州电子科技大学编译原理课程实践作业，分为四个实验：

- 仓颉语言环境配置与使用
- 词法分析器
- 语法分析器
- SysY语言编译器

参考文件：

| 文件名 | 链接 |
| :----: | :--: |
| 北大编译原理实践在线文档 | [https://pku-minic.github.io/online-doc/#/preface/](https://pku-minic.github.io/online-doc/#/preface/) |
| Gitee  Herbert/SysYCompilerPKU2022Spring | [https://gitee.com/zhuoyuan_he/sysy-compiler-pku2022-spring](https://gitee.com/zhuoyuan_he/sysy-compiler-pku2022-spring) |

### 1. 仓颉语言环境配置与使用

略，见华为仓颉语言官网配置即可

### 2. 词法分析器 - Python

简单的词法分析器，对正则表达式进行 NFA，DFA 的匹配输出。

由于相对路径问题，可以直接使用`strips`中的`new.py`进行实验。

#### 2.1 正则表达式转后缀表达式

#### 2.2 后缀表达式转 NFA

#### 2.3 NFA 转为 DFA

#### 2.4 DFA 转最小 DFA


### 3. 语法分析器 - Python

简单的语法分析器，对与输入语法进行 LL1 文法判断，并且输出解析过程。

#### 3.1 消除左递归


#### 3.2 提取左公因子

使用前缀树算法进行最大左公因子提取。

#### 3.3 计算 FIRST 集与 FOLLOW 集

#### 3.4 构造预测分析表并判断LL1文法

#### 3.5 输出解析过程

### 4. SysY语言编译器 - Flex + Bison + c++ （12.12 - IR分析未实现，if-else悬挂未解决，AST树生成还缺少分层）

首先需要配置 `docker` 环境，直接参考北大编译原理文档即可。

```bash
# 建议在Linux下使用， Mac等arm环境无法执行
docker pull maxxing/compiler-dev
```

使用封装好的快捷启动脚本:

```bash
# 封装 docker 启动脚本，自动挂载文件目录与启动 bash
./Experiment-4/start_compiler.sh

# start_compiler 文件内容，name也许不同，请自行修改
docker compose up -d
container_id=$(docker ps -qf "name=compiler")
echo container_id: $container_id
docker exec -it "$container_id" bash
# docker-compose.yml 文件内容，volumes为需要挂载的文件目录，请修改为自己的路径
# 执行此文件会自动打开 docker 下的 bash 终端，若失败，参考北大编译原理实践文档。
services:
  compiler:
    image: maxxing/compiler-dev
    volumes:
      - /home/alvin/Investigation/HDU-Compilation-principle-experiments:/root/compiler
    stdin_open: true
    tty: true
    entrypoint: ["bash"]

```

编译执行：

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

目前只实现六种语义检查：

- 变量声明重复
- 变量未声明
- 函数声明重复
- 函数未声明
- 函数变量混用


