#include <stdio.h>
#include <stdlib.h>
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
	if (semid == -1)
	{
		perror ("error with semget\n");
		return -1;
	}
  /* Пытаемся эксклюзивно создать разделяемую память для выбранного ключа, права доступа 0666 - чтение и запись разрешены для всех */
	int shmid = shmget (SHARED_MEMORY_KEY, 20, IPC_CREAT | 0666);
	if (shmid == -1)
	{
		perror ("error with shmget\n");
		return -1;
	}
	/* Пытаемся отобразить разделяемую память в адресное пространство текущего процесса.*/

	void* shMemPtr = shmat (shmid, NULL, 0);
	if (shMemPtr == (void*)-1)
	{
		perror ("error with shmat\n");
		return -1;
	}

	struct sembuf semaphore;
/*Cоздаем особый FIFO-файл с правами доступа  Read, Write, Execute (чтение, запись и выполнение)*/
	int mkFifoRet = mkfifo ("communicatingFifo", 0777);
	if (mkFifoRet == -1)
	{
		if (errno == EEXIST) /* Если pathname уже существует*/
		{
			printf ("fifo exists\n");
			errno = 0;
		}
		else
		{
			perror ("Error with mkFifo\n");
			return -1;
		}
	}

	sprintf ((char*) shMemPtr, "communicatingFifo");


	semaphore.sem_num = 0;/* Номер семафора */
    semaphore.sem_op  = 1;/* Операция над семафором */
    semaphore.sem_flg = 0;/* Флаги операции */

    if (semop(semId, &semaphore, 1) == -1)
    {
        printf ("line  = %d\n", __LINE__);
        perror ("Error with semop\n");
        return -1;
    }

	int fdWrite = open ("communicatingFifo", O_WRONLY);
	if (fdWrite == -1)
	{
		perror ("Error with open\n");
		return -1;
	}


	int writeRet = write (fdWrite, "Hello world =)\n", 16);
	if (writeRet == -1)
	{
		perror ("Error with write\n");
        return -1;
	}

	return 0;

}
