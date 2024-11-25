class State:
    def __init__(self, is_accept=False):
        self.name = '0'
        self.is_accept = is_accept
        self.transitions = {}

    def add_transition(self, input_char, target_state):
        if input_char not in self.transitions:
            self.transitions[input_char] = []
        self.transitions[input_char].append(target_state)

    def __str__(self):
        return f"State name is: {self.name}, Accepet: {True if self.is_accept else False}"
    