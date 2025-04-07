nr_tasks = int(input())
andrei_tasks, bogdan_tasks = [int(x) for x in input().split()], [int(x) for x in input().split()]

tasks = list(zip(andrei_tasks, bogdan_tasks))

tasks = sorted(tasks, key=lambda task: (max(task[0], task[1]), abs(task[0] - task[1])))

res = 0
for i in range(0, len(tasks)):
    if i % 2 == 0:
        res += tasks[i][0]
    else:
        res -= tasks[i][1]

if res > 0:
    print("Andrei")
elif res < 0:
    print("Bogdan")
else:
    print("amandoi")