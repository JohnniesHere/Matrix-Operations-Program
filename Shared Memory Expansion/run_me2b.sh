#!/bin/bash

# Compile the client code
gcc -o client MatricesCalc_Client.c -lrt -Wall -lm

# Check if compilation was successful
if [ $? -eq 0 ]; then
    #echo "Server compiled successfully."
	./client
	# Run the client with Valgrind
	#valgrind --leak-check=full --show-leak-kinds=all ./client
else
    #echo "Failed to compile client."
    exit 1
fi
