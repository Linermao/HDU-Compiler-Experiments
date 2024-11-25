from scripts.grammar import Grammar

def is_ll1(grammar:Grammar):
    """检查文法是否为 LL(1) 文法"""
    # 检查每个非终结符的产生式
    for non_terminal, productions in grammar.rules.items():
        for i, production1 in enumerate(productions):
            for production2 in productions[i + 1:]:
                # 检查 FIRST(production1) 和 FIRST(production2) 是否不相交
                if not _disjoint(grammar._first(production1[0]), grammar._first(production2[0])):
                    return False
                # 如果 production1 能推导出 ε，则检查 FOLLOW(non_terminal) 是否与 FIRST(production2) 相交
                if 'ε' in grammar._first(production1[0]):
                    if not _disjoint(grammar.follow[non_terminal], grammar._first(production2[0])):
                        return False
    return True

def _disjoint(set1, set2):
    """检查两个集合是否不相交"""
    return not set1.intersection(set2)
