#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <complex.h>
#include <pthread.h>
#include <math.h>

#define MAX_INPUT_LENGTH 128
#define MAX_MATRICES 100


// Structure to represent a matrix --------------------------------------------
typedef struct {
    int rows;
    int cols;
    char ***data;
} Matrix;

// Structure to pass arguments to the thread function ------------------------
typedef struct {
    Matrix *matrix1;
    Matrix *matrix2;
    Matrix *result;
    char operation[10];
    int matrix_type;
} ThreadArgs;

// Function prototypes -------------------------------------------------------
double roundToNearestHalf(double num);
void readMatrix(char input[MAX_INPUT_LENGTH], Matrix *matrix);
void printMatrix(Matrix *matrix);
void addMatrices(Matrix *matrix1, Matrix *matrix2, Matrix *result, const int kindOfNum);
void addMatricesComplex(Matrix *matrix1, Matrix *matrix2, Matrix *result);
void mulMatrices(Matrix *matrix1, Matrix *matrix2, Matrix *result, const int kindOfNum);
void mulMatricesComplex(Matrix *matrix1, Matrix *matrix2, Matrix *result);
double complex parseComplex(const char *str);
void andBinaryMatrix(Matrix *matrix1, Matrix *matrix2, Matrix *result);
void orBinaryMatrix(Matrix *matrix1, Matrix *matrix2, Matrix *result);
void printErrorMessage();
int isMatrix(char input[MAX_INPUT_LENGTH]);
int isBinaryMatrix(Matrix *matrix);
void freeMatrix(Matrix *matrix);
void* performOperation(void* args);


// Function to round a number to the nearest .0
double roundToNearestHalf(double num) {
    return round(num * 2.0) / 2.0;
}

// Function to read a matrix from input ---------------------------------------
void readMatrix(char input[MAX_INPUT_LENGTH], Matrix *matrix) {
    char *token;
    int rows, cols;

    // Read matrix size
    token = strtok(input, ":,()");
    rows = atoi(token);
    token = strtok(NULL, ":,()");
    cols = atoi(token);

    matrix->rows = rows;
    matrix->cols = cols;

    // Allocate memory for matrix rows
    matrix->data = malloc(rows * sizeof(char **));
    if (matrix->data == NULL) {
        perror("malloc");
        return;
    }
    for (int i = 0; i < rows; i++) {
        matrix->data[i] = malloc(cols * sizeof(char *));
        if (matrix->data[i] == NULL) {
            perror("malloc");
            // Free previously allocated memory to avoid leaks
            while (i > 0) {
                free(matrix->data[--i]);
            }
            free(matrix->data);
            return;
        }
        for (int j = 0; j < cols; j++) {
            token = strtok(NULL, ":,()");
            if (token != NULL) {
                matrix->data[i][j] = strdup(token);
                if (matrix->data[i][j] == NULL) {
                    perror("malloc");
                    // Free all allocated memory
                    for (int k = 0; k <= j; k++) {
                        free(matrix->data[i][k]);
                    }
                    while (i > 0) {
                        for (int k = 0; k < cols; k++) {
                            free(matrix->data[i-1][k]);
                        }
                        free(matrix->data[--i]);
                    }
                    free(matrix->data);
                    return;
                }
            }
        }
    }
    //printf("Matrix read successfully\n");
    //print the matrix and its elements using for loop
//    for (int i = 0; i < rows; i++) {
//        for (int j = 0; j < cols; j++) {
//            printf("matrix->data[%d][%d] = %s\n", i, j, matrix->data[i][j]);
//        }
//    }


}


// Function to print a matrix --------------------------------------------------
void printMatrix(Matrix *matrix) {
    printf("(%d,%d:", matrix->rows, matrix->cols);

    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            printf("%s", matrix->data[i][j]);
            if (j < matrix->cols - 1) {
                printf(",");
            }
        }
        if (i < matrix->rows - 1) {
            printf(",");
        }
    }

    printf(")\n");
}

// Function to add two matrices ----------------------------------------------
void addMatrices(Matrix *matrix1, Matrix *matrix2, Matrix *result, const int kindOfNum) {
    if (matrix1->rows != matrix2->rows || matrix1->cols != matrix2->cols) {
        printf("ERR\n");
        return;
    }

    result->rows = matrix1->rows;
    result->cols = matrix1->cols;

    // Allocate memory for result matrix rows
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        // Allocate memory for result matrix columns
        result->data[i] = malloc(result->cols * sizeof(char *));
    }
    if (kindOfNum == 1) {
        for (int i = 0; i < matrix1->rows; i++) {
            for (int j = 0; j < matrix1->cols; j++) {
                int value1 = atof(matrix1->data[i][j]);
                int value2 = atof(matrix2->data[i][j]);
                int sum = value1 + value2;
                // Calculate the length of the result string
                int length = snprintf(NULL, 0, "%d", sum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%d", sum);
            }
        }
    } else if (kindOfNum == 2) {
        for (int i = 0; i < matrix1->rows; i++) {
            for (int j = 0; j < matrix1->cols; j++) {
                double value1 = roundToNearestHalf(atof(matrix1->data[i][j]));
                double value2 = roundToNearestHalf(atof(matrix2->data[i][j]));
                double sum = value1 + value2;
                int length = snprintf(NULL, 0, "%.1f", roundToNearestHalf(sum)) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.1f", roundToNearestHalf(sum));
            }
        }
    }
}

// Function to add two matrices ----------------------------------------------
void addMatricesComplex(Matrix *matrix1, Matrix *matrix2, Matrix *result) {
    if (matrix1->rows != matrix2->rows || matrix1->cols != matrix2->cols) {
        printf("ERR\n");
        return;
    }

    result->rows = matrix1->rows;
    result->cols = matrix1->cols;

    // Allocate memory for result matrix rows
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        // Allocate memory for result matrix columns
        result->data[i] = malloc(result->cols * sizeof(char *));
    }

    for (int i = 0; i < matrix1->rows; i++) {
        for (int j = 0; j < matrix1->cols; j++) {
            double real1 = 0, imag1 = 0, real2 = 0, imag2 = 0;

            // Parse first matrix complex number
            sscanf(matrix1->data[i][j], "%lf%lfi", &real1, &imag1);
            // Parse second matrix complex number
            sscanf(matrix2->data[i][j], "%lf%lfi", &real2, &imag2);

            // Sum of real and imaginary parts
            double realSum = real1 + real2;
            double imagSum = imag1 + imag2;

            // Determine the appropriate output format
            int length;
            if (realSum == 0 && imagSum == 0) {
                length = snprintf(NULL, 0, "0") + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "0");
            } else if (realSum == 0) {
                length = snprintf(NULL, 0, "%.0fi", imagSum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0fi", imagSum);
//            } else if (imagSum == 0) {
//                length = snprintf(NULL, 0, "%.0f", realSum) + 1;
//                result->data[i][j] = malloc(length);
//                snprintf(result->data[i][j], length, "%.0f", realSum);
            } else {
                length = snprintf(NULL, 0, "%.0f%+.0fi", realSum, imagSum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0f%+.0fi", realSum, imagSum);
            }
        }
    }
}

// Function to perform matrix multiplication with real or integer numbers ----
void mulMatrices(Matrix *matrix1, Matrix *matrix2, Matrix *result, const int kindOfNum) {
    if (matrix1->cols != matrix2->rows) {
        printf("ERR\n");
        return;
    }

    result->rows = matrix1->rows;
    result->cols = matrix2->cols;

    // Allocate memory for result matrix rows
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(char *));
        for (int j = 0; j < result->cols; j++) {
            double sum = 0;
            for (int k = 0; k < matrix1->cols; k++) {
                double value1 = roundToNearestHalf(atof(matrix1->data[i][k]));
                double value2 = roundToNearestHalf(atof(matrix2->data[k][j]));
                sum += value1 * value2;
            }

            int length;
            if (kindOfNum == 1) {  // Integer
                length = snprintf(NULL, 0, "%d", (int)sum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%d", (int)sum);
            } else {  // Floating point
                length = snprintf(NULL, 0, "%.1f", sum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.1f", sum);
            }
        }
    }
}

// Function to parse a complex number from a string -------------------------
double complex parseComplex(const char *str) {
    double real = 0.0, imag = 0.0;
    char sign = '+';
    int offset = 0;

    // First try to read in the form a+bi or a-bi
    if (sscanf(str, "%lf%c%lfi%n", &real, &sign, &imag, &offset) >= 3 && offset && !str[offset]) {
        // Adjust imaginary part based on sign
        if (sign == '-') imag = -imag;
        return real + imag * I;
    }
    // Try to read as just an imaginary number (i, +i, -i, 2i, +2i, -2i)
    if (sscanf(str, "%lf%*1[iI]%n", &imag, &offset) == 1 && offset && !str[offset]) {
        return imag * I;
    }
    // If only "i", "+i" or "-i"
    if (strchr(str, 'i') != NULL) {
        if (str[0] == '-') {
            return -I;
        }
        return I;
    }
    // Try to read as just a real number
    sscanf(str, "%lf%n", &real, &offset);
    return real + 0.0 * I;
}

// Function to perform matrix multiplication with complex numbers ----------
void mulMatricesComplex(Matrix *matrix1, Matrix *matrix2, Matrix *result) {
    if (matrix1->cols != matrix2->rows) {
        printf("ERR\n");
        return;
    }

    result->rows = matrix1->rows;
    result->cols = matrix2->cols;

    // Allocate memory for result matrix rows
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(char *));
        for (int j = 0; j < result->cols; j++) {
            double complex sum = 0;
            for (int k = 0; k < matrix1->cols; k++) {
                double complex value1 = parseComplex(matrix1->data[i][k]);
                double complex value2 = parseComplex(matrix2->data[k][j]);
                sum += value1 * value2;
            }

            // Determine the appropriate output format
            int length;
            if (creal(sum) == 0 && cimag(sum) == 0) {
                length = snprintf(NULL, 0, "0") + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "0");
            } else if (creal(sum) == 0) {
                length = snprintf(NULL, 0, "%.0fi", cimag(sum)) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0fi", cimag(sum));
//            } else if (cimag(sum) == 0) {
//                length = snprintf(NULL, 0, "%.0f", creal(sum)) + 1;
//                result->data[i][j] = malloc(length);
//                snprintf(result->data[i][j], length, "%.0f", creal(sum));
            } else {
                length = snprintf(NULL, 0, "%.0f%+.0fi", creal(sum), cimag(sum)) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0f%+.0fi", creal(sum), cimag(sum));
            }
        }
    }
}

// Helper function to check if the matrix contains only binary values (0 or 1)
int isBinaryMatrix(Matrix *matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            if (strcmp(matrix->data[i][j], "0") != 0 && strcmp(matrix->data[i][j], "1") != 0) {
                return 0; // Not a binary value
            }
        }
    }
    return 1; // All values are binary
}

// Function to perform binary AND on two matrices --------------------------
void andBinaryMatrix(Matrix *matrix1, Matrix *matrix2, Matrix *result) {
    if (matrix1->rows != matrix2->rows || matrix1->cols != matrix2->cols) {
        printf("ERR: Matrices dimensions mismatch\n");
        return;
    }

    result->rows = matrix1->rows;
    result->cols = matrix1->cols;
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(char *));
        for (int j = 0; j < result->cols; j++) {
            int resultValue = (matrix1->data[i][j][0] == '1' && matrix2->data[i][j][0] == '1') ? 1 : 0;
            result->data[i][j] = malloc(2 * sizeof(char));
            sprintf(result->data[i][j], "%d", resultValue);
        }
    }
}

// Function to perform binary OR on two matrices ----------------------------
void orBinaryMatrix(Matrix *matrix1, Matrix *matrix2, Matrix *result) {
    if (matrix1->rows != matrix2->rows || matrix1->cols != matrix2->cols) {
        printf("ERR: Matrices dimensions mismatch\n");
        return;
    }


    result->rows = matrix1->rows;
    result->cols = matrix1->cols;
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(char *));
        for (int j = 0; j < result->cols; j++) {
            int resultValue = (matrix1->data[i][j][0] == '1' || matrix2->data[i][j][0] == '1') ? 1 : 0;
            result->data[i][j] = malloc(2 * sizeof(char));
            sprintf(result->data[i][j], "%d", resultValue);
        }
    }
}

// Function to print error message --------------------------------------------
void printErrorMessage() {
    printf("ERR\n");
}

// Function to check if the input is a matrix -----------------------------------
int isMatrix(char input[MAX_INPUT_LENGTH]) {
    int len = strlen(input);
    int hasDecimal = 0, hasImaginary = 0;

    // Check if the input starts with '(' and ends with ')'
    if (input[0] != '(' || input[len - 1] != ')') {
        return 0;
    }

    // Check for the presence of a comma between row and column counts
    char *commaPos = strchr(input, ',');
    if (commaPos == NULL || commaPos == input + 1 || commaPos == input + len - 2) {
        return 0;
    }

    // Check if there is a colon after row and column counts
    char *colonPos = strchr(input, ':');
    if (colonPos == NULL || colonPos == input + 1 || colonPos == input + len - 2) {
        return 0;
    }

    // Extract row and column numbers
    char rowStr[MAX_INPUT_LENGTH];
    char colStr[MAX_INPUT_LENGTH];
    strncpy(rowStr, input + 1, commaPos - input - 1);
    rowStr[commaPos - input - 1] = '\0';
    strncpy(colStr, commaPos + 1, colonPos - commaPos - 1);
    colStr[colonPos - commaPos - 1] = '\0';

    // Check if row and column numbers are valid integers
    for (int i = 0; i < strlen(rowStr); i++) {
        if (!isdigit(rowStr[i])) {
            return 0;
        }
    }
    for (int i = 0; i < strlen(colStr); i++) {
        if (!isdigit(colStr[i])) {
            return 0;
        }
    }



    // Check if the elements after the colon are complex numbers
    for (int i = 5; i < len-1; i++) {
        if (input[i] == 'i') {
            hasImaginary = 1;
        }
    }

    // Check if the elements after the colon are rational numbers
    for (int i = 5; i < len -1 ; i++) {
        if (input[i] == '.') {
            hasDecimal = 1;
        }
    }

    if (hasImaginary == 1 && hasDecimal == 1) {
        return 0;
    }

    if (hasImaginary == 1) {
        return 3;
    }

    // Check if the elements after the colon are valid
    char *elementStr = colonPos + 1;
    int elementCount = 0;
    char *token = strtok(elementStr, ",)");
    // Count the number of elements
    while (token != NULL) {
        elementCount++;
        token = strtok(NULL, ",)");
    }

    // Check if the number of elements matches the row and column count
    int rows = atoi(rowStr);
    int cols = atoi(colStr);
    if (elementCount != rows * cols) {
        return 0;
    }

    if (hasImaginary == 1) {
        return 3;
    } else if (hasDecimal == 1) {
        return 2;
    } else {
        return 1;
    }
}

// Function to free the allocated memory for the matrix ----------------------
void freeMatrix(Matrix *matrix) {
    for (int i = 0; i < matrix->rows; i++) {
        for (int j = 0; j < matrix->cols; j++) {
            free(matrix->data[i][j]);
        }
        free(matrix->data[i]);
    }
    free(matrix->data);
}

void* performOperation(void* args) {
    ThreadArgs* threadArgs = (ThreadArgs*)args;
    int matrix_type = threadArgs->matrix_type;

    Matrix* result = malloc(sizeof(Matrix));
    if (result == NULL) {
        perror("malloc");
        return NULL;
    }

    // Initialize result->data to NULL
    result->data = NULL;

    if (strcmp(threadArgs->operation, "ADD") == 0) {
        if (matrix_type == 1 || matrix_type == 2) {
            addMatrices(threadArgs->matrix1, threadArgs->matrix2, result, matrix_type);
        } else if (matrix_type == 3) {
            addMatricesComplex(threadArgs->matrix1, threadArgs->matrix2, result);
        }
    } else if (strcmp(threadArgs->operation, "MUL") == 0) {
        if (matrix_type == 1 || matrix_type == 2) {
            mulMatrices(threadArgs->matrix1, threadArgs->matrix2, result, matrix_type);
        } else if (matrix_type == 3) {
            mulMatricesComplex(threadArgs->matrix1, threadArgs->matrix2, result);
        }
    } else if (strcmp(threadArgs->operation, "AND") == 0) {
        andBinaryMatrix(threadArgs->matrix1, threadArgs->matrix2, result);
    } else if (strcmp(threadArgs->operation, "OR") == 0) {
        orBinaryMatrix(threadArgs->matrix1, threadArgs->matrix2, result);
    }

    return result;
}


int main() {
    char input[MAX_INPUT_LENGTH];

    while (1) {  // This is the main loop that will keep the program running
        int matrixCount = 0;
        char operation[4];
        int matrix_type;
        char *copy = NULL;
        Matrix matrices[MAX_MATRICES];

        // Read matrices until an operation is encountered
        while (1) {
            if (fgets(input, sizeof(input), stdin) == NULL) {
                printf("ERR: Unexpected end of input\n");
                return 1;
            }
            input[strcspn(input, "\n")] = 0;  // Remove newline

            if (strcmp(input, "END") == 0) {
                // Free memory
                for (int i = 0; i < matrixCount; i++) {
                    freeMatrix(&matrices[i]);
                }
                return 0;  // Exit the program if END is entered
            }


            if (strcmp(input, "ADD") == 0 || strcmp(input, "MUL") == 0 ||
                strcmp(input, "AND") == 0 || strcmp(input, "OR") == 0) {
                strcpy(operation, input);
                break;
            }

            if (matrixCount >= MAX_MATRICES) {
                printf("ERR: Too many matrices\n");
                continue;  // Start over with new input
            }
            copy = strdup(input);
            matrix_type = isMatrix(copy);
            free(copy);
            if (matrix_type != 0) {
                readMatrix(input, &matrices[matrixCount++]);
            } else {
                printf("ERR: Invalid input\n");
                continue;  // Start over with new input
            }
        }

        if (matrixCount % 2 != 0) {
            printf("ERR: Odd number of matrices\n");
            for (int i = 0; i < matrixCount; i++) {
                freeMatrix(&matrices[i]);
            }
            continue;  // Start over with new input
        }


        int numThreads = matrixCount / 2;
        pthread_t threads[numThreads];
        ThreadArgs threadArgs[numThreads];
        Matrix results[numThreads];

// Create and run threads
    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].matrix1 = &matrices[i*2];
        threadArgs[i].matrix2 = &matrices[i*2+1];
        threadArgs[i].result = &results[i];
        strcpy(threadArgs[i].operation, operation);
        threadArgs[i].matrix_type = matrix_type;  // Add this line

        if (pthread_create(&threads[i], NULL, performOperation, (void*)&threadArgs[i]) != 0) {
            printf("ERR: Failed to create thread\n");
            return 1;
        }
    }

// Wait for all threads to complete and collect results
    Matrix* threadResults[numThreads];
    for (int i = 0; i < numThreads; i++) {
        if (pthread_join(threads[i], (void**)&threadResults[i]) != 0) {
            printf("ERR: Failed to join thread\n");
            return 1;
        }
    }
    //If numThreads == 0, continue
    if (numThreads == 0) {
        continue;
    }
//If there is only one thread, the final result is the result of that thread
    if (numThreads == 1) {
        printMatrix(threadResults[0]);
        // Free memory
        for (int i = 0; i < matrixCount; i++) {
            freeMatrix(&matrices[i]);
        }
        freeMatrix(threadResults[0]);
        free(threadResults[0]);
        continue;
    }


// Perform final operation on results
    Matrix* finalResult = threadResults[0];
    for (int i = 1; i < numThreads; i++) {
        Matrix* tempResult = malloc(sizeof(Matrix));
        if (tempResult == NULL) {
            perror("malloc");
            return 1;
        }

        if (strcmp(operation, "ADD") == 0) {
            if(matrix_type == 1 || matrix_type == 2) {
                addMatrices(finalResult, threadResults[i], tempResult, matrix_type);
            } else if (matrix_type == 3) {
                addMatricesComplex(finalResult, threadResults[i], tempResult);
            }
        } else if (strcmp(operation, "MUL") == 0) {
            if (matrix_type == 1 || matrix_type == 2) {
                mulMatrices(finalResult, threadResults[i], tempResult, matrix_type);
            } else if (matrix_type == 3) {
                mulMatricesComplex(finalResult, threadResults[i], tempResult);
            }
        } else if (strcmp(operation, "AND") == 0) {
            andBinaryMatrix(finalResult, threadResults[i], tempResult);
        } else if (strcmp(operation, "OR") == 0) {
            orBinaryMatrix(finalResult, threadResults[i], tempResult);
        }
        freeMatrix(threadResults[i]);
        free(threadResults[i]);

        freeMatrix(finalResult);
        free(finalResult);
        finalResult = tempResult;

    }

// Print final result
        printMatrix(finalResult);

        // Free memory
        for (int i = 0; i < matrixCount; i++) {
            freeMatrix(&matrices[i]);
        }
        freeMatrix(finalResult);
        free(finalResult);

        // The loop will now go back to the beginning, ready for new input
    }

    return 0;  // This line will never be reached, but it's good practice to have it
}