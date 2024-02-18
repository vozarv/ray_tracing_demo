import numpy as np
import matplotlib.pyplot as plt




f = open("vectors.txt", "r")

data = f.readlines()

fig = plt.figure()
ax = fig.add_subplot(projection='3d')

for d in data:

    d.strip()
    vals = d.split()

    x = float(vals[0])
    y = float(vals[1])
    z = float(vals[2])
    ax.scatter(x, y, z)


plt.show()