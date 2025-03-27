import sys
import os
# Add the parent directory to sys.path in order to import from lab1
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from lab1.lab1 import Node, Graph, printNeighbourList
from typing import List, Dict, Tuple, cast
from queue import PriorityQueue
from copy import deepcopy

class AStarNode(Node):
    def __init__(self, data, parent = None, children : List = None, g : int = 0, h : int = 0):
        super().__init__(data, parent, children)
        self.g = g
        self.h = h
    
    def f(self):
        return self.h + self.g
    
    def __lt__(self, other):
        if not isinstance(other, AStarNode):
            raise Exception("[ERROR] __lt__: Invalid argument: Expected " + type(self).__name__  + " | Given " + type(other).__name__)
        
        return self.f() < other.f()
      
    
class AStarGraph(Graph):
    def __init__(self, start_node : AStarNode, dest_nodes : List[AStarNode], node_list: List[AStarNode],
                 neighbour_list: Dict[AStarNode, List[Tuple[AStarNode, int]]], h_list: Dict[AStarNode, int]):
        super().__init__(start_node, dest_nodes, node_list, neighbour_list)
        self.h_list = h_list
    
    def estimate_h(self, node: AStarNode) -> int:
        return self.h_list[node]

    # Determines all neighbours that can be relaxed
    # Attaches them to the node given as argument and returns those relaxed neighbours
    def successors(self, node: AStarNode) -> List[AStarNode]:
        results = []

        for succ, e_weight in self.neighbour_list[node]:
            # Cast to AStarNode
            succ = cast(AStarNode, succ)

            # Update succesor's g if needed and attach it to this node
            if succ.g == 0 or node.g + e_weight < succ.g:
                succ.g = node.g + e_weight
                node.moveChild(succ)
                results.append(succ)

        return results

# List of the graph's destination nodes
def a_star(graph: AStarGraph) -> List[AStarNode]:
    # Keep a dictionary for open nodes
    # Needed when updating
    open_dict = set()

    # Nodes to be proccessed
    open = PriorityQueue()
    open.put(graph.start_node)

    # Proccessed nodes
    closed = set()

    result = []
    while len(result) != len(graph.dest_nodes):
        curr = open.get()
        # This node was already proccessed(lazily removed)
        if curr in open_dict:
            continue
        
        # Mark node as visited
        open_dict.add(curr)
        print(curr, curr.f())

        # Stop when a destination node is found
        if curr in graph.dest_nodes:
            result.append(curr)

        # Relax succesors and add them to the open list(if not already there)
        for neigh in graph.successors(curr):
            # Needed maybe for negative edges?
            # As for only positive edges succesors can't return a node from closed
            if neigh not in closed:
                open.put(neigh)
        
        closed.add(curr)
    return result


def ida_TSP(graph: AStarGraph):
    clone_graph = deepcopy(graph)

    while len(clone_graph.node_list):
        curr_node = clone_graph.start_node
        dists = []

        # Get distances to all nodes
        for node in clone_graph.node_list:
            clone_graph.dest_nodes = [node]
            dists.append((node, ida_star(clone_graph)))
        
        # Choose closest node
        chosen_node = max(dists)[0]
        
        # Remove it from the list
        clone_graph.node_list.remove(curr_node)

        curr_node = chosen_node


# Returns the distance to the first destination node found
def ida_star(graph: AStarGraph) -> List[AStarGraph]:
    bound = int('inf')

    while True:
        start = deepcopy(graph.start_node)
        res = ida_search(start, 0, bound)

        if res == "found":
            return bound
        if res == int('inf'):
            return "not found"
        
        bound = res        

def ida_search(graph: AStarGraph, curr_node: AStarNode, bound: int):
    curr_f = curr_node.f()
    # Update bound
    if curr_f > bound:
        return curr_f
    
    # Found node
    if graph.isDestination(curr_node):
        return "found"
    
    v_min = int('inf')
    # DFS
    for succ, e_weight in graph.neighbour_list[curr_node]:
        succ = cast(AStarNode, succ)
        succ.g = curr_node.g + e_weight

        curr_node.addChild(succ)

        res = ida_search(graph, succ, bound)
        if res == 'found':
            return 'found'
        v_min = min(res, v_min)

        curr_node.removeChild(succ)

    return v_min
        
    

# Assuming the edges are directed and weighted
# The value of the node is also index into the node_list
def EdgeLToNeighL(node_list : List[AStarNode], edge_list : List[Tuple[int, int, int]]) -> Dict[AStarNode, List[Tuple[AStarNode, int]]]:
    result = {node : [] for node in node_list}
    for n in result:
        print(hash(n))

    for node1, node2, weight in edge_list:
        result[node_list[node1]].append((node_list[node2], weight))
    
    return result

def main():
    in_file = open("data.in")

    # First line: node list
    node_list = [AStarNode(int(node)) for node in in_file.readline().split()]
    print(*node_list)

    # Second line: start node
    start_node = node_list[int(in_file.readline())]
    print(start_node)

    # Third line: destination nodes
    dest_nodes = [node_list[int(node)] for node in in_file.readline().split()]
    print(*dest_nodes)

    # Forth line: Edge count
    edge_count = int(in_file.readline())
    print(edge_count)

    # Edges form: node1, node2, weight
    edge_list = []
    while edge_count:
        data = [int(d) for d in in_file.readline().split()]
        edge_list.append((data[0], data[1], data[2]))

        edge_count -= 1
    
    # Convert to neighbour list
    neigh_list = EdgeLToNeighL(node_list, edge_list)
    printNeighbourList(neigh_list)

    # AstarNode -> h value
    h_dict = {node : 0 for node in node_list}
    for line in in_file:
        data = [int(d) for d in line.split()]
        # Set h for node
        node_list[data[0]].h = data[1]

        # Set h for dict
        h_dict[node_list[data[0]]] = data[1]
    
    # for n in h_dict:
    #     print(n.data, h_dict[n])

    g_test = AStarGraph(start_node, dest_nodes, node_list, neigh_list, h_dict)

    result = a_star(g_test)
    for n in result:
        print(n.pathToString(), ": ", n.g)

main()

# x = AStarNode(1)
# print(hash(x), hash(1))




