#!/bin/bash

# Compile the server code
gcc -o matricesCalcThreads MatricesCalc_Threads.c -lrt -Wall

# Check if compilation was successful
if [ $? -eq 0 ]; then
    #echo "MatricesCalc_Threads.c compiled successfully."
	./matricesCalcThreads
	# Run the calc with Valgrind
	#valgrind --leak-check=full --show-leak-kinds=all ./server
else
    #echo "Failed to compile MatricesCalc_Threads.c"
    exit 1
fi





