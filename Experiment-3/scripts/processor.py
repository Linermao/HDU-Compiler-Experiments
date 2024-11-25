from typing import List, Dict, Set

def eliminate_left_recursion(grammar):
    """先消除间接左递归，再消除直接左递归"""
    non_terminals = list(grammar.rules.keys())
    
    # 依次消除非终结符之间的间接左递归
    for i in range(len(non_terminals)):
        for j in range(i):
            _replace_productions(grammar, non_terminals[i], non_terminals[j])
    
    # 消除直接左递归
    for non_terminal in non_terminals:
        _eliminate_immediate_left_recursion(grammar, non_terminal)

def extract_left_common_factor(grammar):
    """提取文法中的左公因子"""
    updated_rules = {}
    for non_terminal, productions in grammar.rules.items():
        # 创建一个前缀树（Trie）来帮助识别公因子
        prefix_tree = {}
        for production in productions:
            node = prefix_tree
            for symbol in production:
                if symbol not in node:
                    node[symbol] = {}
                node = node[symbol]
            node['#'] = production  # 终止标志，用于区分叶子节点
        # 遍历前缀树提取左公因子
        new_productions = []
        sub_rules = {}
        for symbol, subtree in prefix_tree.items():
            common_prefix, branches = _extract_common_prefix({symbol: subtree}, [])
            if len(branches) > 1:  # 有多个分支，需提取公因子
                new_non_terminal = non_terminal + "'"
                refined_branches = [
                    branch[len(common_prefix):] if branch[len(common_prefix):] else ["ε"] for branch in branches
                ]
                new_productions.append(common_prefix + [new_non_terminal])
                sub_rules[new_non_terminal] = refined_branches
            else:
                new_productions.append(branches[0])  # 单个分支，无需提取公因子
        updated_rules[non_terminal] = new_productions
        updated_rules.update(sub_rules)  # 添加新的非终结符规则
    grammar.rules = updated_rules

def _extract_common_prefix(subtree, prefix):
    """提取公共前缀"""
    keys = list(subtree.keys())
    if len(keys) == 1 and '#' not in keys:  # 只有一个符号（非叶子节点）
        symbol = keys[0]
        return _extract_common_prefix(subtree[symbol], prefix + [symbol])
    else:
        branches = []
        for symbol, child in subtree.items():
            if symbol == '#':  # 到达叶子节点
                branches.append(prefix)
            else:
                _, child_branches = _extract_common_prefix(child, prefix + [symbol])
                branches.extend(child_branches)
        return prefix, branches

def compute_first(grammar):
    """计算所有非终结符的 FIRST 集合"""
    _init_first_follow_rule(grammar)
    for non_terminal in grammar.rules:
        _first(grammar, non_terminal)

def compute_follow(grammar):
    """计算所有非终结符的 FOLLOW 集合"""
    for non_terminal in grammar.rules:
        _follow(grammar, non_terminal)

def _replace_productions(grammar, non_terminal: str, target_non_terminal: str):
    """用target_non_terminal的产生式替换non_terminal中以target_non_terminal为前缀的产生式"""
    new_productions = []
    for production in grammar.rules[non_terminal]:
        if production[0] == target_non_terminal:
            for target_production in grammar.rules[target_non_terminal]:
                new_productions.append(target_production + production[1:])
        else:
            new_productions.append(production)
    grammar.rules[non_terminal] = new_productions

def _eliminate_immediate_left_recursion(grammar, non_terminal: str):
    """消除直接左递归"""
    productions = grammar.rules[non_terminal]
    left_recursions = []
    non_left_recursions = []
    # 分离左递归和非左递归的产生式
    for production in productions:
        if production[0] == non_terminal:
            left_recursions.append(production[1:])  # 移除左递归前缀
        else:
            non_left_recursions.append(production)
    # 如果没有直接左递归，直接返回
    if not left_recursions:
        return
    # 创建新的非终结符
    new_non_terminal = non_terminal + "'"
    grammar.rules[non_terminal] = [p + [new_non_terminal] for p in non_left_recursions]
    grammar.rules[new_non_terminal] = [p + [new_non_terminal] for p in left_recursions] + [['ε']]

def _init_first_follow_rule(grammar):
    """初始化first和follow集合"""
    non_terminals = list(grammar.rules.keys())
    for non_terminal in non_terminals:
        if non_terminal not in grammar.first:
            grammar.first[non_terminal] = set()
            grammar.follow[non_terminal] = set()

def _first(grammar, symbol: str) -> Set[str]:
    """递归计算单个符号的 FIRST 集合"""
    if symbol not in grammar.rules:
        return {symbol}  # 终结符的 FIRST 集合为其自身
    if symbol in grammar.first and grammar.first[symbol]:
        return grammar.first[symbol]
    for production in grammar.rules[symbol]:
        for token in production:
            token_first = _first(grammar, token)
            grammar.first[symbol].update(token_first - {'ε'})
            if 'ε' not in token_first:
                break
        else:
            grammar.first[symbol].add('ε')
    return grammar.first[symbol]

def _follow(grammar, non_terminal: str) -> Set[str]:
    """递归计算单个非终结符的 FOLLOW 集合"""
    if non_terminal not in grammar.follow or not grammar.follow[non_terminal]:
        if non_terminal == grammar.start_symbol:
            grammar.follow[non_terminal].add('$')
        # 如果FOLLOW集合为空，开始计算
        for non_terminal_prod in grammar.rules:
            for production in grammar.rules[non_terminal_prod]:
                for symbol in production:
                    if symbol == non_terminal:
                        # 若为最后一个元素
                        if production.index(symbol) == len(production) - 1:
                            grammar.follow[non_terminal].update(grammar.follow[non_terminal_prod])
                        # 查询后一元素的First集合，加入到目标非终结符中，筛除ε
                        for next_symbol in production[production.index(symbol)+1:]:
                            next_symbol_first = _first(grammar, next_symbol)
                            grammar.follow[non_terminal].update(next_symbol_first - {'ε'})
                            if 'ε' in next_symbol_first:
                                if production.index(next_symbol) == len(production) - 1:
                                    grammar.follow[non_terminal].update(grammar.follow[non_terminal_prod])
                                continue
                            else:
                                break
    return grammar.follow[non_terminal]

def _compute_non_terminals(grammar):
    """计算非终结符集合"""
    grammar.non_terminals = set(grammar.rules.keys())

def _compute_terminals(grammar):
    """计算终结符集合"""
    grammar.terminals = set()
    for productions in grammar.rules.values():
        for production in productions:
            for symbol in production:
                if symbol not in grammar.non_terminals:
                    grammar.terminals.add(symbol)

def print_first(grammar):
    for non_terminal, f_set in grammar.first.items():
        print(f"{non_terminal}: {f_set}")

def print_follow(grammar):
    for non_terminal, f_set in grammar.follow.items():
        print(f"{non_terminal}: {f_set}")