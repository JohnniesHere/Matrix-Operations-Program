#!/bin/bash

# Compile the server code
gcc -o server MatricesCalc_Server.c -lrt -Wall -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    #echo "Server compiled successfully."
	./server
	# Run the server with Valgrind
	#valgrind --leak-check=full --show-leak-kinds=all ./server
else
    #echo "Failed to compile server."
    exit 1
fi




