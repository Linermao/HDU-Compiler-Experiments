import Regex2Postfix as r2p
import Postfix2NFA as p2n
import NFA2DFA as n2d
from ToGraph import Drawer 

if __name__ == "__main__":
    regex = input("请输入你要分析的正则表达式:\n")
    # regex = '(a|b)*'
    postfix = r2p.regex_to_postfix(r2p.regex_insert_concatenation(regex))
    
    print("\nNFA:")
    nfa = p2n.postfix_to_NFA(postfix)
    print(nfa)

    print("\nDFA:")
    dfa = n2d.nfa_to_dfa(nfa)
    print(dfa)

    drawer = Drawer('test')
    drawer.visualize_nfa(nfa, 'NFA Visualization')
    drawer.visualize_dfa(dfa, 'DFA Visualization')





