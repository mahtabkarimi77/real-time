import matplotlib.pyplot as plt 
file="powerInS.txt";
powers=[];
with open(file,'r')as file:
    for line in file:
        powers.append(line.strip())

filetime="data.txt";
times=[];
with open(filetime,'r')as filetime:
    for line in filetime:
        l=line.split(',')
        times.append(l[1].strip())
filepr="pr.txt";
pr=[];
with open(filepr,'r')as filepr:
    for line in filepr:
        pr.append(line.strip())
plt.figure(1)
plt.plot(times,powers)
plt.title('Remaninig charge in the battery ')
plt.figure(2)
plt.plot(times,pr)
plt.title('Battery charging ')
plt.show()
