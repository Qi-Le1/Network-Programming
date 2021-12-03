#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

int main(){
	int* x = malloc(sizeof(int));
	*x = 5;
	if(fork()==0){
		*x = 20;
		printf("child: %d\n",*x);
		free(x);
	}
	else{
		waitpid(-1,NULL,0);
		printf("parent: %d\n",*x);
	}
	free(x);
	return 0;
}
