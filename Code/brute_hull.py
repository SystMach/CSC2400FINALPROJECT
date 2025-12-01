import matplotlib.pyplot as plt
import sys
import time  # <-- add this

# ------------------------------
# Load points from text file
# ------------------------------
def load_points(filename):
    points = []
    with open(filename, "r") as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            x_str, y_str = line.split(",")
            x = float(x_str.strip())
            y = float(y_str.strip())
            points.append((x, y))
    return points

# ------------------------------
# Convex Hull – Brute Force
# ------------------------------
def cross_product(p1, p2, p3):
    x1, y1 = p1
    x2, y2 = p2
    x3, y3 = p3
    return (x2 - x1)*(y3 - y1) - (y2 - y1)*(x3 - x1)

def brute_force_convex_hull(points):
    n = len(points)
    edges = []
    for i in range(n):
        for j in range(i + 1, n):
            p1 = points[i]
            p2 = points[j]
            left = right = False
            for k in range(n):
                if k == i or k == j:
                    continue
                cp = cross_product(p1, p2, points[k])
                if cp > 0:
                    left = True
                elif cp < 0:
                    right = True
                if left and right:
                    break
            if not (left and right):
                edges.append((p1, p2))
    return edges

# ------------------------------
# MAIN
# ------------------------------
if len(sys.argv) != 2:
    print("Usage: python3 brute_hull.py <filename>")
    sys.exit(1)

filename = sys.argv[1]
points = load_points(filename)

# ---- TIMING START ----
start_time = time.time()

edges = brute_force_convex_hull(points)

end_time = time.time()
elapsed = end_time - start_time
print(f"Brute-force hull completed in {elapsed:.6f} seconds")
# ---- TIMING END ----

# Visualization
plt.figure(figsize=(6,6))
x_vals, y_vals = zip(*points)
plt.scatter(x_vals, y_vals)

# Draw hull edges
for p1, p2 in edges:
    plt.plot([p1[0], p2[0]], [p1[1], p2[1]])

plt.title(f"Brute-Force Convex Hull: {filename}")
plt.xlabel("X")
plt.ylabel("Y")
plt.grid(True)

# Save figure
output_file = f"{filename}_hull.png"
plt.savefig(output_file)
print(f"Saved hull visualization as {output_file}")