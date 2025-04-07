MODULO = 999999937

def matrix_mult(A, B):
    """ Înmulțirea a două matrici 5x5 modulo MODULO """
    return [[sum(A[i][k] * B[k][j] for k in range(5)) % MODULO for j in range(5)] for i in range(5)]

def matrix_exponentiation(M, exp):
    """ Ridicarea matricii 5x5 la puterea exp folosind exponentiere rapidă """
    result = [[1 if i == j else 0 for j in range(5)] for i in range(5)]
    base = M
    
    while exp:
        if exp % 2:
            result = matrix_mult(result, base)
        base = matrix_mult(base, base)
        exp //= 2
    
    return result

def compute_gold_coins(N):
    if N == 0:
        return 1  # La pasul 0, în față avem 1 bănuț

    M = [
        [0, 1, 0, 0, 1],  # Nou P0 = P4 + P1
        [1, 0, 0, 0, 0],  # Nou P1 = P0 (rotație)
        [0, 1, 0, 0, 0],  # Nou P2 = P1 (rotație)
        [0, 0, 1, 0, 0],  # Nou P3 = P2 (rotație)
        [0, 0, 0, 1, 0]   # Nou P4 = P3 (rotație)
    ]
    
    # Ridicăm matricea M la puterea N
    MN = matrix_exponentiation(M, N)
    
    # Vector inițial (toate paharele încep cu 1)
    V0 = [1, 1, 1, 1, 1]
    
    # Calculăm rezultatul: MN * V0
    result = sum(MN[0][i] * V0[i] for i in range(5)) % MODULO
    return result

# Exemplu de rulare
N = int(input())  # Introducem numărul de pași
print(compute_gold_coins(N))
