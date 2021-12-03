#include "../../unpv13e/lib/unp.h"
#include <math.h>

void child();
void parent();
void sig_child(int signo);

int dead_children = 0;
int pid_array[100];
int loc1 = 0;
int loc2 = 0;

int main(int argc, char* argv[]) {
	printf("Please input the number of children.\n");
	int number = 0;
	scanf("%d", &number);
	pid_t child_pid;
	printf("Number of children to spawn: %d\n", number);

	Signal(SIGCHLD, sig_child);

	for (int i = 0; i < number; i++) {
		if ((child_pid = fork()) > 0) {
			// parent
			srand(time(NULL) + pow(i,10)*rand());
			parent(child_pid);
		}
		else {
			// child
			child();
		}
	}
	while (dead_children < number) {
		sleep(10);
		printf("Parent sees child PID %d has terminated.\n", pid_array[loc2]);
		loc2++;
	}
	return 0;
}

void child() {
	int time_ = 0;
	time_ = rand() % 6;
	printf("Child PID %d dying in %d seconds.\n", getpid(), time_);
	sleep(time_);
	printf("Child PID %d terminating.\n", getpid());
	kill(getpid(), SIGKILL);
}

void parent(int child_pid) {
	printf("Parent spawned child PID %d.\n", child_pid);

}

void sig_child(int signo) {
	pid_t pid;
	int stat;
	while((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
		dead_children++;
		pid_array[loc1] = pid;
		loc1++;
	}
	return;
}









