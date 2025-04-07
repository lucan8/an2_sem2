import numpy as np
import matplotlib.pyplot as plt

def plotPol(precision, func_domain, pol_coef):
    x = np.linspace(func_domain[0], func_domain[1], 10**precision)

    # Evaluate the polynomial
    y = np.polyval(pol_coef, x)

    label = ""
    for i in range(len(pol_coef)):
        if pol_coef[i] == 0:
            continue
        if pol_coef[i] < 0:
            label += " - " + str(abs(pol_coef[i])) + " * x^" + str(len(pol_coef) - i - 1)
        else:
            label += " + " + str(pol_coef[i]) + " * x^" + str(len(pol_coef) - i - 1)

    # Plot the function
    plt.plot(x, y, label=label, color="b")
    plt.axhline(0, color="black", linewidth=0.5)  # X-axis
    plt.axvline(0, color="black", linewidth=0.5)  # Y-axis
    plt.grid(True, linestyle="--", linewidth=0.5)
    plt.legend()
    plt.xlabel("x")
    plt.ylabel("f(x)")
    plt.title("Polynomial Function Plot")

in_file = open("py_data.out", 'r')

population_size = int(in_file.readline())
precision = int(in_file.readline())
func_domain = [float(coef) for coef in in_file.readline().split()]
pol_coef = [float(coef) for coef in in_file.readline().split()]

i = 0
chromos = []

# Read chromosomes and plot each population
for line in in_file:
    if i < population_size:
        data = tuple([float(x) for x in line.split()])
        chromos.append(data)
        i += 1
    else:
        # Plot polynomial and population
        plotPol(precision, func_domain, pol_coef)
        x_values, y_values = zip(*chromos)
        plt.scatter(x_values, y_values, color="red", label="chromosomes")
        plt.show()

        # Reset population
        i = 0
        chromos = []



