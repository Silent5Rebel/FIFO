#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define SHARED_MEMORY_KEY 1837
#define SIZE_OF_SHARED_BUFFER 128
#define SEMAFORES_KEY 1818

int main ()
{
	/* идентификатор множества семафоров, предварительно полученный при помощи системного вызова semget, права доступа Read, Write (чтение и запись)*/
	int semid = semget (SEMAFORES_KEY, 1, IPC_CREAT | 0666);
	//PRINT ("semId = %d\n", semId)
	if (semid == -1)
	{
		perror ("error with semget\n");
		return -1;
	}
/* Пытаемся эксклюзивно создать разделяемую память для выбранного ключа, права доступа 0666 - чтение и запись разрешены для всех */
	int shmid = shmget (SHARED_MEMORY_KEY, 20, IPC_CREAT | 0666);
	//PRINT ("shmId = %d\n", shmId)
	if (shmid == -1)
	{
		perror ("error with shmget\n");
		return -1;
	}

	void* shMemPtr = shmat (shmId, NULL, 0);
	//PRINT ("shMemPtr = %p\n", shMemPtr)
	if (shMemPtr == (void*)-1)
	{
		perror ("error with shmat\n");
		return -1;
	}

	struct sembuf semaphore;
/*Заполним нашу структуру. Флаг, как обычно, полагаем равным 0.
	Наш массив семафоров состоит из одного семафора с номером 0. Код операции -1.*/
	semaphore.sem_num = 0;/* Номер семафора */
    semaphore.sem_op  = -1;/* Операция над семафором */
    semaphore.sem_flg = 0;/* Флаги операции */

    if (semop(semid, &semaphore, 1) == -1)
    {
        printf ("line  = %d\n", __LINE__);
        perror ("Error with semop\n");
        return -1;
    }

	char* fifoName = (char*) shMemPtr;

	int fdRead = open (fifoName, O_RDONLY);
	if (fdRead == -1)
	{
		perror ("Error with open\n");
		return -1;
	}

	char buffer [16] = {};

	int readRet = read (fdRead, buffer, 16);
	if (readRet == -1)
	{
		perror ("Error with read\n");
		return -1;
	}

	int writeRet = write (STDOUT_FILENO, buffer, readRet);
	if (readRet == -1)
	{
		perror ("Error with write (reader)\n");
		return -1;
	}

	return 0;




}
