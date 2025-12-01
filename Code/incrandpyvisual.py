import matplotlib.pyplot as plt
from scipy.spatial import ConvexHull
import numpy as np

# Read points from coordinates.txt
points = []
with open("coordinates.txt") as f:
    n = int(f.readline().strip())  # first line is number of points
    for _ in range(n):
        line = f.readline().strip()
        if not line:
            continue
        x_str, y_str = line.split(",")
        points.append((float(x_str), float(y_str)))

points = np.array(points)

# Compute convex hull
hull = ConvexHull(points)

# Plot all points
plt.scatter(points[:,0], points[:,1], color='blue', s=30, label='Input points')

# Plot hull polygon
for simplex in hull.simplices:
    plt.plot(points[simplex,0], points[simplex,1], 'r-')

# Highlight hull vertices
plt.scatter(points[hull.vertices,0], points[hull.vertices,1], color='red', s=50, label='Hull vertices')

plt.title("Convex Hull from coordinates.txt")
plt.legend()
plt.gca().set_aspect('equal', adjustable='box')
plt.show()

plt.savefig("hull_plot.png", dpi=300)
