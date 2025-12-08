N = 8             #位数为8
########## Begin ##########
def  ZhenToFan(z):
    z=str(z)
    a=[]
    p=True
    if "-" in z:
        a.append('1')
        z=z[1:]
        p=False
    elif '+' in z:
        a.append('0')
        z=z[1:]
    else:
        a.append('0')
    n=int(z)
    if a[0]=="0" and n==0:
        for i in range(1,N):
            a.append('0')
        return ''.join(a)
    elif a[0]=='1' and n==0:
        for i in range(1,N):
            a.append('1')
        return ''.join(a)
    b=[]
    for i in range(len(z)):
        b.append(z[i])
    while len(b)<7:
        b.insert(0,"0")
    c=[]
    if p==False:
        for i in range(len(b)):
            if b[i]=='1':
                c.append('0')
            elif b[i]=='0':
                c.append('1')
            else:
                continue
    else:
        c.extend(b)
    a.extend(c)
    return ''.join(a)
########## End ##########
z = input()       #真实值
f = ZhenToFan(z)  #转换成8位反码
print('%s -> %s' % (z, f))