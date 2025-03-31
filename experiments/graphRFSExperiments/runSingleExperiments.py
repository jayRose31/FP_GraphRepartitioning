import subprocess
import os
import time
import json

# create file
res_temp = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/results_temporary.txt"
with open(res_temp, "w"):
    pass

analyzer_tool = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/analyzerTool_temporary.txt"
with open(analyzer_tool, "w"):
    pass


# run the program
cpp_executable = "/home/jacob/Dokumente/AldaPraktikum/Code/build/graphRFSExp"

args = ["input.txt", "42"]

result = subprocess.run([cpp_executable] + args, capture_output=True, text=True)

# wichtig zum debuggen
if result.stderr:
    print("Program Errors:")
    print(result.stderr)



# read file and print to cli:
with open(res_temp, "r") as file:
    repartitioning_time = file.readline().strip()

print("Repartitioning Time: ", repartitioning_time)

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


print("Communication Cost: ", communication_cost)

# daten verarbeiten die ich davor eingelesen habe





# delete the file
if os.path.exists(res_temp):
    os.remove(res_temp)
else:
    print("The file does not exist, for some reason...")
    
if os.path.exists(analyzer_tool):
    os.remove(analyzer_tool)
else:
    print("The file does not exist, for some reason...")