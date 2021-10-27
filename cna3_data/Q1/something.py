positive = {}
negative = {}
diff = []

fp=open("tcp-example.tr","r")

i = 0
for i in fp:

    i += 1
    if i >100:
        break
    
	token=i.split()
	sign=token[0]
	time=float(token[1])
	idx=token[18]

    print(idx)

    if sign=='+' :
		positive[idx]=time
	elif sign=='-':
		negative.append(time)
		diff.append(time-positive[idx])
		del positive[idx]

	
    