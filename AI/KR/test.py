from typing import List, Dict
from copy import deepcopy

class Node:
    def __init__(self, data, parent = None, children: List = None):
        self.data = data
        self.parent = parent
        if children is None:
            self.children = []

    def __eq__(self, other):
        self.data = other.data

    def __str__(self):
        return str(self.data) + self.pathToString()
    
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
            res += r_path[i].data + '->'

        res += r_path[-1].data + ')'
        return res
        
    # Checks if the given node is part of the path from root to this object
    def visited(self, node):
        return node in self.rootPath()
def main():
    root = Node(0)

    v1 = [Node(100)]
    root.children.append(Node(1, root))
    root.children.append(Node(2, root))

    root.children[0].children.append(Node(3, root.children[0]))
    root.children[0].children.append(Node(4, root.children[0]))

    root.children[1].children.append(Node(5, root.children[1]))
    root.children[1].children.append(Node(6, root.children[1]))

    cp = deepcopy(root.children[1].children[1])

    root.children[1].children[1].data = 1000
    root.children[1].children.append(Node(7, root.children[1]))

    k = {i: [] for i in range(10)}
    k[1].append(12)
    print(id(v1[0]))
    #v = {v1[0]: "sus"}
    #print(v[v1[0]])

main()