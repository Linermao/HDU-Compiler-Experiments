from scripts.grammar import Grammar

if __name__ == "__main__":
    # 示例
    '''
    S -> S+T | T
    T -> T*F | F
    F -> (E) | id
    '''

    test = {
        "S": [["S", "+", 'T'], ["T"]],
        "T": [["T", "*", "F"], ["F"]],
        "F": [["(", "E", ")"], ["id"]]
    }

    grammar = Grammar(list(test.keys())[0])
    # grammar.add_rule("S", [["a", "b"], ["a", 'c']])
    grammar.rules = test

    print("原始文法:")
    print(grammar)

    print("\n消除左递归后的文法:")
    grammar.eliminate_left_recursion()
    print(grammar)

