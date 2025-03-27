import sys
import os
# Add the parent directory to sys.path in order to import from lab1
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from lab1.lab1 import Node, Graph, printNeighbourList
from typing import List, Dict, Tuple, cast
from copy import deepcopy
import numpy as np

def all_equal(l: List):
    return len(set(l)) == 1

# Converts NP matrix to a list of lists
def fromNPToNormal(np_matrix) -> List[List]:
    return [list(l) for l in np_matrix]

def flatten(matrix: List[List]):
    return [
            elem
            for l in matrix
            for elem in l
          ]

class TicTacToeTable:
    MIN_SYMBOL = '0'
    MAX_SYMBOL = 'X'
    EMPTY_SYMBOL = '*'

    def __init__(self, matrix : List[List[str]] = None, turn : str = MAX_SYMBOL):
        if matrix == None:
            matrix = [[TicTacToeTable.EMPTY_SYMBOL for _ in range(0, 3)] for _ in range(0, 3)]
        
        self.matrix = matrix
        self.turn = turn
              
    def getOtherTurn(self):
        if self.turn == TicTacToeTable.MAX_SYMBOL:
            return TicTacToeTable.MIN_SYMBOL
        return TicTacToeTable.MAX_SYMBOL
        
    def ended(self):
        # No more space left
        if TicTacToeTable.EMPTY_SYMBOL not in flatten(self.matrix):
            return True
        
        # Check line win
        if self._ended(self.matrix):
           return True
        
        # Check column win
        if self._ended(fromNPToNormal(np.transpose(np.array(self.matrix)))):
           return True

        return self._ended(self._getDiag())
            
    
    def getHeuristic(self):
        transpose = fromNPToNormal(np.transpose(self.matrix))
        diag = self._getDiag()
        return sum([self._getHeuristic(l) for l in self.matrix + transpose + diag])
    

    def __eq__(self, other):
        if not isinstance(other, TicTacToeTable):
            return False
        return self.matrix == other.matrix
    

    def __hash__(self):
        res = []
        for l in self.matrix:
            res += l

        return hash(tuple(res))

    def __str__(self):
        return "\n".join([" ".join(l) for l in self.matrix])

    def _getHeuristic(self, l: List):
        max_count = l.count(TicTacToeTable.MAX_SYMBOL)
        min_count = l.count(TicTacToeTable.MIN_SYMBOL)

        if max_count:
            if min_count:
                return 0
            return 10 ** max_count
        elif min_count:
            return -10 ** min_count
        return 0


    def _ended(self, matrix : List[List[str]]):
        for l in matrix:
            if l.count(self.turn) == len(l):
                return True
                
        return False
    

    def _getDiag(self):
        diag = [[], []]

        mat_len = len(self.matrix)
        for i in range(0, mat_len):
            diag[0].append(self.matrix[i][i])
            diag[1].append(self.matrix[i][mat_len - i - 1])

        return diag
        
        
class TicTacToeNode(Node):
    def __init__(self, data : TicTacToeTable, parent = None, children : List = None):
        super().__init__(data, parent, children)


    def successors(self) -> List:
        result = []
        for i in range(0, len(self.data.matrix)):
            for j in range(0, len(self.data.matrix[i])):
                # Empty space, simulate player marking it and change turns
                if self.data.matrix[i][j] == TicTacToeTable.EMPTY_SYMBOL:
                    new_table = deepcopy(self.data.matrix)
                    new_table[i][j] = self.data.turn

                    new_state = TicTacToeTable(new_table, self.data.getOtherTurn())
                    result.append(TicTacToeNode(new_state))
        
        return result
    
    def minMax(self, remaining_height = 10):
        # Final state or reached max height, just return heuristic
        if self.data.ended() or remaining_height == 0:
            return self.data.getHeuristic()
        
        # Get successors and their heuristic values
        succ = self.successors()
        succ_h = [child.minMax(remaining_height - 1) for child in succ]

        if self.data.turn == TicTacToeTable.MAX_SYMBOL:
            best_choice_h = max(succ_h)
        else:
            best_choice_h = min(succ_h)
        
        # Get index of the chosen node
        chosen_index = succ_h.index(best_choice_h)

        # Make chosen node child of this node
        self.addChild(succ[chosen_index])
        return best_choice_h
        

def main():
    start_node = TicTacToeNode(TicTacToeTable())

    res = start_node.minMax()

    print(start_node)
    print()
    curr_node = start_node
    while curr_node != None:
        node = None
        for node in curr_node.children:
            print(node)
            print()
        curr_node = node
        

main()
        