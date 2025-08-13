import subprocess
import os
import time
import json
import statistics
import matplotlib.pyplot as plt
import glob

BASE_DIR = os.path.dirname(__file__)


def run_single_experiment_RFS(args, multilevel=False):
    
    # create files
    res_temp = os.path.join(BASE_DIR, "../graphRFSExperiments/results_temporary.txt")
    with open(res_temp, "w"):
        pass

    analyzer_tool = os.path.join(BASE_DIR, "../graphRFSExperiments/analyzerTool_temporary.txt")
    with open(analyzer_tool, "w"):
        pass

    cpp_executable = ""
    
    # run the program
    if multilevel:
        cpp_executable = os.path.join(BASE_DIR, "../../build/graphRFSMultilevelExp")
    else:
        cpp_executable = os.path.join(BASE_DIR, "../../build/graphRFSExp")
    
    result = subprocess.run([cpp_executable] + args, capture_output=True, text=True)

    # wichtig zum debuggen
    if result.stderr:
        print("Program Errors:")
        print(result.stderr)
    
    
    # read file and save the results
    with open(res_temp, "r") as file:
        repartitioning_time_str = file.readline().strip()
        baseline_cost_str = file.readline().strip()
        baseline_speed_str = file.readline().strip()
        migration_cost_str = file.readline().strip()
    
    
    repartitioning_time = (float)(repartitioning_time_str)
    migration_cost = (float)(migration_cost_str)
    
        
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
    executable_path = os.path.join(BASE_DIR, "../../third_party/ProcessMappingAnalyzer/build/processmappinganalyzer")
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
        
        
    return repartitioning_time, communication_cost, migration_cost



def run_single_experiment_LS(args):
    
    # create files
    res_temp = os.path.join(BASE_DIR, "../graphLSExperiments/results_temporary.txt")
    with open(res_temp, "w"):
        pass

    analyzer_tool = os.path.join(BASE_DIR, "../graphLSExperiments/analyzerTool_temporary.txt")
    with open(analyzer_tool, "w"):
        pass

    
    cpp_executable = os.path.join(BASE_DIR, "../../build/graphLSExp")
    
    result = subprocess.run([cpp_executable] + args, capture_output=True, text=True)

    # wichtig zum debuggen
    if result.stderr:
        print("Program Errors:")
        print(result.stderr)
    
    
    # read file and save the results
    with open(res_temp, "r") as file:
        repartitioning_time_str = file.readline().strip()
        migration_cost_str =  file.readline().strip()
        
    repartitioning_time = (float)(repartitioning_time_str)
    migration_cost = (float)(migration_cost_str)
    

    
        
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
    executable_path = os.path.join(BASE_DIR, "../../third_party/ProcessMappingAnalyzer/build/processmappinganalyzer")
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
        
        
    return repartitioning_time, communication_cost, migration_cost


def compare_algorithms():
    # define configurations with the different graphs
    args1 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/munmun_digg.seq", "100"]
    
    update_steps = [0, 10, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
    #update_steps = [1000, 5000 ,10000, 20000, 30000,40000, 50000, 100000]
    #update_steps = [100, 10000, 50000, 100000]
    
    
    
    # ------------------------------------- compute values for both algorithms ----------------------------------------------
    rep_time_RFS= []
    comm_cost_RFS = []
    migration_costs_RFS = []
    
    for steps in update_steps:
        args1[2] = str(steps)
        repartitioning_time, communication_cost, migration_cost_single =  run_single_experiment_RFS(args1, True)   
        rep_time_RFS.append(repartitioning_time)
        comm_cost_RFS.append(communication_cost)
        migration_costs_RFS.append(migration_cost_single)


    rep_time_LS= []
    comm_cost_LS = []
    migration_costs_LS = []
    
    for steps in update_steps:
        args1[2] = str(steps)
        repartitioning_time, communication_cost, migration_cost_single =  run_single_experiment_LS(args1)   
        rep_time_LS.append(repartitioning_time)
        comm_cost_LS.append(communication_cost)
        migration_costs_LS.append(migration_cost_single)
    
    
    # Plot the communication costs 
    plt.figure(figsize=(10, 5))
    plt.plot(update_steps, comm_cost_LS, marker='o', label="local search Algorithm", color="blue")
    plt.plot(update_steps, comm_cost_RFS, marker='o', label="Repartition from scratch", color="red")
    plt.xlabel('Update Steps')
    plt.ylabel('Communication Cost')
    plt.title('Communication Cost: LS vs RFS')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/sharedExperiments/comm_cost_LS_vs_RFS.png")  # Save the plot as a PNG file
    plt.close()
    
    # Plot the repartitioning times
    plt.figure(figsize=(10, 5))
    plt.plot(update_steps, rep_time_LS, marker='o', label="local search Algorithm", color="blue")
    plt.plot(update_steps, rep_time_RFS, marker='o', label="Repartition from scratch", color="red")
    plt.xlabel('Update Steps')
    plt.ylabel('Repartitioning Time')
    plt.title('Repartitioning Time: LS vs RFS')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/sharedExperiments/rep_time_LS_vs_RFS.png")  # Save the plot as a PNG file
    plt.close()

    # Plot the migration costs
    plt.figure(figsize=(10, 5))
    plt.plot(update_steps, migration_costs_LS, marker='o', label="local search Algorithm", color="blue")
    plt.plot(update_steps, migration_costs_RFS, marker='o', label="Repartition from scratch", color="red")
    plt.xlabel('Update Steps')
    plt.ylabel('Migration Cost')
    plt.title('Migration Cost: LS vs RFS')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/sharedExperiments/migration_cost_LS_vs_RFS.png")  # Save the plot as a PNG file
    plt.close()
    print(migration_costs_RFS)
    print(migration_costs_LS)


# TODO pick correct config for sharedMap
# TODO pick good update stepsize: right now i only test with repartitioning after 100 steps
#? Mit mehreren verschieden stepsizes experimente machen? Wie plotte ich das dann?
def test_all_static_graphs():
        
    results = {}  # Dictionary für die Ergebnisse
    
    output_file = os.path.join(BASE_DIR, "experiment_results_static.json")
    count = 0
    

    
    
    graph_names = ["144_formatted.seq", "589a_formatted.seq", "as-skitter_formatted.seq",
                   "citationCitesee_formatted.seq", "fe_ocean_formatted.seq",
                   "web-Google_formatted.seq"
                   ]
    
    
    for graph_name in graph_names:

        print(f"Processing graph: {graph_name}, Count: {count}")
        
        count += 1
        results[graph_name] = {
            "RFS": {"repartitioning_time": [], "communication_cost": [], "migration_cost": []},
            "RFS_multilevel": {"repartitioning_time": [], "communication_cost": [], "migration_cost": []},
            "LS": {"repartitioning_time": [], "communication_cost": [], "migration_cost": []}
        }
        
        # Erstelle den String im gewünschten Format
        graph_string = f"./res/static_test_graphs/{graph_name}"
        
        # Argumente für die Experimente
        args_RFS = ["./res/sharedMapConfigs/sharedMap_config1.json", graph_string, "100", "1", "2"]
        args_LS = ["./res/sharedMapConfigs/sharedMap_config1.json", graph_string, "100", "1", "2"]
        
        # Führe 5 Runs für jeden Algorithmus aus
        #! Füge mehr prints hinzu
        for iteration in range(5):
            
            if iteration == 1:
                args_RFS[3] = "0"
                args_LS[3] = "0"
                args_RFS[0] = "./res/sharedMapConfigs/config_experiment2.json"
                args_LS[0] = "./res/sharedMapConfigs/config_experiment2.json"
            elif iteration == 2:
                args_RFS[0] = "./res/sharedMapConfigs/config_experiment3.json"
                args_LS[0] = "./res/sharedMapConfigs/config_experiment3.json"
            elif iteration == 3:
                args_RFS[0] = "./res/sharedMapConfigs/config_experiment4.json"
                args_LS[0] = "./res/sharedMapConfigs/config_experiment4.json"
            elif iteration == 4:
                args_RFS[0] = "./res/sharedMapConfigs/config_experiment5.json"
                args_LS[0] = "./res/sharedMapConfigs/config_experiment5.json"
            else:
                args_RFS[0] = "./res/sharedMapConfigs/config_experiment1.json"
                args_LS[0] = "./res/sharedMapConfigs/config_experiment1.json"
            
            # RFS Experiment
            repartitioning_time, communication_cost, migration_cost = run_single_experiment_RFS(args_RFS)
            results[graph_name]["RFS"]["repartitioning_time"].append(repartitioning_time)
            results[graph_name]["RFS"]["communication_cost"].append(communication_cost)
            results[graph_name]["RFS"]["migration_cost"].append(migration_cost)
            
            repartitioning_time, communication_cost, migration_cost = run_single_experiment_RFS(args_RFS, multilevel=True)
            results[graph_name]["RFS_multilevel"]["repartitioning_time"].append(repartitioning_time)
            results[graph_name]["RFS_multilevel"]["communication_cost"].append(communication_cost)
            results[graph_name]["RFS_multilevel"]["migration_cost"].append(migration_cost)
            
            # LS Experiment
            repartitioning_time, communication_cost, migration_cost = run_single_experiment_LS(args_LS)
            results[graph_name]["LS"]["repartitioning_time"].append(repartitioning_time)
            results[graph_name]["LS"]["communication_cost"].append(communication_cost)
            results[graph_name]["LS"]["migration_cost"].append(migration_cost)
    
    
        with open(output_file, "w") as json_file:
            json.dump(results, json_file, indent=4)
        
    print(f"Results saved to {output_file}")


test_all_static_graphs()