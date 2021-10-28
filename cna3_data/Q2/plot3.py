import matplotlib.pyplot as plt

f = list(open("tcp-example.tr", "rt").readlines()) #Reads and stores the lines from the trace file as separate entries to a list

queue0 = {} #Will be used for enqueue dequeue operation
times0 = [] #Will store times at which dequeue operation is observed
delays0 = [] #Would store delays observed at corresponding times

queue1 = {}
times1 = []
delays1 = []

for i in f:
    if i[0] == '+':
        x = i.split(' ')
        if x[2][10] == '0':
            queue0[x[18]] = float(x[1]) #Enqueue Operation
        elif x[2][10] == '1':
            queue1[x[18]] = float(x[1])

    elif i[0] == '-':
        x = i.split(' ')
        if x[2][10] == '0':
            y = float(queue0[x[18]])
            del queue0[x[18]] #Dequeue Operation
            w = float(x[1]) - y #Time delay is stored
            times0.append(float(x[1]))
            delays0.append(w)
        elif x[2][10] == '1':
            y = float(queue1[x[18]])
            del queue1[x[18]] #Dequeue Operation
            w = float(x[1]) - y #Time delay is stored
            times1.append(float(x[1]))
            delays1.append(w)

plt.figure()
plt.plot(times1,delays1,'go',label="Node 1")
plt.plot(times0,delays0,'bx',label="Node 0")
plt.xlabel('Time elapsed')
plt.ylabel('Queueing Delay')
plt.legend()
plt.show()

