from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator, FormatStrFormatter
import numpy as np

def print_time_eff():
    fig = plt.figure()
    ax = fig.gca(projection='3d')

    # Make data.
    X = np.arange(200, 0, -20)
    Y = np.arange(0, 51, 1)
    Z = np.zeros((51, 10))
    X, Y = np.meshgrid(X, Y)

    for line in open("eff_test/stats.txt", "rt"):
        dims, percent, effect = list(map(float, line.split()))
        Z[int(percent), int(dims / 20 - 1)] = int(effect)

    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                        linewidth=0, antialiased=False)

    # Customize the z axis.
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.5, aspect=5)

    plt.show()

def print_mem_eff():
    fig = plt.figure()
    ax = fig.gca(projection='3d')

    # Make data.
    X = np.arange(50, 1050, 50)
    Y = np.arange(50, 1050, 50)
    Z = np.zeros((20, 20))
    X, Y = np.meshgrid(X, Y)

    for line in open("eff_test/stats2.txt", "rt"):
        rows, cols, eff = list(map(float, line.split()))
        Z[int(rows) // 50 - 1, int(cols) // 50 - 1] = eff * 100

    # Plot the surface.
    surf = ax.plot_surface(X, Y, Z, cmap=cm.coolwarm,
                        linewidth=0, antialiased=False)

    # Customize the z axis.
    ax.zaxis.set_major_locator(LinearLocator(10))
    ax.zaxis.set_major_formatter(FormatStrFormatter('%.02f'))

    # Add a color bar which maps values to colors.
    fig.colorbar(surf, shrink=0.5, aspect=5)

    plt.show()

print_mem_eff()
