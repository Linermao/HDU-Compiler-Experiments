from scripts.grammar import Grammar
from tabulate import tabulate

class LL1PredictiveParser:
    def __init__(self, grammar: Grammar):
        self.grammar = grammar
        self.parse_table = self._build_parse_table()
        self.input_tokens = []
        self.current_token_index = 0
        self.parse_process = []

    def _build_parse_table(self):
        parse_table = {}
        for non_terminal, productions in self.grammar.rules.items():
            parse_table[non_terminal] = {}
            for production in productions:
                first_set = self.grammar._first(production[0])
                for symbol in first_set:
                    if symbol == 'ε':
                        follow_set = self.grammar.follow[non_terminal]
                        for symbol in follow_set:
                            if symbol not in parse_table[non_terminal]:
                                parse_table[non_terminal][symbol] = production
                            parse_table[non_terminal]['$'] = production
                        continue
                    parse_table[non_terminal][symbol] = production

        return parse_table
    
    def print_parse_table(self):
        """以表格形式打印预测分析表"""
        # 获取所有非终结符和终结符
        non_terminals = list(self.parse_table.keys())
        terminals = set()
        for row in self.parse_table.values():
            terminals.update(row.keys())
        terminals = sorted(terminals)  # 按字典序排序

        # 准备表格数据
        header = [""] + terminals  # 第一行：空格 + 终结符
        table = []
        for non_terminal in non_terminals:
            row = [non_terminal]  # 第一列是非终结符
            for terminal in terminals:
                if terminal in self.parse_table[non_terminal]:
                    production = self.parse_table[non_terminal][terminal]
                    row.append(f"{non_terminal} -> {''.join(production)}")
                else:
                    row.append("")  # 空单元格
            table.append(row)

        # 打印表格
        print(tabulate(table, headers=header, tablefmt="grid"))

    def print_parse_process(self):
        """打印解析过程"""
        if not hasattr(self, "parse_process") or not self.parse_process:
            print("No parse process available.")
            return
        # 表头
        headers = ["Step", "Stack", "Input", "Current Symbol", "Action"]
        # 使用 tabulate 打印表格
        print(tabulate(self.parse_process, headers=headers, tablefmt="grid"))
    
    def parse(self, input_tokens):
        self.input_tokens = input_tokens
        self.current_token_index = 0
        stack = ["$", self.grammar.start_symbol]
        steps = []  # 用于记录解析过程
        
        steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', '', "Start Parsing"))

        while stack:
            next_token = self.input_tokens[self.current_token_index] if self.current_token_index < len(self.input_tokens) else '$'
            current_symbol = stack.pop()

            if current_symbol == 'ε':
                # 忽略空字符
                steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', current_symbol, "Skip ε"))
                continue
            elif current_symbol == '$':
                if self.current_token_index == len(self.input_tokens):
                    steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', current_symbol, "Accept"))
                    self.parse_process = steps
                    return True
                else:
                    steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', current_symbol, "Error: Unexpected end"))
                    self.parse_process = steps
                    return False
            elif current_symbol in self.grammar.rules:  # 非终结符
                if next_token in self.parse_table[current_symbol]:
                    production = self.parse_table[current_symbol][next_token]
                    stack.extend(reversed(production))
                    steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', current_symbol, f"Expand {current_symbol} -> {''.join(production)}"))
                else:
                    steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', current_symbol, f"Error: No rule for {current_symbol} with {next_token}"))
                    self.parse_process = steps
                    return False
            elif current_symbol == self.input_tokens[self.current_token_index]:  # 匹配终结符
                self.current_token_index += 1
                steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', current_symbol, f"Match {current_symbol}"))
            else:
                steps.append((len(steps) + 1, "".join(reversed(stack)), "".join(input_tokens[self.current_token_index:])+'$', current_symbol, f"Error: Expected {current_symbol}, found {next_token}"))
                self.parse_process = steps
                return False

        self.parse_process = steps
        return False
    
    def __str__(self):
        pass
