N = 8             #位数为8
########## Begin ##########
def ZhenToFan(z):
    if "-" in z :
        m=z.replace ("-","")
        while len(m)<7:
            m="0"+m
        n=int(m)
        n=1111111-n 
        n=str(n)
        
        n="1"+n
    else :
        m=z.replace ("+","")
        while len(m)<8:
            m="0"+m
        n=m
    return n 

########## End ##########
z = input()       #真实值
f = ZhenToFan(z)  #转换成8位反码
print('%s -> %s' % (z, f))