#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#define SHM_KEY 1234
#define MAX_INPUT_LENGTH 128
#define SHM_SIZE 2048

typedef struct {
    int numStrings;
    char data[1][MAX_INPUT_LENGTH  ];  // Flexible array member for string data
} SharedMemory;

int main() {
    int shmid = shmget(SHM_KEY, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        exit(1);
    }

    SharedMemory *shm_ptr = (SharedMemory *)shmat(shmid, NULL, 0);
    if (shm_ptr == (SharedMemory *)-1) {
        perror("shmat");
        exit(1);
    }

    sem_unlink("/sem");
    sem_t *sem = sem_open("/sem", O_CREAT, 0600, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    shm_ptr->numStrings = 0;

    char input[MAX_INPUT_LENGTH];
  //  int currentOffset = 0;

    //printf("Enter strings (empty string to finish):\n");
    while (1) {
        //printf("String %d: ", shm_ptr->numStrings + 1);
        fgets(input, MAX_INPUT_LENGTH, stdin);
        input[strcspn(input, "\n")] = '\0';  // Remove newline character

        // Calculate the new offset and check memory limit
      //  currentOffset = sizeof(SharedMemory) + (shm_ptr->numStrings + 1) * MAX_INPUT_LENGTH;
//        if (currentOffset > SHM_SIZE) {
//            printf("ERR\n");
//            break;
//        }

        sem_wait(sem);  // Wait for semaphore (lock)
        strcpy(shm_ptr->data[shm_ptr->numStrings], input);  // Store string
        shm_ptr->numStrings++;
        sem_post(sem);  // Release semaphore (unlock)

        // Check for "END" signal
        if (strncmp(input, "END", 3) == 0) {
            break;
        }
    }

    // Cleanup shared memory and semaphore
    // shmctl(shmid, IPC_RMID, NULL);
    //sem_close(sem);
    //sem_unlink("/sem");

    //printf("Initialized shared memory with strings.\n");

    // Detach from shared memory
    shmdt(shm_ptr);


    return 0;
}
