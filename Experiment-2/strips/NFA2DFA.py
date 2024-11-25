from typing import List
import Postfix2NFA as p2n
from useful import State
from Postfix2NFA import NFA

class DFAState():
    def __init__(self, is_accept=False):
        self.name = '0'
        self.states : List[State] = []
        self.is_accept = is_accept
        self.transitions = {}

    def add_transition(self, input_char, target_state):
        if input_char not in self.transitions:
            self.transitions[input_char] = []
        self.transitions[input_char].append(target_state)

    def __str__(self):
        statesName = ', '.join(state.name for state in self.states)
        return f"DFAState name is: {self.name}, Include NFAStates names are {statesName}, Accepet: {True if self.is_accept else False}"
    
class DFA():
    def __init__(self):
        self.start_state = DFAState()
        self.accept_states: List[DFAState] = []
        self.states: List[DFAState] = []
    
    def __str__(self):
        lines = []

        for state in self.states:
            lines.append(f"{state}")
            for input_char, target_states in state.transitions.items():
                for target_state in target_states:
                    lines.append(f"{state.name} --{input_char}--> {target_state.name}")

        return '\n'.join(lines)

def move(dfaState: DFAState, input_char: str):
    """Compute move on input_char from a given DFA state."""
    dfa = DFAState()

    for state in dfaState.states:
        if input_char in state.transitions:
            for target_state in state.transitions[input_char]:
                if target_state not in dfa.states:
                    dfa.states.append(target_state)

    return dfa

def epsilon_closure(dfaState: DFAState):
    """Calculate ε-closure for a DFA state."""
    dfa = DFAState()
    stack = list(dfaState.states)

    while stack:
        state = stack.pop()
        if state not in dfa.states:
            dfa.states.append(state)
            # Add all ε-transitions recursively
            for target_state in state.transitions.get('ε', []):
                if target_state not in stack:
                    stack.append(target_state)

    return dfa

def nfa_to_dfa(nfa: NFA):
    # 创建 DFA
    start_state = DFAState()
    start_state.states.append(nfa.start_state)

    dfa = DFA()
    dfa.start_state = epsilon_closure(start_state)
    dfa.states.append(dfa.start_state)

    # 状态队列用于遍历 DFA 状态
    unmarked_states = [dfa.start_state]

    while unmarked_states:
        dfa_state = unmarked_states.pop()

        # 如果 dfa_state 是 NFA 接受状态的 ε-闭包，标记为 DFA 接受状态
        if any(state.is_accept for state in dfa_state.states):
            dfa_state.is_accept = True
            if dfa_state not in dfa.accept_states:
                dfa.accept_states.append(dfa_state)

        # 对每个字符进行状态转移
        for char in "abcdefghijklmnopqrstuvwxyz":
            next_dfa_state = epsilon_closure(move(dfa_state, char))

            if not next_dfa_state.states:
                continue

            # 检查是否是新状态
            for state in dfa.states:
                if set(next_dfa_state.states) == set(state.states):
                    dfa_state.add_transition(char, state)
                    break
            else:
                # 是新状态，添加到 DFA 中
                next_dfa_state.name = str(len(dfa.states))
                dfa.states.append(next_dfa_state)
                unmarked_states.append(next_dfa_state)
                dfa_state.add_transition(char, next_dfa_state)

    return dfa

if __name__ == "__main__":
    nfa = p2n.postfix_to_NFA('ab|')
    print(nfa)
    print("\nDFA:")
    dfa = nfa_to_dfa(nfa)
    print(dfa)