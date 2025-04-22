import networkx as nx
import matplotlib.pyplot as plt
from networkx.algorithms.community import greedy_modularity_communities


def load_dynamic_graph(graph_path):
    edges = []
    with open(graph_path, "r") as file:
        # Überspringe die erste Zeile (Header)
        header = file.readline().strip()
        print(f"Header: {header}")  # Optional: Header ausgeben, um sicherzustellen, dass er korrekt gelesen wird

        # Verarbeite die restlichen Zeilen
        for line in file:
            parts = line.strip().split()
            if len(parts) == 3 and parts[0] == "1":  # Prüfe, ob es sich um eine gültige Kante handelt
                node1, node2 = int(parts[1]), int(parts[2])
                edges.append((node1, node2))

    # Erstelle den Graphen
    G = nx.Graph()
    G.add_edges_from(edges)
    return G

def analyze_graph_topology(graph_path):
    
    # Lade den Graphen
    
    G = load_dynamic_graph(graph_path)
    
    # Berechne grundlegende Metriken
    num_nodes = G.number_of_nodes()
    num_edges = G.number_of_edges()
    density = nx.density(G)
    avg_clustering = nx.average_clustering(G)
    diameter = nx.diameter(G) if nx.is_connected(G) else "Unconnected Graph"
    degree_distribution = [d for _, d in G.degree()]

    # Ausgabe der Ergebnisse
    print(f"Graph: {graph_path}")
    print(f"Anzahl der Knoten: {num_nodes}")
    print(f"Anzahl der Kanten: {num_edges}")
    print(f"Dichte: {density}")
    print(f"Durchschnittlicher Clustering-Koeffizient: {avg_clustering}")
    print(f"Durchmesser: {diameter}")
    
    print("Community detecion: ")
    communities = list(greedy_modularity_communities(G))
    num_communities = len(communities)
    print(f"Anzahl der Communities: {num_communities}")
    #for i, community in enumerate(communities):
    #    print(f"Community {i+1}: {len(community)} ")


    return G


# Beispiel für dynamischen Graph
dynamic_graph_path = "./res/dynGraphs/haggle.seq"
analyze_graph_topology(dynamic_graph_path)

print( "\n")
# Beispiel für statischen Graph
#static_graph_path = "./res/dynGraphs/static1_formatted.seq"
#analyze_graph_topology(static_graph_path)