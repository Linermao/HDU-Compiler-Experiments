from typing import List, Dict, Set
from scripts.parser import LL1PredictiveParser
from scripts.grammar import Grammar
from scripts.ll1 import is_ll1

if __name__ == "__main__":
    # 示例
    '''
    E→TE'
    E'→+TE'|ε
    T→FT'
    T'→*FT'|ε
    F→id| (E)
    '''

    # test = {
    #     "S": [["a", "p", "p", "l", "e"], ["a", "p", "p", "l", "y"], ["a", "p", "p", "l", "c", "a", "t", "i", "o", "n"], ["b", "a", "l", "l"], ["b", "a", "t"], ["b", "a", "t", "h"], ["X", "b"]],
    #     "X": [["a", "b"], ["a", "c"], ["a", "d"]],
    # }

    test = {
        "S": [["A", "B"]],
        "A": [['a', 'A'], ['ε']],
        'B': [['b']]
    }

    grammar = Grammar(list(test.keys())[0])
    # grammar.add_rule("S", [["a", "b"], ["a", 'c']])
    grammar.rules = test

    print("原始文法:")
    print(grammar)

    print("\n消除左递归后的文法:")
    grammar.eliminate_left_recursion()
    print(grammar)

    print("\n提取左公因子后的文法:")
    grammar.extract_left_common_factor()
    print(grammar)

    print('\nFIRST 集合:')
    grammar.compute_first()
    grammar.print_first()

    print('\nFOLLOW 集合:')
    grammar.compute_follow()
    grammar.print_follow()

    if is_ll1(grammar):
        print('\n该文法是 LL(1) 文法')

        parser = LL1PredictiveParser(grammar)

        print('\n预测分析表:')
        parser.print_parse_table()

        input_tokens = input("\n请输入要分析的语法\n").split(' ')
        # input_tokens = ["id", '+', 'id', '*', 'id'] 

        if parser.parse(input_tokens):
            print(f"输入字符串 '{input_tokens}' 符合文法规则。")
        else:
            print(f"输入字符串 '{input_tokens}' 不符合文法规则。")


        print('\n解析过程:')
        parser.print_parse_process()

    else:
        print('\n该文法不是 LL(1) 文法')

