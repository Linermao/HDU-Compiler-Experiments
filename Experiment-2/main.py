import strips.Regex2Postfix as r2p
import strips.NFA2DFA as n2d
import strips.Postfix2NFA as p2n
from strips.ToGraph import Drawer

"""
正则表达式转DFA处理引擎

作者: 林灿
日期: 2024-11-09
"""


if __name__ == "__main__":
    nfa = p2n.postfix_to_NFA('ab|')
    print(nfa)
    dfa = n2d.nfa_to_dfa(nfa)
    print(dfa)

    drawer = Drawer('test')
    drawer.visualize(nfa)

