import subprocess
import os
import time
import json
import statistics
import matplotlib.pyplot as plt
import glob

BASE_DIR = os.path.dirname(__file__)

# get the communication cost for all the 6 partitions
def determine_comm_cost(type = "RFS"):

    config_path = os.path.join(BASE_DIR, "../../res/sharedMapConfigs/config_experiment1.json")

    with open(config_path, "r") as file1:
        configdata = json.load(file1)
    
    hierarchy = configdata["hierarchy"] 
    distance = configdata["distance"]
    epsilon = configdata["imbalance"]
    
    hierarchy_str = ":".join(map(str, hierarchy))
    distance_str = ":".join(map(str, distance))

    base_partition_path = ""

    if type == "RFS":
        # path for RFS
        base_partition_path = os.path.join(BASE_DIR, "../graphRFSExperiments/Graph_partition")
    if type == "LS":
        # path for LS
        base_partition_path = os.path.join(BASE_DIR, "../graphLSExperiments/Graph_partition")
    
    
    
    graph_path = os.path.join(BASE_DIR, "../graphRFSExperiments/Graph_metis")
    out_path = os.path.join(BASE_DIR, "../graphRFSExperiments/out_graph")
    executable_path = os.path.join(BASE_DIR, "../../third_party/ProcessMappingAnalyzer/build/processmappinganalyzer")
    
    
    comm_costs = []
    number_experiments = 6
    
    
    for i in range(number_experiments):
        
        # Jetzt benutze ich das tool vom Henning. Abfahrt!
        partition_path = base_partition_path + str(i+1) 
        
        # die file am out_path muss ich noch erstellen.
        with open(out_path, "w"):
            pass
        
        
        command = [executable_path, graph_path, partition_path, hierarchy_str, distance_str, str(epsilon), out_path]
        
        result = subprocess.run(command, capture_output=True, text=True)

        if result.returncode != 0:
            print(f"Error running {executable_path}: {result.stderr}")

        # Dann lese ich die communication cost aus der Datei aus
        with open(out_path, "r") as file:
            data = json.load(file)

        communication_cost = data["comm_cost"]
        comm_costs.append(communication_cost)
        
        #? maybe delete the out_path just for to be safe 
        if os.path.exists(out_path):
            os.remove(out_path)
            
    
    return comm_costs

    



costs = determine_comm_cost()
print(costs)

# read the file and extract the values
def determine_migration_cost():
    return 1



def determine_rep_time():
    return 1


#TODO: function to put it all together

#TODO: cleanup function?


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
    
    print(baseline_cost_str)
    
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


def test_all_static_graphs(size, output_file_name = "", experiment_config = "0"):
        
    results = {}  # Dictionary für die Ergebnisse
    
    output_file = os.path.join(BASE_DIR, output_file_name)
    if output_file_name == "":
        output_file = os.path.join(BASE_DIR, "experiment_results_static.json")
        
    count = 0
    

    graph_names = [""]
    
    if size == "m":
        graph_names = [ "144_formatted.seq", "598a_formatted.seq", 
                   "citationCiteseer_formatted.seq", "brack2_formatted.seq",
                   "enron_formatted.seq", "fe_body_formatted.seq",
                   "bcsstk29_formatted.seq" , "fe_tooth_formatted.seq" ,
                   "finan512_formatted.seq", "t60k_formatted.seq",
                   "wing_formatted.seq", "fe_ocean_formatted.seq"
                    ]
    elif size == "l":
        graph_names = [ "cnr-2000_formatted.seq",
                   "coAuthorsDBLP_formatted.seq", "as-skitter_formatted.seq",
                   "web-Google_formatted.seq", "eu-2005_formatted.seq" 
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
        args_RFS = ["./res/sharedMapConfigs/config_experiment1.json", graph_string, "100", "1", experiment_config]
        args_LS = ["./res/sharedMapConfigs/config_experiment1.json", graph_string, "100", "1", experiment_config]
        
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
            repartitioning_time, communication_cost, migration_cost = run_single_experiment_RFS(args_RFS, multilevel=False)
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


# TODO pick correct config for sharedMap
# TODO pick good update stepsize: right now i only test with repartitioning after 100 steps
#? Mit mehreren verschieden stepsizes experimente machen? Wie plotte ich das dann?
def test_all_graphs(size , output_file_name = "", experiment_config = "0"):
        
    results = {}  # Dictionary für die Ergebnisse
    
    output_file = os.path.join(BASE_DIR, output_file_name)
    if output_file_name == "":
        output_file = os.path.join(BASE_DIR, "experiment_results.json")
    
    count = 0
    

    graph_names = [""]
    
    if size == "m":
        graph_names = ["haggle.seq", "dnc-temporalGraph.seq", "sociopatterns-infections.seq",
                   "munmun_digg.seq", "topology.seq", "facebook-wosn-wall.seq", 
                   "movielens10m.seq", "lkml-reply.seq" , "proper_loans.seq", 
                   "wiki_simple_en.seq", "lastfm_band.seq", "stackexchange-stackoverflow.seq",
                   "citeulike_ui.seq"
                     ]
    elif size == "l":
        graph_names = [
                      "dewiki_clean.seq","amazon-ratings.seq","flickr-growth.seq" , "youtube-u-growth.seq"
                     ]
    #"wikipedia-growth.seq",
    # -> something wrong with sharedmap processing big graphs!!
    
    for graph_name in graph_names:

        print(f"Processing graph: {graph_name}, Count: {count}")
        
        count += 1
        results[graph_name] = {
            "RFS": {"repartitioning_time": [], "communication_cost": [], "migration_cost": []},
            "RFS_multilevel": {"repartitioning_time": [], "communication_cost": [], "migration_cost": []},
            "LS": {"repartitioning_time": [], "communication_cost": [], "migration_cost": []}
        }
        
        # Erstelle den String im gewünschten Format
        graph_string = f"./res/final_real_dyn/{graph_name}"
        
        # Argumente für die Experimente
        args_RFS = ["./res/sharedMapConfigs/config_experiment1.json", graph_string, "100", "1", experiment_config ]
        args_LS = ["./res/sharedMapConfigs/config_experiment1.json", graph_string, "100", "1",  experiment_config ]
        
        # Führe 5 Runs für jeden Algorithmus aus
        #! Füge mehr prints hinzu
        for iteration in range(1):
            
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
            print(repartitioning_time)
            print(communication_cost)
            print(migration_cost)
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

