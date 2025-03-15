import sys
import os

# Add the parent directory to sys.path in order to import from lab1
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from typing import List, Dict
from copy import deepcopy
from lab1.lab1 import Node, Graph
from abc import abstractmethod

# TODO: Write a hash function for state
class State:
    initial_nr_each = 3
    boat_capacity = 2

    def __init__(self, nr_cannibals: int, nr_missionaries: int, boat_poz: bool):
        self.nr_cannibals = nr_cannibals
        self.nr_missionaries = nr_missionaries
        self.boat_poz = boat_poz

    @staticmethod
    def init_static_data(file_name):
        file = open(file_name, "r")
        
        data = file.readline().split()
        State.initial_nr_each, State.boat_capacity = int(data[0]), int(data[1])

        file.close()
    
    def __eq__(self, other):
        return self.nr_cannibals == other.nr_cannibals and self.nr_missionaries == other.nr_missionaries and self.boat_poz == other.boat_poz
    
    # Simple hashing using a prima number for each field
    def __hash__(self):
        return hash((self.nr_cannibals, self.nr_missionaries, self.boat_poz))

    def __str__(self):
        if not self.boat_poz:
            return "Current State:\n" + str(self.nr_missionaries) + " missionaries, " + str(self.nr_cannibals) + " cannibals " + " | " + str(State.initial_nr_each - self.nr_missionaries) + " missionaries, " + str(State.initial_nr_each - self.nr_cannibals) + " cannibals\n Boat is on the left\n"
        
        return "Current State:\n" + str(self.nr_missionaries) + " missionaries, " + str(self.nr_cannibals) + " cannibals " + " | " + str(State.initial_nr_each - self.nr_missionaries) + " missionaries, " + str(State.initial_nr_each - self.nr_cannibals) + " cannibals\n Boat is on the right\n"

    def __repr__(self):
        return "(" + str(self.nr_missionaries) + ", " + str(self.nr_cannibals) + ", " + str(self.boat_poz) + ")"
    
    def isValidState(self):
        nr_can_curr = self.nr_cannibals
        nr_mis_curr = self.nr_missionaries
       
        nr_can_compl = State.initial_nr_each - self.nr_cannibals
        nr_mis_compl = State.initial_nr_each - self.nr_missionaries
        
        valid_curr = (nr_can_curr <= nr_mis_curr or nr_mis_curr == 0) and nr_can_curr >= 0 and nr_can_curr <= State.initial_nr_each and nr_mis_curr >= 0 and self.nr_missionaries <= State.initial_nr_each
        valid_other = (nr_can_compl <= nr_mis_compl or nr_mis_compl == 0) and nr_can_compl >= 0 and nr_can_compl <= State.initial_nr_each and nr_mis_compl >= 0 and self.nr_missionaries <= State.initial_nr_each
        
        return valid_curr and valid_other

class StateNode(Node):
    # Returns a list of nodes that can be this node's succesors
    def succesors(self):
        result = []
        for nr_cannibals_boat in range(0, State.boat_capacity + 1):
            for nr_missionaries_boat in range(0, State.boat_capacity - nr_cannibals_boat + 1):
                # Boat is left, send people right
                if not self.data.boat_poz:
                    new_state = State(self.data.nr_cannibals - nr_cannibals_boat, self.data.nr_missionaries - nr_missionaries_boat, not self.data.boat_poz)
                else: # Boat is right, send people left
                    new_state = State(self.data.nr_cannibals + nr_cannibals_boat, self.data.nr_missionaries + nr_missionaries_boat, not self.data.boat_poz)

                valid_boat = (nr_cannibals_boat <= nr_missionaries_boat or nr_missionaries_boat == 0) and nr_cannibals_boat + nr_missionaries_boat > 0

                # Avoiding invalid states
                if new_state.isValidState() and valid_boat:
                    result.append(StateNode(new_state))

        return result

        
class StateGraph(Graph):
    def scop(self, node: Node):
        return State.initial_nr_each == node.data.nr_cannibals and State.initial_nr_each == node.data.nr_missionaries


def generateStateGraph(in_file_name: str):
    # Set the initial set up of the game
    State.init_static_data(in_file_name)

    # Set the start node
    start_node_state = State(State.initial_nr_each, State.initial_nr_each, False)
    start_node = StateNode(start_node_state)

    # Set the destination node
    dest_node_state = State(0, 0, True)
    dest_node = StateNode(dest_node_state)

    # Declare the the neighbour list(Node: List[Node])
    neighbour_list = {}
    queue = [start_node]

    visited = set()
    visited.add(start_node)

    # Fill neighbour list
    while len(queue):
        curr = queue[0]
        # Declare the neighbour list of the current node
        neighbour_list[curr] = []

        queue.pop(0)
        
        # Go through all possible succesors
        for succ in curr.succesors():
            # Every succesor is a neighbour
            neighbour_list[curr].append(succ)

            # If not visited, we found a new state that we add to the queue
            if succ not in visited:
                queue.append(succ)
                visited.add(succ)
    
    return StateGraph(start_node, [dest_node], list(visited), neighbour_list)


def printRootPath(path: List[Node]):
    for node in path:
        print(node, sep='\n', end="\n\n")

                
def main():
    graph = generateStateGraph("data.in")
    print(len(graph.node_list))
    result = graph.BFS(1)[0]

    printRootPath(result)

main()




