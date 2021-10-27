import matplotlib.pyplot as plt

f = list(open("tcp-example.tr", "rt").readlines()) #Reads and stores the lines from the trace file as separate entries to a list

queue = {} #Will be used for enqueue dequeue operation
times = [] #Will store times at which dequeue operation is observed
delays = [] #Would store delays observed at corresponding times

for i in f:
    if i[0] == '+':
        x = i.split(' ')
        queue[x[18]] = float(x[1]) #Enqueue Operation
    elif i[0] == '-':
        x = i.split(' ')
        y = float(queue[x[18]])
        del queue[x[18]] #Dequeue Operation
        w = float(x[1]) - y #Time delay is stored
        times.append(float(x[1]))
        delays.append(w)

plt.figure()
plt.plot(times,delays,'bx')
plt.xlabel('Time elapsed')
plt.ylabel('Queueing Delay')
plt.show()

