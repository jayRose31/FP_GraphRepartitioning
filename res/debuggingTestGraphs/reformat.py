# bei den static graphs hab ich ein problem mit der formatierung:
# Die haben nicht werte von 1 bis n, sondern 0 bis n-1
# weil ich von der anderen formatierung ausgehe, ziehe immer 1 ab und bekomme dann
# underflow!
# deshalb muss ich auf alle wert 1 drauf addieren damit es wieder klappt
def reformat_file(input_file, output_file):
    with open(input_file, 'r') as infile, open(output_file, 'w') as outfile:
        lines = infile.readlines()
        if not lines:
            return  # Handle empty file gracefully

        # Write the header as is
        outfile.write(lines[0])

        # Process the remaining lines
        for line in lines[1:]:
            parts = line.split()
            if len(parts) == 3:
                try:
                    first, second, third = map(int, parts)
                    second += 1
                    third += 1
                    outfile.write(f"{first} {second} {third}\n")
                except ValueError:
                    # Skip lines that don't have three integers
                    continue




reformat_file("rgg_n_2_17_s0.graph.seq", "rgg_formatted.seq" )
