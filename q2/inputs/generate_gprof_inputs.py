import random

def generate_input_file(filename, implementation, n, m):
    with open(filename, 'w') as f:
        f.write(f"{n} {m}\n")
        f.write(f"{implementation}\n")
        arcs = set()
        while len(arcs) < m:
            u = random.randint(1, n)
            v = random.randint(1, n)
            if u != v:
                arcs.add((u, v))
        for u, v in arcs:
            f.write(f"{u} {v}\n")

# Large inputs
generate_input_file("input_large_deque.txt", "deque", 100000, 300000)
generate_input_file("input_large_adjmatrix.txt", "adjmatrix", 100000, 300000)
generate_input_file("input_large_list.txt", "list", 100000, 300000)
