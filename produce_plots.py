import matplotlib.pyplot as plt
import numpy as np
import csv

# Plot time as a function of size : 
sizeval = []
timeval = []

with open("./benchmarks/benchmarks_edges+verticies.csv") as csvfile :
    csvreader = csv.DictReader(csvfile)
    for row in csvreader :
        sizeval.append(int(row['size']))
        timeval.append(float(row['time']))

x_space = np.linspace(0,max(sizeval))

# Linear regression
A = np.vstack([sizeval, np.ones(len(sizeval))]).T

m, c = np.linalg.lstsq(A, timeval, rcond=None)[0]

plt.scatter(sizeval,timeval,label='Benchmark points')
plt.plot(x_space,m*x_space+c,label='Comparison with f(x)=x, linear regression')
plt.xlabel("Number of edges + number of verticies")
plt.ylabel("Time, in seconds")
plt.title("Time of execution as a function of the number of verticies\n plus the number of edges, including the graph from twitch")
plt.legend()
plt.savefig("./benchmarks_plots/edges+verticies_plot_all.png")
plt.close()

# Now plot removing the last benchmark point

sizeval = []
timeval = []

with open("./benchmarks/benchmarks_edges+verticies.csv") as csvfile :
    csvreader = csv.DictReader(csvfile)
    for row in csvreader :
        sizeval.append(int(row['size']))
        timeval.append(float(row['time']))

# Remove benchmark of twitch
tab = list(zip(sizeval,timeval))
tab.sort(key = lambda x : x[0])
sizeval = [x for x,y in tab]
timeval = [y for x,y in tab]
sizeval = sizeval[:-1]
timeval = timeval[:-1]

x_space = np.linspace(0,max(sizeval))

# Linear regression
A = np.vstack([sizeval, np.ones(len(sizeval))]).T

m, c = np.linalg.lstsq(A, timeval, rcond=None)[0]

plt.scatter(sizeval,timeval,label='Benchmark points')
plt.plot(x_space,m*x_space+c,label='Comparison with f(x)=x, linear regression')
plt.xlabel("Number of edges + number of verticies")
plt.ylabel("Time, in seconds")
plt.title("Time of execution as a function of the number of verticies\n plus the number of edges, without the graph from twitch")
plt.legend()
plt.savefig("./benchmarks_plots/edges+verticies_plot_first_points.png")
plt.close()


# Now the same but plotting with edges as the size

sizeval = []
timeval = []

with open("./benchmarks/benchmarks_edges.csv") as csvfile :
    csvreader = csv.DictReader(csvfile)
    for row in csvreader :
        sizeval.append(int(row['size']))
        timeval.append(float(row['time']))

# Remove benchmark of twitch
tab = list(zip(sizeval,timeval))
tab.sort(key = lambda x : x[0])
sizeval = [x for x,y in tab]
timeval = [y for x,y in tab]
sizeval = sizeval[:-1]
timeval = timeval[:-1]

x_space = np.linspace(0,max(sizeval))

# Linear regression
A = np.vstack([sizeval, np.ones(len(sizeval))]).T

m, c = np.linalg.lstsq(A, timeval, rcond=None)[0]

plt.scatter(sizeval,timeval,label='Benchmark points')
plt.plot(x_space,m*x_space+c,label='Comparison with f(x)=x, linear regression')
plt.xlabel("Number of edges")
plt.ylabel("Time, in seconds")
plt.title("Time of execution as a function of the number of edges\n without the graph from twitch")
plt.legend()
plt.savefig("./benchmarks_plots/edges_plot.png")
plt.close()