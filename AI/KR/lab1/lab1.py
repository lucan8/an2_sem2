from typing import List, Dict, Tuple
from copy import deepcopy
from abc import ABC, abstractmethod
class Node(ABC):
    def __init__(self, data, parent = None, children : List = None):
        self.data = data
        self.parent = parent
        if children is None:
            self.children = []

    def __eq__(self, other):
        if not isinstance(other, Node):
            return False
        return self.data == other.data
    
    def __hash__(self):
        return hash(self.data)

    def __str__(self):
        return str(self.data)
    
    # Returns the path from root to this node
    def rootPath(self) -> List:
        curr = self
        res = [self]

        while (curr.parent != None):
            curr = curr.parent
            res.append(curr)
        return res
    
    
    def pathToString(self):
        r_path = self.rootPath()
        res = "("

        for i in range(0, len(r_path) - 1):
            res += str(r_path[i].data) + '->'

        res += str(r_path[-1].data) + ')'
        return res
    
    def addChild(self, node):
        if not isinstance(node, Node):
            raise Exception("[ERROR] addChild: Invalid argument: Expected " + type(self).__name__  + " | Given " + type(node).__name__)
        
        self.children.append(node)
        node.parent = self
    
    def removeChild(self, node):
        if not isinstance(node, Node):
            raise Exception("[ERROR] removeChild: Invalid argument: Expected " + type(self).__name__  + " | Given " + type(node).__name__)
        
        if node.parent is not self:
            raise Exception("[ERROR] removeChild:" + self + " is not the parent of " + node)
        
        node.parent = None
        self.children.remove(node)

    def moveChild(self, node):
        # Do nothing if this node is the parent
        if node.parent == self:
            return
        
        # Detach child from parent
        if node.parent is not None:
            node.parent.removeChild(node)
        
        # Re-attach to this
        self.addChild(node)

    # Checks if the given node is part of the path from root to this object
    def visited(self, node):
        for n in self.rootPath():
            if n == node:
                return True
        return False


class Graph:
    # Start node should be the first both in the node list and the neighbour_list
    def __init__(self, start_node : Node, dest_nodes : List[Node], node_list: List[Node], neighbour_list: Dict[Node, List[Tuple[Node, int]]]):
        self.start_node = start_node
        self.dest_nodes = dest_nodes
        self.node_list = node_list
        self.neighbour_list = neighbour_list
    
    def isDestination(self, node : Node):
        return node in self.dest_nodes

    def getUnvisitedSuccesors(self, node: Node):
        return [n for n in node.children if node.visited(n)]

    def DFS(self, nr_paths: int):
        start_node = deepcopy(self.start_node)

        results = []
        self._DFS(start_node, results, nr_paths)
        
        return [node.rootPath() for node in results]

    def BFS(self, nr_paths: int) -> List[List[Node]]:
        start_node = deepcopy(self.start_node)

        results = []
        self._BFS(start_node, results, nr_paths)

        return [node.rootPath() for node in results]

    # Constructs all paths to the destination nodes, and stores the dest nodes in the result
    # Paths are constructed using DFS
    def _DFS(self, start_node: Node, result: List[Node], nr_paths: int) -> List[Node]:
        # Stop when we reach the desired nr of paths
        if nr_paths == len(result):
            return
        
        # Stopping as we reached a destination node
        if self.isDestination(start_node):
            result.append(deepcopy(start_node))
            return

        for neighbour in self.neighbour_list[start_node]:
            # Only need the node
            neighbour = neighbour[0]
            # Adding only unvisited neighbours
            if not start_node.visited(neighbour):
                start_node.addChild(neighbour)
                self._DFS(neighbour, result, nr_paths)
                start_node.removeChild(neighbour)
    
    # Constructs all paths to the destination nodes, and stores the dest nodes in the result
    # Paths are constructed using BFS
    def _BFS(self, start_node: Node, result: List[Node], nr_paths: int) -> List[Node]:
        queue = [start_node]

        while len(queue):
            curr = queue[0]
            queue.pop(0)
            
            # Stop when we reach the desired nr of paths
            if nr_paths == len(result):
                return

            # Skip the destination node's neighbours, this path is finished
            if self.isDestination(curr):
                result.append(curr)
                continue

            # Visit all unvisited neighbours and add them to the queue
            # Deepcopying as a node might be used in multiple paths
            for neighbour in self.neighbour_list[curr]:
                # Only need the node
                neighbour = neighbour[0]
                if not curr.visited(neighbour):
                    curr.addChild(deepcopy(neighbour))
                    queue.append(curr.children[-1])
        
# Assuming that the data of the node is an int
# And is index for the node_list
# Not reading weight
def readNeighbourList(node_list : List[Node], in_file):
    neighbour_list = {node: [] for node in node_list}
    # print(neighbour_list)

    for line in in_file:
        data = [int(node_data) for node_data in line.split()]
        # First elem is the node, the rest are it's neighbours
        curr_node = node_list[data[0]]

        # Adding the neighbours
        for i in range(1, len(data)):
            neighbour_list[curr_node].append((node_list[data[i]], 0))

    return neighbour_list


def printNeighbourList(neighbour_list :Dict[Node, List[Tuple[Node, int]]]):
    for n in neighbour_list:
        print("List for " + str(n.data))
        for neighbour, weight in neighbour_list[n]:
            print(f"({neighbour.data}, {weight})", end=" ")
        print()


def main():
    in_file = open("data.in")

    # First line: node list
    node_list = [Node(int(node)) for node in in_file.readline().split()]
    print(*node_list)

    # Second line: start node
    start_node = node_list[int(in_file.readline()) - 1]
    print(start_node)

    # Third line: destination nodes
    dest_nodes = [node_list[int(node) - 1] for node in in_file.readline().split()]
    print(*dest_nodes)
    
    # Rest: neighbour lists
    neighbour_list = {node: [] for node in node_list}
    print(neighbour_list)

    for line in in_file:
        data = [int(node_data) for node_data in line.split()]
        # First elem is the node, the rest are it's neighbours
        curr_node = node_list[data[0] - 1]

        # Adding the neighbours
        for i in range(1, len(data)):
            neighbour_list[curr_node].append((node_list[data[i] - 1], 0))

    for n in neighbour_list:
        print("List for " + str(n.data))
        for neighbour in neighbour_list[n]:
            print(str(neighbour.data), end=" ")
        print()
        i += 1

    g_test = Graph(start_node, dest_nodes, node_list, neighbour_list)
    g_test.BFS()