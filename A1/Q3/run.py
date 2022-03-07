import subprocess
from time import process_time_ns
import matplotlib.pyplot as plt

programs = [
    {"name":"Sequential", "fileName":"./sequential", "timeTakenList":[]},
    {"name":"Multithread1", "fileName":"./multithread1", "timeTakenList":[]},
    {"name":"Multithread2", "fileName":"./multithread2", "timeTakenList":[]},
    {"name":"Multithread3", "fileName":"./multithread3", "timeTakenList":[]},
]

nVals = ["1","5","10", "20", "100", "200", "300", "500", "1000", "2000"]
# nVals = ["1","5","10"]

for program in programs:
    for val in nVals:
        tStart = process_time_ns()
        subprocess.run([program["fileName"],val])
        program["timeTakenList"].append(process_time_ns()-tStart) 
    
    plt.plot([int(val) for val in nVals], program["timeTakenList"], "-o", label=program["name"])

for program in programs:
    print(program["name"], ":")
    z = [print(x, " : ", y) for x,y in zip(nVals, program["timeTakenList"])]
    print()


plt.legend()
plt.show()

