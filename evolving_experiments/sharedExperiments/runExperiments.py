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
    graph_path = ""
    out_path = ""

    if type == "RFS":
        # path for RFS
        base_partition_path = os.path.join(BASE_DIR, "../graphRFSExperiments/Graph_partition")
        graph_path = os.path.join(BASE_DIR, "../graphRFSExperiments/Graph_metis")
        out_path = os.path.join(BASE_DIR, "../graphRFSExperiments/out_graph")
    
    if type == "LS":
        # path for LS
        base_partition_path = os.path.join(BASE_DIR, "../graphLSExperiments/Graph_partition")
        graph_path = os.path.join(BASE_DIR, "../graphLSExperiments/Graph_metis")
        out_path = os.path.join(BASE_DIR, "../graphLSExperiments/out_graph")
    
    
    
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
        
        #? maybe delete the out_path just to be safe 
        if os.path.exists(out_path):
            os.remove(out_path)
            
    
    return comm_costs


def determine_migration_cost(type="RFS"):

    file_path = ""

    if type == "RFS":
        # path for RFS
        file_path = os.path.join(BASE_DIR, "../graphRFSExperiments/migration_results.txt")
    if type == "LS":
        # path for LS
        file_path = os.path.join(BASE_DIR, "../graphLSExperiments/migration_results.txt")

    with open(file_path, "r") as file:
        results = [float(line.strip()) for line in file.readlines()]
    return results


def determine_rep_time(type="RFS"):

    file_path = ""

    if type == "RFS":
        # path for RFS
        file_path = os.path.join(BASE_DIR, "../graphRFSExperiments/time_results.txt")
    if type == "LS":
        # path for LS
        file_path = os.path.join(BASE_DIR, "../graphLSExperiments/time_results.txt")

    with open(file_path, "r") as file:
        results = [float(line.strip()) for line in file.readlines()]
    return results



def clean_up(type="RFS"):

    base_path = ""

    if type == "RFS":
        # path for RFS
        base_path = os.path.join(BASE_DIR, "../graphRFSExperiments/")
    if type == "LS":
        # path for LS
        base_path = os.path.join(BASE_DIR, "../graphLSExperiments/")
    
    #? delete the graph metis file?
    #! a bit dangerous because we need it over multiple runs...
    
    number_experiments = 6
    
    for i in range(number_experiments):
        partition_path = base_path + "Graph_partition" + str(i+1) 
        if os.path.exists(partition_path):
            os.remove(partition_path)
    
    if os.path.exists(base_path + "migration_results.txt"):
        os.remove(base_path + "migration_results.txt")
    
    if os.path.exists(base_path + "time_results.txt"):
        os.remove(base_path + "time_results.txt")
    
    
    return



# this now just executes the programm
# use determine functions to get values
def run_single_experiment_RFS(args, multilevel=False):
    
    cpp_executable = ""
    
    # run the program
    if multilevel:
        cpp_executable = os.path.join(BASE_DIR, "../../build/graphRFSMultilevel_evolving")
    else:
        cpp_executable = os.path.join(BASE_DIR, "../../build/graphRFS_evolving")
    
    result = subprocess.run([cpp_executable] + args, capture_output=True, text=True)

    # wichtig zum debuggen
    if result.stderr:
        print("output to std::err:")
        print(result.stderr)
        
    return 


def run_single_experiment_LS(args):
    
    cpp_executable = ""
    
    # run the program
    cpp_executable = os.path.join(BASE_DIR, "../../build/graphLS_evolving")
    
    result = subprocess.run([cpp_executable] + args, capture_output=True, text=True)

    # wichtig zum debuggen
    if result.stderr:
        print("output to std::err:")
        print(result.stderr)
        
    return 




def test_all_static_graphs(size, output_file_name = ""):
        
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
        args = ["./res/sharedMapConfigs/config_experiment1.json", graph_string, "1"]
        
        
        # Führe 5 Runs für jeden Algorithmus aus
        #! Füge mehr prints hinzu
        for iteration in range(1):
            
            if iteration == 1:
                args[3] = "0"
                args[0] = "./res/sharedMapConfigs/config_experiment2.json"
            elif iteration == 2:
                args[0] = "./res/sharedMapConfigs/config_experiment3.json"
            elif iteration == 3:
                args[0] = "./res/sharedMapConfigs/config_experiment4.json"
            elif iteration == 4:
                args[0] = "./res/sharedMapConfigs/config_experiment5.json"
            else:
                args[0] = "./res/sharedMapConfigs/config_experiment1.json"
                
            
            # RFS Experiment
            run_single_experiment_RFS(args)
            comm_costs = determine_comm_cost()
            migration_costs = determine_migration_cost()
            rep_times = determine_rep_time()
            clean_up()
            
            for k in range(len(comm_costs)):
                results[graph_name]["RFS"]["repartitioning_time"].append(rep_times[k])
                results[graph_name]["RFS"]["communication_cost"].append(comm_costs[k])
                results[graph_name]["RFS"]["migration_cost"].append(migration_costs[k])
                
            # RFS multilevel Experiment
            run_single_experiment_RFS(args, multilevel=True)
            comm_costs = determine_comm_cost()
            migration_costs = determine_migration_cost()
            rep_times = determine_rep_time()
            clean_up()
            
            for k in range(len(comm_costs)):
                results[graph_name]["RFS_multilevel"]["repartitioning_time"].append(rep_times[k])
                results[graph_name]["RFS_multilevel"]["communication_cost"].append(comm_costs[k])
                results[graph_name]["RFS_multilevel"]["migration_cost"].append(migration_costs[k])
            
            
            # LS Experiment
            run_single_experiment_LS(args)
            comm_costs = determine_comm_cost("LS")
            migration_costs = determine_migration_cost("LS")
            rep_times = determine_rep_time("LS")
            clean_up("LS")
            
            for k in range(len(comm_costs)):
                results[graph_name]["LS"]["repartitioning_time"].append(rep_times[k])
                results[graph_name]["LS"]["communication_cost"].append(comm_costs[k])
                results[graph_name]["LS"]["migration_cost"].append(migration_costs[k])
            
    
    
        with open(output_file, "w") as json_file:
            json.dump(results, json_file, indent=4)
        
    print(f"Results saved to {output_file}")


# TODO pick correct config for sharedMap
# TODO pick good update stepsize: right now i only test with repartitioning after 100 steps
#? Mit mehreren verschieden stepsizes experimente machen? Wie plotte ich das dann?
def test_all_graphs(size , output_file_name = ""):
        
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
        args = ["./res/sharedMapConfigs/config_experiment1.json", graph_string, "1" ]
        
        # Führe 5 Runs für jeden Algorithmus aus
        for iteration in range(1):
            
            if iteration == 1:
                args[3] = "0"
                args[0] = "./res/sharedMapConfigs/config_experiment2.json"
            elif iteration == 2:
                args[0] = "./res/sharedMapConfigs/config_experiment3.json"
            elif iteration == 3:
                args[0] = "./res/sharedMapConfigs/config_experiment4.json"
            elif iteration == 4:
                args[0] = "./res/sharedMapConfigs/config_experiment5.json"
            else:
                args[0] = "./res/sharedMapConfigs/config_experiment1.json"
                
            
            # RFS Experiment
            run_single_experiment_RFS(args)
            comm_costs = determine_comm_cost()
            migration_costs = determine_migration_cost()
            rep_times = determine_rep_time()
            clean_up()
            
            for k in range(len(comm_costs)):
                results[graph_name]["RFS"]["repartitioning_time"].append(rep_times[k])
                results[graph_name]["RFS"]["communication_cost"].append(comm_costs[k])
                results[graph_name]["RFS"]["migration_cost"].append(migration_costs[k])
                
            # RFS multilevel Experiment
            run_single_experiment_RFS(args, multilevel=True)
            comm_costs = determine_comm_cost()
            migration_costs = determine_migration_cost()
            rep_times = determine_rep_time()
            clean_up()
            
            for k in range(len(comm_costs)):
                results[graph_name]["RFS_multilevel"]["repartitioning_time"].append(rep_times[k])
                results[graph_name]["RFS_multilevel"]["communication_cost"].append(comm_costs[k])
                results[graph_name]["RFS_multilevel"]["migration_cost"].append(migration_costs[k])
            
            
            # LS Experiment
            run_single_experiment_LS(args)
            comm_costs = determine_comm_cost("LS")
            migration_costs = determine_migration_cost("LS")
            rep_times = determine_rep_time("LS")
            clean_up("LS")
            
            for k in range(len(comm_costs)):
                results[graph_name]["LS"]["repartitioning_time"].append(rep_times[k])
                results[graph_name]["LS"]["communication_cost"].append(comm_costs[k])
                results[graph_name]["LS"]["migration_cost"].append(migration_costs[k])
            
    
        with open(output_file, "w") as json_file:
            json.dump(results, json_file, indent=4)
        
    print(f"Results saved to {output_file}")


test_all_static_graphs("m", "testrun.json")
