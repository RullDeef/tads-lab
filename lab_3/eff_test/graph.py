from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np


fig = plt.figure()
ax = fig.gca(projection='3d')

# Make data.
X = np.arange(200, 0, -20)
Y = np.arange(5, 105, 5)
Z = np.zeros((20, 10))
X, Y = np.meshgrid(X, Y)

for line in open("eff_test/stats.txt", "rt"):
    dims, percent, effect = list(map(float, line.split()))
    Z[int(percent / 5 - 1), int(dims / 20 - 1)] = int(effect)

# Plot the surface.
surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                       linewidth=0, antialiased=False)

# Customize the z axis.
ax.zaxis.set_major_locator(LinearLocator(10))
ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

# Add a color bar which maps values to colors.
fig.colorbar(surf, shrink=0.5, aspect=5)

plt.show()
