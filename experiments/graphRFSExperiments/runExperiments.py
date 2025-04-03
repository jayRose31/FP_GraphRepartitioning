# hier probiere ich das gleiche wie in runExperiments.py, nur dass ich das ganze in eine Funktion packe.

import subprocess
import os
import time
import json
import statistics
import matplotlib.pyplot as plt


# input: args := [sharedMapConfig, graphIn, numberUpdatesTillRepartition] array of String
# Format für Dateipfad ist: "./res/..."
def run_single_experiment(args):
    
    # create files
    res_temp = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/results_temporary.txt"
    with open(res_temp, "w"):
        pass

    analyzer_tool = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/analyzerTool_temporary.txt"
    with open(analyzer_tool, "w"):
        pass


    # run the program
    cpp_executable = "/home/jacob/Dokumente/AldaPraktikum/Code/build/graphRFSExp"
    result = subprocess.run([cpp_executable] + args, capture_output=True, text=True)

    # wichtig zum debuggen
    if result.stderr:
        print("Program Errors:")
        print(result.stderr)
    
    
    # read file and save the results
    with open(res_temp, "r") as file:
        repartitioning_time = file.readline().strip()
    
        
    # ----------------- Benutze tool von Henning (analyzer) -----------------
    with open(analyzer_tool, "r") as file:
        graph_path = file.readline().strip()
        partition_path = file.readline().strip()
        out_path = file.readline().strip()
        hierarchy = file.readline().strip()
        distance = file.readline().strip()
        epsilon = file.readline().strip()
        
    # die file am out_path muss ich noch erstellen.
    with open(out_path, "w"):
        pass
         
    
    # Jetzt benutze ich das tool vom Henning. Abfahrt!
    executable_path = "/home/jacob/Dokumente/AldaPraktikum/Code/third_party/ProcessMappingAnalyzer/build/processmappinganalyzer"
    command = [executable_path, graph_path, partition_path, hierarchy, distance, epsilon, out_path]
    result = subprocess.run(command, capture_output=True, text=True)

    # Dann lese ich die communication cost aus der Datei aus
    with open(out_path, "r") as file:
        data = json.load(file)

    communication_cost = data["comm_cost"]

    #? optional: lösche alle erstellten Dateien (graph_path, partition_path, out_path)

    # ----------------------- done --------------------------------------------

    # delete the file
    if os.path.exists(res_temp):
        os.remove(res_temp)
    else:
        print("The file does not exist, for some reason...")
        
    if os.path.exists(analyzer_tool):
        os.remove(analyzer_tool)
    else:
        print("The file does not exist, for some reason...")
        
        
    return repartitioning_time, communication_cost


def run_experiment_with_median(args):
    # List to store the results of repartitioning times and communication costs
    repartitioning_times = []
    communication_costs = []

    # Run the experiment 3 times
    for _ in range(3):
        repartitioning_time, communication_cost = run_single_experiment(args)
        repartitioning_times.append(float(repartitioning_time))  # Convert to float for numerical operations
        communication_costs.append(float(communication_cost))   # Convert to float for numerical operations

    # Calculate the median of the results
    median_repartitioning_time = statistics.median(repartitioning_times)
    median_communication_cost = statistics.median(communication_costs)

    return median_repartitioning_time, median_communication_cost



# run all the experiments
args1 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/haggle.seq", "1000"]
args2 = ["./res/sharedMapConfigs/sharedMap_config2.json", "./res/dynGraphs/haggle.seq", "1000"]
args3 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/munmun_digg.seq", "1000"]
args4 = ["./res/sharedMapConfigs/sharedMap_config3.json", "./res/dynGraphs/munmun_digg.seq", "100"]





update_steps = [10, 50, 100, 200, 500, 1000]

# Arrays to store all results
repartitioning_time_all = []
communication_cost_all = []

print("start running experiments: ")

for _ in range(5):  # Run the loop 20 times
    for steps in update_steps:
        args = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/munmun_digg.seq", str(steps)]
        repartitioning_time, communication_cost = run_experiment_with_median(args)
        repartitioning_time_all.append(repartitioning_time)
        communication_cost_all.append(communication_cost)


# Write the results to a file
output_file = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/results_summary.txt"
count = 0
with open(output_file, "w") as file:
    file.write("Repartitioning Times:\n")
    for time in repartitioning_time_all:
        file.write(f"{time}\n")
        count += 1
        if count == 6:
            count = 0
            file.write("\n")
        
    file.write("\n")
    file.write("Communication Costs:\n")
    for cost in communication_cost_all:
        file.write(f"{cost}\n")
        count += 1
        if count == 6:
            count = 0
            file.write("\n")
    file.write("\n")

print("finished experiments")