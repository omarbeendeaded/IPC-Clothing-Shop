#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define SHM_NAME "/inventory"                                   // Name of the shared memory object
#define SEM_NAME "/inventorySem"				// Name of semaphore
#define NUM_ROWS 3                                              // Number of rows (must match the writer)
#define SIZE (NUM_ROWS * (sizeof(int) + sizeof(double) + 256))  // Same memory size

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
	int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
	if (shm_fd == -1) 
	{
		perror("shm_open");
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

	while (1)
	{
		sem_wait(sem);

		// Read and print data from shared memory arrays
		printf("\nCurrent Stock:\n");
		printf("No.\tCount\tPrice\tName\n");
		for (int i = 0; i < NUM_ROWS; i++) 
		{
			printf("%d\t%d\t%.2f\t%s\n", i, int_array[i], double_array[i], string_array[i]);
		}

		
		sem_post(sem);
		
		double p;
		int n, a;
		char c;
		
		printf("\nEnter R to restock or P to update prices: ");
		scanf(" %c", &c);

		// Restocking
		if (c == 'R' || c == 'r')
		{
			printf("Select item: ");
			scanf("%d", &n);

			if (n >= 0 && n <= NUM_ROWS - 1)
			{
				printf("Restock amount: ");
				scanf("%d", &a);
				fflush(stdin);

				sem_wait(sem);
				int_array[n] += a;
				printf("%s restocked!\n", string_array[n]);
				sem_post(sem);
			}
			else printf("Invalid item\n");
		}
		// Changing price
		else if (c == 'P' || c == 'p')
		{
			printf("Select item: ");
                        scanf("%d", &n);
			fflush(stdin);

			if (n >= 0 && n <= NUM_ROWS - 1)
			{
				printf("New Price: ");
				scanf("%lf", &p);

				sem_wait(sem);
				double_array[n] = p;
				printf("%s price changed!\n", string_array[n]);
				sem_post(sem);
			}
			else printf("Invalid item\n");
		}
		else printf("Invalid Option\n");

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

