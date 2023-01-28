import matplotlib.pyplot as plt
import numpy as np
import csv

# For plotting functions as comparisons

def square(n,normalization_coeff,space_size):
    return normalization_coeff*(n/space_size)**2 

# Plot time in function of size in 3 cases : 
# Using number of edges as size,
# Using number of edges + number of verticies as size,
# Using number of verticies as size
sizeval = []
timeval = []

# First case

with open("./benchmarks/benchmarks_edges.csv") as csvfile :
    csvreader = csv.DictReader(csvfile)
    for row in csvreader :
        sizeval.append(int(row['size']))
        timeval.append(float(row['time']))

x_space = np.linspace(0,max(sizeval))

# Linear regression
A = np.vstack([sizeval, np.ones(len(sizeval))]).T

m, c = np.linalg.lstsq(A, timeval, rcond=None)[0]

plt.scatter(sizeval,timeval,label='Benchmark points')
plt.plot(x_space,m*x_space+c,label='Comparison with f(x)=x, normalized')
plt.xlabel("Number of edges")
plt.ylabel("Time, in seconds")
plt.title("Time of execution as a function of the number of edges")
plt.legend()
plt.savefig("./benchmarks_plots/edges_graph.png")
plt.close()

# Second case

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
plt.plot(x_space,m*x_space+c,label='Comparison with f(x)=x, normalized')
plt.xlabel("Number of edges + number of verticies")
plt.ylabel("Time, in seconds")
plt.title("Time of execution as a function of the number of verticies\n plus the number of edges")
plt.legend()
plt.savefig("./benchmarks_plots/edges+verticies_graph.png")
plt.close()

# Third case 

sizeval = []
timeval = []

with open("./benchmarks/benchmarks_verticies.csv") as csvfile :
    csvreader = csv.DictReader(csvfile)
    for row in csvreader :
        sizeval.append(int(row['size']))
        timeval.append(float(row['time']))

x_space = np.linspace(0,max(sizeval))
square_vals = square(x_space,max(timeval),max(sizeval))

plt.scatter(sizeval,timeval,label='Benchmark points')
plt.plot(x_space,square_vals,label='Comparison with f(x)=x^2, normalized')
plt.xlabel("Number of verticies")
plt.ylabel("Time, in seconds")
plt.title("Time of execution as a function of the number of verticies")
plt.legend()
plt.savefig("./benchmarks_plots/verticies_graph.png")
plt.close()