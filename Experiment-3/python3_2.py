from scripts.grammar import Grammar

if __name__ == "__main__":
    # 示例
    '''
    S -> apple | apply | applcation | ball | bat | bath | Xb
    X -> ab | ac | ad
    '''

    test = {
        "S": [["a", "p", "p", "l", "e"], ["a", "p", "p", "l", "y"], ["a", "p", "p", "l", "c", "a", "t", "i", "o", "n"], ["b", "a", "l", "l"], ["b", "a", "t"], ["b", "a", "t", "h"], ["X", "b"]],
        "X": [["a", "b"], ["a", "c"], ["a", "d"]],
    }

    grammar = Grammar(list(test.keys())[0])
    grammar.rules = test

    print("原始文法:")
    print(grammar)

    print("\n提取左公因子后的文法:")
    grammar.extract_left_common_factor()
    print(grammar)