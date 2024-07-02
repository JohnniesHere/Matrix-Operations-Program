#!/bin/bash

# Compile the server code
gcc -o matCalculator MatricesCalculator.c -lrt -Wall

# Check if compilation was successful
if [ $? -eq 0 ]; then
    #echo "Server compiled successfully."
	./matCalculator
	# Run the matCalculator with Valgrind
	#valgrind --leak-check=full --show-leak-kinds=all ./server
else
    #echo "Failed to compile matCalculator."
    exit 1
fi





