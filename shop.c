#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define SHM_NAME "/inventory"                                  // Name of the shared memory object
#define SEM_NAME "/inventorySem"                               // Name of semaphore
#define NUM_ROWS 3                                             // Number of rows (can be adjusted based on your CSV size)
#define SIZE (NUM_ROWS * (sizeof(int) + sizeof(double) + 256)) // Memory required for 3 arrays (int, double, string)

int main(void) 
{
	// Create semaphore
	sem_t *sem = sem_open(SEM_NAME,O_CREAT, 0666, 1);
	if (sem == SEM_FAILED)
	{
		perror("sem_open");
		return 1;
	}

	// Open shared memory object
	int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
	if (shm_fd == -1) 
	{
		perror("shm_open");
		return 1;
	}

	// Resize the shared memory object
	if (ftruncate(shm_fd, SIZE) == -1) 
	{
		perror("ftruncate");
		return 1;
	}

	// Map the shared memory object into the process's address space
	void *shm_ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
	if (shm_ptr == MAP_FAILED) 
	{
		perror("mmap");
		return 1;
	}

	// Prepare pointers to the three arrays (int, double, string)
	int *int_array = (int *)shm_ptr;
	double *double_array = (double *)(int_array + NUM_ROWS);
	char (*string_array)[256] = (char *)(double_array + NUM_ROWS);

	// Read data from CSV file and store in arrays
	FILE *file = fopen("init.csv", "r");
	if (file == NULL) 
	{
		perror("fopen");
		return 1;
	}

	char line[512];
	int i = 0;
	while (fgets(line, sizeof(line), file) != NULL && i < NUM_ROWS) 
	{
		int int_value;
		double double_value;
		char string_value[256];

		// Parse each line (assuming format: int, double, string)
		if (sscanf(line, "%d,%lf,%255s", &int_value, &double_value, string_value) == 3) 
		{
		    int_array[i] = int_value;
		    double_array[i] = double_value;
		    strncpy(string_array[i], string_value, 255);  // Copy string into the array
		    i++;
		}
	}

	fclose(file);

	while (1)
	{
		sem_wait(sem);

		// Print Inventory
		printf("\nCurrent Stock:\n");
		printf("Count\tPrice\tItem\n");
		for (int i = 0; i < NUM_ROWS; i++) 
		{
			printf("%d\t%.2f\t%s\n", int_array[i], double_array[i], string_array[i]);
		}

		sem_post(sem);

		sleep(3);
	}



	// Cleanup
	if (munmap(shm_ptr, SIZE) == -1) 
	{
		perror("munmap");
		return 1;
	}

	if (close(shm_fd) == -1) 
	{
		perror("close");
		return 1;
	}

	return 0;
}

