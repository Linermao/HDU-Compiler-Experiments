from typing import List
from useful import State

class NFA:
    def __init__(self, input_char):
        self.start_state = State()
        self.accept_state = State(is_accept=True)
        self.states : List[State] = []
        self.states_num = 2

        self.start_state.add_transition(input_char, self.accept_state)
        self.states.append(self.start_state)
        self.states.append(self.accept_state)
        self.start_state.name = '0'
        self.accept_state.name = '1'

    def add_state(self, state):
        self.states.append(state)

    def modify_num(self):
        self.states_num = len(self.states)

    def __str__(self):
        lines = []

        for state in self.states:
            lines.append(f"{state}")
            for input_char, target_states in state.transitions.items():
                for target_state in target_states:
                    lines.append(f"{state.name} --{input_char}--> {target_state.name}")

        return '\n'.join(lines)

def nfa_concatenate(nfa1:NFA, nfa2:NFA):

    # 将 nfa1 的接受状态通过 ε 转换连接到 nfa2 的起始状态
    nfa1.accept_state.add_transition('ε', nfa2.start_state)
    nfa1.accept_state.is_accept = False

    # 合并接受状态
    nfa1.accept_state = nfa2.accept_state

    """
    l = nfa1.nodes_num
    for state in nfa2.states:
        state.name = str(int(state.name) + l)
        nfa1.states.append(state)
    """

    # 更新状态名，合并状态集合
    for state in nfa2.states:
        state.name = str(int(state.name) + nfa1.states_num)
        nfa1.states.append(state)

    nfa1.modify_num()

    return nfa1

def nfa_closure(nfa:NFA):
    new_start_state = State()
    new_accept_state = State(is_accept=True)
    
    # 新的起始状态通过 ε 转换连接到 nfa 的起始状态和新的接受状态
    new_start_state.add_transition('ε', nfa.start_state)
    new_start_state.add_transition('ε', new_accept_state)
    
    # nfa 的接受状态通过 ε 转换连接到 nfa 的起始状态和新的接受状态
    nfa.accept_state.add_transition('ε', nfa.start_state)
    nfa.accept_state.add_transition('ε', new_accept_state)
    nfa.accept_state.is_accept = False

    # 更新状态名
    for state in nfa.states:
        state.name = str(int(state.name) + 1)
    
    new_accept_state.name = str(nfa.states_num+ 1)

    nfa.start_state = new_start_state
    nfa.accept_state = new_accept_state

    nfa.states.append(new_start_state)
    nfa.states.append(new_accept_state)

    nfa.modify_num()

    return nfa

def nfa_or(nfa1:NFA, nfa2:NFA):

    new_start_state = State()
    new_accept_state = State(is_accept=True)

    # 新的起始状态通过 ε 转换连接到 nfa1 和 nfa2 的起始状态
    new_start_state.add_transition('ε', nfa1.start_state)
    new_start_state.add_transition('ε', nfa2.start_state)

    # nfa1 和 nfa2 的接受状态通过 ε 转换连接到新的接受状态
    nfa1.accept_state.add_transition('ε', new_accept_state)
    nfa1.accept_state.is_accept = False

    nfa2.accept_state.add_transition('ε', new_accept_state)
    nfa2.accept_state.is_accept = False

    # 更新状态名，合并状态集合
    for state in nfa1.states:
        state.name = str(int(state.name) + 1)

    for state in nfa2.states:
        state.name = str(int(state.name) + nfa1.states_num + 1)
        nfa1.states.append(state)

    new_accept_state.name = str(nfa1.states_num + nfa2.states_num + 1)

    nfa1.start_state = new_start_state
    nfa1.accept_state = new_accept_state

    nfa1.states.append(new_start_state)
    nfa1.states.append(new_accept_state)

    nfa1.modify_num()

    return nfa1
    
def postfix_to_NFA(postfix:str):
    stack = []

    for char in postfix:
        if char.isalpha():  # 如果是字母，创建基本 NFA 并推入栈中
            nfa = NFA(char)
            stack.append(nfa)
        elif char == '.':
            nfa2 = stack.pop()
            nfa1 = stack.pop()
            concatenated_nfa = nfa_concatenate(nfa1, nfa2)
            stack.append(concatenated_nfa)
        elif char == '|':
            nfa2 = stack.pop()
            nfa1 = stack.pop()
            or_nfa_result = nfa_or(nfa1, nfa2)
            stack.append(or_nfa_result)
        elif char == '*':
            nfa = stack.pop()
            closure_nfa_result = nfa_closure(nfa)
            stack.append(closure_nfa_result)

    return stack[0]

if __name__ == "__main__":
    postfix = 'ab|'
    nfa = postfix_to_NFA(postfix)
    print(nfa)