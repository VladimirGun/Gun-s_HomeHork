#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct
{
	long type;
	long return_id;
	int a;
	int b;
	int ans;
} bufer_t;

int main() {
	int msq_id;
	key_t key;
	int i;

	if ((key = ftok("key.txt", 0)) < 0) {
		printf("key error\n");
		exit(-1);
	}

	if ((msq_id = msgget(key, 0666 | IPC_CREAT)) < 0) {
		printf("msq_id error\n");
		exit(-1);
	}

	bufer_t bufer;
	bufer.type = 1;

	scanf("%d%d", &bufer.a, &bufer.b);
	bufer.return_id = getpid();
	int length = sizeof(bufer);

	if (msgsnd(msq_id, (struct msgbuf *) &bufer, length, 0) < 0) {
		printf("send error\n");
		msgctl(msq_id, IPC_RMID, NULL);
		exit(-1);
	}

	if ((length = msgrcv(msq_id, (struct msgbuf *) &bufer, sizeof(bufer), getpid(), 0) < 0)) {
		printf("recive error(\n");
		exit(-1);
	}
	
	printf("ans = %d\n", bufer.ans);
	return 0;
}
