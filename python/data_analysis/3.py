N = 8

def ZhenToFan(z):
    z = str(z)
    result = []
    
    # 处理符号位
    if "-" in z:
        result.append('1')
        z = z[1:]
        is_negative = True
    elif '+' in z:
        result.append('0')
        z = z[1:]
        is_negative = False
    else:
        result.append('0')
        is_negative = False
    
    num = int(z)
    
    # 处理0的特殊情况
    if num == 0:
        if result[0] == '0':  # +0
            return '0' + '0' * 7
        else:  # -0
            return '1' + '1' * 7
    
    # 将数值部分转换为7位二进制
    binary = []
    temp = num
    for i in range(6, -1, -1):
        if temp >= (1 << i):
            binary.append('1')
            temp -= (1 << i)
        else:
            binary.append('0')
    
    # 负数：按位取反
    if is_negative:
        for i in range(7):
            if binary[i] == '1':
                binary[i] = '0'
            else:
                binary[i] = '1'
    
    result.extend(binary)
    return ''.join(result)

z = input()       #真实值
f = ZhenToFan(z)  #转换成8位反码
print('%s -> %s' % (z, f))