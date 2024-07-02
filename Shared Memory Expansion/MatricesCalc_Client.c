#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <complex.h>

#define SHM_KEY 1234
#define MAX_INPUT_LENGTH 128
#define SHM_SIZE 2048

typedef struct {
    int numStrings;
    char data[1][MAX_INPUT_LENGTH ];  // Flexible array member for string data
} SharedMemory;


// Structure to represent a matrix --------------------------------------------
typedef struct {
    int rows;
    int cols;
    char ***data;
} Matrix;

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
        printf("Memory allocation failed\n");
        return;
    }
    for (int i = 0; i < rows; i++) {
        matrix->data[i] = malloc(cols * sizeof(char *));
        if (matrix->data[i] == NULL) {
            printf("Memory allocation failed\n");
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
                    printf("Memory allocation failed\n");
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
//    printf("Matrix read successfully:\n");
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
                double value1 = atof(matrix1->data[i][j]);
                double value2 = atof(matrix2->data[i][j]);
                double sum = value1 + value2;

                // Calculate the length of the result string
                int length = snprintf(NULL, 0, "%.2f", sum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.2f", sum);
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
            } else if (imagSum == 0) {
                length = snprintf(NULL, 0, "%.0f", realSum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0f", realSum);
            } else {
                length = snprintf(NULL, 0, "%.0f%+.0fi", realSum, imagSum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0f%+.0fi", realSum, imagSum);
            }
        }
    }
}

// Function to subtract two matrices ------------------------------------------
void subMatrices(Matrix *matrix1, Matrix *matrix2, Matrix *result, const int kindOfNum) {
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

    if (kindOfNum == 1) {  // Integer values
        for (int i = 0; i < matrix1->rows; i++) {
            for (int j = 0; j < matrix1->cols; j++) {
                int value1 = atoi(matrix1->data[i][j]);
                int value2 = atoi(matrix2->data[i][j]);
                int diff = value1 - value2;
                int length = snprintf(NULL, 0, "%d", diff) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%d", diff);
            }
        }
    } else if (kindOfNum == 2) {  // Floating-point values
        for (int i = 0; i < matrix1->rows; i++) {
            for (int j = 0; j < matrix1->cols; j++) {
                double value1 = atof(matrix1->data[i][j]);
                double value2 = atof(matrix2->data[i][j]);
                double diff = value1 - value2;
                int length = snprintf(NULL, 0, "%.2f", diff) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.2f", diff);
            }
        }
    }
}

// Function to subtract two matrices with complex numbers -----------------
void subMatricesComplex(Matrix *matrix1, Matrix *matrix2, Matrix *result) {
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

            // Difference of real and imaginary parts
            double realDiff = real1 - real2;
            double imagDiff = imag1 - imag2;

            // Determine the appropriate output format
            int length;
            if (realDiff == 0 && imagDiff == 0) {
                length = snprintf(NULL, 0, "0") + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "0");
            } else if (realDiff == 0) {
                length = snprintf(NULL, 0, "%.0fi", imagDiff) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0fi", imagDiff);
            } else if (imagDiff == 0) {
                length = snprintf(NULL, 0, "%.0f", realDiff) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0f", realDiff);
            } else {
                if (imagDiff >= 0) {
                    length = snprintf(NULL, 0, "%.0f+%.0fi", realDiff, imagDiff) + 1;
                } else {
                    length = snprintf(NULL, 0, "%.0f%.0fi", realDiff, imagDiff) + 1;
                }
                result->data[i][j] = malloc(length);
                if (imagDiff >= 0) {
                    snprintf(result->data[i][j], length, "%.0f+%.0fi", realDiff, imagDiff);
                } else {
                    snprintf(result->data[i][j], length, "%.0f%.0fi", realDiff, imagDiff);
                }
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
                double value1 = atof(matrix1->data[i][k]);
                double value2 = atof(matrix2->data[k][j]);
                sum += value1 * value2;
            }

            int length;
            if (kindOfNum == 1) {  // Integer
                length = snprintf(NULL, 0, "%d", (int)sum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%d", (int)sum);
            } else {  // Floating point
                length = snprintf(NULL, 0, "%.2f", sum) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.2f", sum);
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
            } else if (cimag(sum) == 0) {
                length = snprintf(NULL, 0, "%.0f", creal(sum)) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0f", creal(sum));
            } else {
                length = snprintf(NULL, 0, "%.0f%+.0fi", creal(sum), cimag(sum)) + 1;
                result->data[i][j] = malloc(length);
                snprintf(result->data[i][j], length, "%.0f%+.0fi", creal(sum), cimag(sum));
            }
        }
    }
}

// Function to transpose a matrix ---------------------------------------------
void transposeMatrix(Matrix *matrix, Matrix *result) {
    result->rows = matrix->cols;
    result->cols = matrix->rows;

    // Allocate memory for the transposed matrix
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(char *));
        for (int j = 0; j < result->cols; j++) {
            // Allocate and copy the data from the original matrix to the transposed matrix
            result->data[i][j] = strdup(matrix->data[j][i]);
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

// Function to perform binary NOT on a matrix -------------------------------
void notBinaryMatrix(Matrix *matrix, Matrix *result) {
    result->rows = matrix->rows;
    result->cols = matrix->cols;

    // Allocate memory for result matrix
    result->data = malloc(result->rows * sizeof(char **));
    for (int i = 0; i < result->rows; i++) {
        result->data[i] = malloc(result->cols * sizeof(char *));
        for (int j = 0; j < result->cols; j++) {
            // Perform NOT operation
            char *value = matrix->data[i][j];
            int binaryValue = strcmp(value, "0") == 0 ? 1 : 0; // NOT operation
            result->data[i][j] = malloc(2 * sizeof(char)); // Allocate space for "0" or "1"
            sprintf(result->data[i][j], "%d", binaryValue);
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

int main() {
    // Get shared memory
    int shmid = shmget(SHM_KEY, SHM_SIZE, 0600);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach to shared memory
    SharedMemory *shm_ptr = (SharedMemory *)shmat(shmid, NULL, 0);
    if (shm_ptr == (SharedMemory *)-1) {
        perror("shmat");
        exit(1);
    }

    // Open semaphore
    sem_t *sem = sem_open("/sem", 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    char input[MAX_INPUT_LENGTH];
    Matrix matrix1, matrix2, result;
    char operation[MAX_INPUT_LENGTH];
    int firstMatrixRead = 0, secondMatrixRead = 0;
    int matrix_type = 0;
    int operation_completed = 0;
    int  error = 0;
    char *copy = NULL;
    int end = 0;


    //printf("Test program started, waiting for data...\n");

    while (1) {
        sem_wait(sem);  // Wait for semaphore (lock)

        if (shm_ptr->numStrings == 0) {
            sem_post(sem);  // Release semaphore (unlock)
//            printf("No data available, waiting...\n");
//            printf("First matrix read: %d\n", firstMatrixRead);
//            printf("Second matrix read: %d\n", secondMatrixRead);
            sleep(1);  // Wait a bit before trying again
            continue;
        }

        // Print strings from shared memory
        for (int i = 0; i < shm_ptr->numStrings; i++) {
            if (error == 1) {
                if (firstMatrixRead == 1) {
                    freeMatrix(&matrix1);
                }
                if (secondMatrixRead == 1) {
                    freeMatrix(&matrix2);
                }
                error = 0;
                firstMatrixRead = 0;
                secondMatrixRead = 0;
            }

            strcpy(input, shm_ptr->data[i]);
            // if input is larger than MAX_INPUT_LENGTH continue and clear the buffer
            if (strlen(input) == MAX_INPUT_LENGTH - 1 && input[strlen(input) - 1] != '\n') {
                printErrorMessage();
                while (fgetc(stdin) != '\n');
                continue;
            }
            if (input[0] == '\n') {
                continue;
            }
            input[strcspn(input, "\n")] = 0;  // Remove newline character
            copy = strdup(input);
            if (strncmp(input, "END", 3) == 0) {
                free(copy);
                if (firstMatrixRead == 1) {
                    freeMatrix(&matrix1);
                }
                if (secondMatrixRead == 1) {
                    freeMatrix(&matrix2);
                }
                end = 1;
                break;
            }

            if (firstMatrixRead == 0) {
                matrix_type = isMatrix(copy);
                free(copy);
                if (matrix_type != 0) {
                    readMatrix(input, &matrix1);
                    firstMatrixRead = 1;
                } else {
                    printErrorMessage();
                    error = 1;
                    continue;
                }
            } else {
                if (isMatrix(copy)) {
                    readMatrix(input, &matrix2);
                    secondMatrixRead = 1;

                    // Wait for the operation string to be available in shared memory
                    while (i + 1 >= shm_ptr->numStrings) {
                        sem_post(sem);  // Release semaphore (unlock)
//                        printf("No data available, waiting...\n");
//                        printf("First matrix read: %d\n", firstMatrixRead);
//                        printf("Second matrix read: %d\n", secondMatrixRead);
                        sleep(1);  // Wait a bit before trying again
                        sem_wait(sem);  // Wait for semaphore (lock)
                    }

                    // Read the operation from the shared memory
                    i++;
                    strcpy(input, shm_ptr->data[i]);
                    input[strcspn(input, "\n")] = 0;  // Remove newline character
                    sscanf(input, "%s", operation);
                    if (strncmp(input, "END", 3) == 0) {
                        free(copy);
                        if (firstMatrixRead == 1) {
                            freeMatrix(&matrix1);
                        }
                        if (secondMatrixRead == 1) {
                            freeMatrix(&matrix2);
                        }
                        break;
                    }
                    // Perform the operation
                    if (strcmp(operation, "ADD") == 0) {
                        if (matrix_type == 1 || matrix_type == 2) {
                            addMatrices(&matrix1, &matrix2, &result, matrix_type);
                            printMatrix(&result);
                            operation_completed = 1;
                        } else if (matrix_type == 3) {
                            addMatricesComplex(&matrix1, &matrix2, &result);
                            printMatrix(&result);
                            operation_completed = 1;
                        }
                    } else if (strcmp(operation, "SUB") == 0) {
                        if (matrix_type == 1 || matrix_type == 2) {
                            subMatrices(&matrix1, &matrix2, &result, matrix_type);
                            printMatrix(&result);
                            operation_completed = 1;
                        } else if (matrix_type == 3) {
                            subMatricesComplex(&matrix1, &matrix2, &result);
                            printMatrix(&result);
                            operation_completed = 1;
                        }
                    } else if (strcmp(operation, "MUL") == 0) {
                        if (matrix_type == 1 || matrix_type == 2) {
                            mulMatrices(&matrix1, &matrix2, &result, matrix_type);
                            printMatrix(&result);
                            operation_completed = 1;
                        } else if (matrix_type == 3) {
                            mulMatricesComplex(&matrix1, &matrix2, &result);
                            printMatrix(&result);
                            operation_completed = 1;
                        }
                    } else if (strcmp(operation, "AND") == 0) {
                        if (!isBinaryMatrix(&matrix1) || !isBinaryMatrix(&matrix2)) {
                            printErrorMessage();
                            free(copy);
                            error = 1;
                            continue;
                        }
                        andBinaryMatrix(&matrix1, &matrix2, &result);
                        printMatrix(&result);
                        operation_completed = 1;
                    } else if (strcmp(operation, "OR") == 0) {
                        if (!isBinaryMatrix(&matrix1) || !isBinaryMatrix(&matrix2)) {
                            printErrorMessage();
                            free(copy);
                            error = 1;
                            continue;
                        }
                        orBinaryMatrix(&matrix1, &matrix2, &result);
                        printMatrix(&result);
                        operation_completed = 1;
                    } else {
                        printErrorMessage();
                        error = 1;
                        continue;
                    }
                } else if (strcmp(input, "TRANSPOSE") == 0) {
                    if (secondMatrixRead == 1) {
                        printErrorMessage();
                        free(copy);
                        error = 1;
                        continue;
                    }
                    transposeMatrix(&matrix1, &result);
                    printMatrix(&result);
                    operation_completed = 1;
                } else if (strcmp(input, "NOT") == 0) {
                    if (!isBinaryMatrix(&matrix1) || secondMatrixRead == 1) {
                        printErrorMessage();
                        free(copy);
                        error = 1;
                        continue;
                    }
                    notBinaryMatrix(&matrix1, &result);
                    printMatrix(&result);
                    operation_completed = 1;
                } else {
                    printErrorMessage();
                    free(copy);
                    error = 1;
                    continue;
                }
            }

            if (operation_completed == 1) {
                if (firstMatrixRead == 1) {
                    freeMatrix(&matrix1);
                    freeMatrix(&result);
                    free(copy);
                    firstMatrixRead = 0;
                }
                if (secondMatrixRead == 1) {
                    freeMatrix(&matrix2);
                    secondMatrixRead = 0;
                }
                operation_completed = 0;
            }
        }

        if (end == 1) {
            break;
        }

        shm_ptr->numStrings = 0;  // Reset numStrings after reading
        sem_post(sem);  // Release semaphore (unlock)
        break;
    }
    //Clean the shared memory
    shmdt(shm_ptr);



    // Cleanup shared memory and semaphore
   // shmctl(shmid, IPC_RMID, NULL);
   sem_close(sem);
   //sem_unlink("/sem");

    //printf("Test program finished.\n");

    // Detach from shared memory
    //shmdt(shm_ptr);

    return 0;
}
