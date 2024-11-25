from scripts.grammar import Grammar

if __name__ == "__main__":
    # 示例
    '''
    S -> AB
    A -> a | ε
    B -> b
    '''

    test = {
        "S": [["A", "B"]],
        "A": [['a', 'A'], ['ε']],
        'B': [['b']]
    }

    grammar = Grammar(list(test.keys())[0])
    grammar.rules = test

    print("原始文法:")
    print(grammar)

    print('\nFIRST 集合:')
    grammar.compute_first()
    grammar.print_first()

    print('\nFOLLOW 集合:')
    grammar.compute_follow()
    grammar.print_follow()