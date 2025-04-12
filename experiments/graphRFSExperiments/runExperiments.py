# hier probiere ich das gleiche wie in runExperiments.py, nur dass ich das ganze in eine Funktion packe.

import subprocess
import os
import time
import json
import statistics
import matplotlib.pyplot as plt



# input: args := [sharedMapConfig, graphIn, numberUpdatesTillRepartition] array of String
# Format für Dateipfad ist: "./res/..."
def run_single_experiment(args, multilevel=False):
    
    # create files
    res_temp = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/results_temporary.txt"
    with open(res_temp, "w"):
        pass

    analyzer_tool = "/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/analyzerTool_temporary.txt"
    with open(analyzer_tool, "w"):
        pass

    cpp_executable = ""
    
    # run the program
    if multilevel  :
        cpp_executable = "/home/jacob/Dokumente/AldaPraktikum/Code/build/graphRFSMultilevelExp"
    else:
        cpp_executable = "/home/jacob/Dokumente/AldaPraktikum/Code/build/graphRFSExp"
    
    result = subprocess.run([cpp_executable] + args, capture_output=True, text=True)

    # wichtig zum debuggen
    if result.stderr:
        print("Program Errors:")
        print(result.stderr)
    
    
    # read file and save the results
    with open(res_temp, "r") as file:
        repartitioning_time = file.readline().strip()
        baseline_cost_str = file.readline().strip()
    
    baseline_cost = (int)(baseline_cost_str)
    
        
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
        
        
    return repartitioning_time, communication_cost, baseline_cost


#! adjust to baseline cost return
def run_experiment_with_median(args, multilevel=False):
    # List to store the results of repartitioning times and communication costs
    repartitioning_times = []
    communication_costs = []

    # Run the experiment 3 times
    for _ in range(3):
        repartitioning_time, communication_cost = run_single_experiment(args, multilevel)
        repartitioning_times.append(float(repartitioning_time))  # Convert to float for numerical operations
        communication_costs.append(float(communication_cost))   # Convert to float for numerical operations

    # Calculate the median of the results
    median_repartitioning_time = statistics.median(repartitioning_times)
    median_communication_cost = statistics.median(communication_costs)

    return median_repartitioning_time, median_communication_cost


# this test is to find out, how the repartitioning time and the communication cost
# are affected by the number of update steps.
def test_repartitioning_step_size():
        
    update_steps = [10, 50, 100, 200, 500, 1000]

    # Arrays to store all results
    repartitioning_time_all = []
    communication_cost_all = []

    rep_time_one_round = []
    comm_cost_one_round = []

    print("start running experiments: ")

    number_of_experiments = 5

    for _ in range(number_of_experiments): 
        for steps in update_steps:
            args = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/haggle.seq", str(steps)]
            repartitioning_time, communication_cost = run_experiment_with_median(args)
            rep_time_one_round.append(repartitioning_time)
            comm_cost_one_round.append(communication_cost)
        repartitioning_time_all.append(rep_time_one_round)
        communication_cost_all.append(comm_cost_one_round)
        rep_time_one_round = []
        comm_cost_one_round = []


    experiment_labeling = []
    for i in range(len(update_steps)):
        experiment_labeling.append(i)


    # Plot the results and save plot to file
    plt.figure(figsize=(10, 5))

    for i in range(number_of_experiments):
        plt.scatter(experiment_labeling, repartitioning_time_all[i], marker='o', color="blue")  # Use scatter plot for points
    plt.xlabel('Update Steps')
    plt.title('Repartitioning Time vs Update Steps')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/experiment_results.png")  # Save the plot as a PNG file
    plt.close()

    plt.figure(figsize=(10, 5))
    for i in range(number_of_experiments):
        plt.scatter(experiment_labeling, communication_cost_all[i], marker='o', color="blue")  # Use scatter plot for points
    plt.xlabel('Update Steps')
    plt.title('Communication Cost vs Update Steps')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/experiment_results_communication_cost.png")  # Save the plot as a PNG file
    plt.close()



    print("finished experiments")
    return



# this should test the algorithm for 5 different example graphs
# with varying update steps.
# I want to run the same experiment for the new algorithm and then compare results!
def test_against_multiple_graphs():

    # define configurations with the different graphs
    args1 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/dnc-temporalGraph.seq", "100"]
    args2 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/haggle.seq", "100"]
    args3 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/munmun_digg.seq", "100"]
    #args4 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/proper_loans.seq", "100"]
    args5 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/sociopatterns-infections.seq", "100"]
    
    graph_names = ["dnc-temporalGraph", "haggle", "munmun_digg", "sociopatterns-infections"]
    
    args_all = [args1, args2, args3, args5]
    
    update_step_sizes = [10, 20 ,50, 100]
    
    repTime_one_graph = []
    comm_one_graph = []
    
    repTime_all = []
    comm_all = []
    
    print("start running experiments: ")
    
    # run experiments for all graphs and update step sizes
    # gather data in repTime_all and comm_all
    for args in args_all:
        for step in update_step_sizes:
            args[2] = str(step)
            repartitioning_time, communication_cost = run_experiment_with_median(args)
            repTime_one_graph.append(repartitioning_time)
            comm_one_graph.append(communication_cost)
        repTime_all.append(repTime_one_graph)
        comm_all.append(comm_one_graph)
        repTime_one_graph = []
        comm_one_graph = []
    
    # plot the results in two different plots
    # x achse ist update step size
    # y achse ist repartitioning time
    # Datenpunkte für alle Graphen jeweils eintragen, jeder Graph soll eigene farbe haben
    plt.figure(figsize=(10, 5))
    for i in range(len(args_all)):
        plt.plot(update_step_sizes, repTime_all[i], marker='o', label=f"Graph: {graph_names[i]}")  # Use scatter plot for points
    plt.xlabel('Update Steps')
    plt.ylabel('Repartitioning Time')
    plt.title('Repartitioning Time vs Update Steps')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/experiment_results_multiple_graphs.png")  # Save the plot as a PNG file
    plt.close()
    
    
    
    # x achse ist update step size
    # y achse ist communication cost
    # Datenpunkte für alle Graphen jeweils eintragen, jeder Graph soll eigene farbe haben
    plt.figure(figsize=(10, 5))
    for i in range(len(args_all)):
        plt.plot(update_step_sizes, comm_all[i], marker='o', label=f"Graph: {graph_names[i]}")  # Use scatter plot for points
    plt.xlabel('Update Steps')
    plt.ylabel('Communication Cost')
    plt.title('Communication Cost vs Update Steps')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/experiment_results_multiple_graphs_communication_cost.png")  # Save the plot as a PNG file
    plt.close()
    print("finished experiments")
    return
    


def compare_algorithms_multilevel_and_singlelevel():
    
    
    # define configurations with the different graphs
    args1 = ["./res/sharedMapConfigs/sharedMap_config1.json", "./res/dynGraphs/munmun_digg.seq", "100"]

    # run singlelevel algorithm with different step sizes
    
    # run multilevel algorithm with different step sizes
    
    # print the values in a plot together
    
    
    #update_steps = [10, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000]
    update_steps = [10, 50, 100, 200, 400]

    rep_time_singleLevel= []
    comm_cost_singleLevel = []
    
    comm_cost_baseline = []

    print("start running experiments: ")

    for steps in update_steps:
        repartitioning_time, communication_cost, baseline_cost =  run_single_experiment(args1)   # run_experiment_with_median(args1)
        rep_time_singleLevel.append(repartitioning_time)
        comm_cost_singleLevel.append(communication_cost)
        comm_cost_baseline.append(baseline_cost)
    
    # Plot the communication costs for single-level and baseline
    plt.figure(figsize=(10, 5))
    plt.plot(update_steps, comm_cost_singleLevel, marker='o', label="Single-Level Algorithm", color="blue")
    plt.plot(update_steps, comm_cost_baseline, marker='o', label="Baseline Cost", color="red")
    plt.xlabel('Update Steps')
    plt.ylabel('Communication Cost')
    plt.title('Communication Cost: Single-Level vs Baseline')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/comm_cost_single_vs_baseline.png")  # Save the plot as a PNG file
    plt.close()
    
    # Calculate the quotient of baseline cost and single-level communication cost
    quotient_singleLevel = [baseline / single if single != 0 else 0 for baseline, single in zip(comm_cost_baseline, comm_cost_singleLevel)]

    # Plot the quotient for single-level
    plt.figure(figsize=(10, 5))
    plt.plot(update_steps, [single / baseline if baseline != 0 else 0 for single, baseline in zip(comm_cost_singleLevel, comm_cost_baseline)], 
             marker='o', label="Single-Level / Baseline", color="green")
    plt.xlabel('Update Steps')
    plt.ylabel('Quotient (Single-Level / Baseline)')
    plt.title('Quotient of Single-Level and Baseline Communication Costs')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/quotient_singlelevel_baseline.png")  # Save the plot as a PNG file
    plt.close()
    
    rep_time_multiLevel= []
    comm_cost_multiLevel = []
    comm_cost_baseline = []

    for steps in update_steps:
        repartitioning_time, communication_cost, baseline_cost =  run_single_experiment(args1, True) #run_experiment_with_median(args1, True)
        rep_time_multiLevel.append(repartitioning_time)
        comm_cost_multiLevel.append(communication_cost)
        comm_cost_baseline.append(baseline_cost)
        
    
    # Plot the communication costs for single-level and baseline
    plt.figure(figsize=(10, 5))
    plt.plot(update_steps, comm_cost_multiLevel, marker='o', label="multi-Level Algorithm", color="blue")
    plt.plot(update_steps, comm_cost_baseline, marker='o', label="Baseline Cost", color="red")
    plt.xlabel('Update Steps')
    plt.ylabel('Communication Cost')
    plt.title('Communication Cost: multi-level vs Baseline')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/comm_cost_multi_vs_baseline.png")  # Save the plot as a PNG file
    plt.close()
    
        # Calculate the quotient of baseline cost and single-level communication cost
    # Calculate the quotient of baseline cost and single-level communication cost
    quotient_multiLevel = [multi / baseline if baseline != 0 else 0 for multi, baseline in zip(comm_cost_multiLevel, comm_cost_baseline)]

    # Plot the quotient for multi-level
    plt.figure(figsize=(10, 5))
    plt.plot(update_steps, quotient_multiLevel, marker='o', label="Multi-Level / Baseline", color="purple")
    plt.xlabel('Update Steps')
    plt.ylabel('Quotient (Multi-Level / Baseline)')
    plt.title('Quotient of Multi-Level and Baseline Communication Costs')
    plt.legend()
    plt.grid()
    plt.savefig("/home/jacob/Dokumente/AldaPraktikum/Code/experiments/graphRFSExperiments/quotient_multilevel_baseline.png")  # Save the plot as a PNG file
    plt.close()
    
    
    print("finish experiments")
    





    
    
    
    
compare_algorithms_multilevel_and_singlelevel()


