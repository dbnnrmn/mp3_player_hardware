import math

def rd(x,y=0):
    m = int('1'+'0'*y) # multiplier - how many positions to the right
    q = x*m # shift to the right by multiplier
    c = int(q) # new number
    i = int( (q-c)*10 ) # indicator number on the right
    if i >= 5:
        c += 1
    return c/m

def find_res(resistance,res_range):

    e12=[1.0, 1.2, 1.5, 1.8, 2.2, 2.7, 3.3, 3.9, 4.7, 5.6, 6.8, 8.2]

    e24=[1.0, 1.1, 1.2, 1.3, 1.5, 1.6, 1.8, 2.0,
    2.2, 2.4, 2.7, 3.0, 3.3, 3.6, 3.9, 4.3, 4.7, 5.1, 5.6,
    6.2, 6.8, 7.5, 8.2, 9.1] 

    current_res_range=[]

    if res_range =="e12" :
        current_res_range.extend(e12)
    if res_range =="e24" :
        current_res_range.extend(e24)


    factor=0
    while resistance>=10:
        resistance=resistance/10
        factor+=1
    value=0
    delta_old=100

    for i in range(len(current_res_range)):
        delta_new=abs(current_res_range[i]-resistance)
        if delta_new<delta_old:
            delta_old=delta_new
            value=current_res_range[i]

    range_resistance=value*pow(10,factor)
    print(range_resistance)
    return range_resistance


print("hello")


###################################

Vfb=0.5
R1=100000
Vout=5

R2=(Vfb*R1)/(Vout-Vfb)
R2=rd(R2)
R2=find_res(R2,"e24")

Voutreal=Vfb*((R1+R2)/R2)
print('R1='+str(R1))
print('R2='+str(R2))
print('Voutreal='+str(Voutreal))


###################################

Ilcdbackl=0.06
Vlcdbackl=3

Rlcdbackl=(3.3-Vlcdbackl)/Ilcdbackl

Rlcdbackl=rd(Rlcdbackl)
Rlcdbackl=find_res(Rlcdbackl,"e24")
Plcdbackl=(3.3-Vlcdbackl)*Ilcdbackl
print(Plcdbackl)
print(Rlcdbackl)