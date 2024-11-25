def regex_insert_concatenation(regex):

    new_regex = []
    prev = ''
    for curr in regex:
        if prev and ((prev.isalpha() and curr.isalpha()) or
                    (prev.isalpha() and curr == '(') or
                    (prev == ')' and curr.isalpha()) or
                    (prev == ')' and curr == '(') or
                    (prev in '*' and curr.isalpha()) or
                    (prev in '*' and curr == '(')):
            new_regex.append('.')
        new_regex.append(curr)
        prev = curr

    return ''.join(new_regex)

def regex_to_postfix(regex):
        precedence = {'*': 3, '.': 2, '|': 1}
        stack = []
        postfix = []

        for char in regex:
            if char.isalpha():  # 如果是字母，直接加入后缀表达式
                postfix.append(char)
            elif char == '(':
                stack.append(char)
            elif char == ')':
                while stack and stack[-1] != '(':
                    postfix.append(stack.pop())
                stack.pop()  # 弹出 '('
            else:
                # 处理操作符 *, +, ., |
                while (stack and stack[-1] != '(' and
                    precedence[stack[-1]] >= precedence[char]):
                    postfix.append(stack.pop())
                stack.append(char)

        # 将栈中剩余的操作符加入后缀表达式
        while stack:
            postfix.append(stack.pop())
        
        return ''.join(postfix)
