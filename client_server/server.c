#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_THREAD 4

typedef struct
{
	long type;
	long return_id;
	int a;
	int b;
	int ans;
} bufer_t;

typedef struct
{
	long type;
	long return_id;
	int a;
	int b;
	int msq_id;
	int sem_id;
} thread_buf_t;


void sem_op(int sem_id, int op);

void* proc_req(void* buf);

int main()
{
	key_t key;
	bufer_t bufer;
	int msq_id;

	if ((key = ftok("key.txt", 0)) < 0) {
		printf("no key\n");
		exit(-1);
	}


	if ((msq_id = msgget(key, 0666 | IPC_CREAT)) < 0) {
		printf("no msq_id\n");
		exit(-1);
	}

	int sem_id;

	if ((sem_id = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
		printf("no sem_id\n");
		exit(-1);
	}
	semctl(sem_id, 0, IPC_RMID);
	if ((sem_id = semget(key, 1, 0666 | IPC_CREAT)) < 0) {
		printf("no sem_id\n");
		exit(-1);
	}

	int length;

	sem_op(sem_id, MAX_THREAD);
	while (1) {
		bufer.return_id = 0;
		if ((length = msgrcv(msq_id, (struct msgbuf *) &bufer, sizeof(bufer), 1, 0) < 0)) {
			printf("No received message\n");
			exit(-1);
		}
		if (bufer.return_id) {
			thread_buf_t* buf;
			buf = malloc(sizeof(thread_buf_t));
			memcpy(buf, &bufer, sizeof(bufer));
			buf->msq_id = msq_id;
			buf->sem_id = sem_id;
			pthread_t th;
			sem_op(sem_id, -1);
			pthread_create(&th, NULL, proc_req, buf);
		}

	}
	return 0;
}

void sem_op(int sem_id, int op) {
	struct sembuf sb;
	sb.sem_op = op;
	sb.sem_flg = 0;
	sb.sem_num = 0;
	semop(sem_id, &sb, 1);
}

void* proc_req(void* buf) {
	printf("Start %ld\n", (((thread_buf_t*)buf)->return_id));

	bufer_t bufer;
	bufer.ans = ((thread_buf_t*)buf)->a * ((thread_buf_t*)buf)->b;
	bufer.type = ((thread_buf_t*)buf)->return_id;
	bufer.return_id = 1;
	sleep(5);

	if (msgsnd(((thread_buf_t*)buf)->msq_id, (bufer_t*)&bufer, sizeof(bufer), 0) < 0) {
		printf("no message\n");
		msgctl(((thread_buf_t*)buf)->msq_id, IPC_RMID, NULL);
		exit(-1);
	}
	
	free(buf);
	printf("Finish %ld\n", ((thread_buf_t*)buf)->return_id);
	sem_op(((thread_buf_t*)buf)->sem_id, 1);

	return NULL;
}

