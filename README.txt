Authorize: Jonathan Elgarisi

## Matrix Operations Program
This project involves a series of C programs designed for matrix operations. The base code handles fundamental matrix operations, while the other codes expand on this base using shared memory and threads to demonstrate different inter-process communication techniques.

## Features
- Perform basic matrix operations: addition, subtraction, multiplication, transpose, and logical operations (AND, OR, NOT).
- Handle matrices containing integers, floating-point numbers, and complex numbers.
- Utilize shared memory for inter-process communication.
- Use threads for concurrent matrix operations.

## How to Run
To compile the programs, use the following commands:

```bash
#Base Code
gcc -Wall MatricesCalculator.c -o MatricesCalculator
#Shared Memory Expansion
gcc -Wall MatricesCalc_Server.c -o MatricesCalc_Server -pthread
gcc -Wall MatricesCalc_Client.c -o MatricesCalc_Client -pthread
#Threads Expansion
gcc -Wall MatricesCalc_Threads.c -o MatricesCalc_Threads -pthread
```

Or by using the run_me files:
```bash
#Base Code
./run_me_1.sh
#Shared Memory Expansion
./run_me_2a.sh
./run_me_2b.sh
#Threads Expansion
./run_me_3.sh
```

## How to Use
If you directly compiled them in the terminal, after successfully compiling the programs, you can run them using the following commands:

### Base Code
```bash
./MatricesCalculator
```
Enter matrix inputs and operations as specified. Supported operations are ADD, SUB, MUL, TRANSPOSE, AND, OR, and NOT.

### Shared Memory Expansion
1. Run the server:
    ```bash
    ./MatricesCalc_Server
    ```
2. Run the client in a separate terminal:
    ```bash
    ./MatricesCalc_Client
    ```

The server initializes shared memory and waits for matrix input. The client sends matrices and operations to the server via shared memory.

### Threads Expansion
```bash
./MatricesCalc_Threads
```
Enter matrix inputs followed by the desired operation (ADD, MUL, AND, OR). The program will perform the operations concurrently using threads.

## Input
Matrix input have to be in the following format:
(rows,columns:value1,value2,...,valueN) 
whereas N = rows * columns.
notes regarding the inputs:
• The matrices must be squared (rows == columns).
• No spacing at all.
• For complex numbers - the imaginary part must have a prefix (i.e. if you want to enter a number+i, you have to write it as number+1i)

## Output
The output of the programs will display the results of the matrix operations. Error messages will provide information if any issues occur.

## Files
	### Base Code Folder
		- MatricesCalculator.c: The base code containing the main matrix operations.
		- run_me_1.sh
		- CMake files
	### Shared Memory Expansion Folder
		- MatricesCalc_Server.c: Server code for shared memory implementation.
		- MatricesCalc_Client.c: Client code for shared memory implementation.
		- run_me_2a.sh
		- run_me_3a.sh
		- CMake files
	### Threads Expansion Folder
		- MatricesCalc_Threads.c: Code for implementing matrix operations using threads.
		- run_me_3.sh
		- CMake files

- README.txt: The text file you're currently reading.

## License
MIT - https://choosealicense.com/licenses/mit/
