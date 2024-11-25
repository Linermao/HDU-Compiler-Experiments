import graphviz
from Postfix2NFA import NFA
from NFA2DFA import DFA

class Drawer:
    def __init__(self, filename, format="png"):
        self.graph = graphviz.Digraph(filename=filename, format=format)
    
    def visualize_nfa(self, nfa:NFA, filename=''):
        dot = graphviz.Digraph(comment=filename, format='png')

        for state in nfa.states:
            if state.is_accept:
                dot.node(state.name, label=state.name, shape='doublecircle')
            else:
                dot.node(state.name, label=state.name, shape='circle')

        for state in nfa.states:
            for symbols in state.transitions.keys():
                for symbol in symbols:
                    for next_state in state.transitions[symbols]:  
                        dot.edge(state.name, next_state.name, label=symbol)

        dot.render(filename, format='png', cleanup=True)
        dot.view()

    def visualize_dfa(self, dfa:DFA, filename=''):
        dot = graphviz.Digraph(comment=filename, format='png')

        for state in dfa.states:
            if state.is_accept:
                dot.node(state.name, label=state.name, shape='doublecircle')
            else:
                dot.node(state.name, label=state.name, shape='circle')

        for state in dfa.states:
            for symbols in state.transitions.keys():
                for symbol in symbols:
                    for next_state in state.transitions[symbols]:  
                        dot.edge(state.name, next_state.name, label=symbol)

        dot.render(filename, format='png', cleanup=True)
        dot.view()
