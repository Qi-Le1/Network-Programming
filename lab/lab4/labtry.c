#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<string.h>

#define NUM_CHILD 5


struct toadd
{
  long a;
  long b;
};

void* add(void* ptr)
{
  struct toadd *to_add;
  to_add = (struct toadd *)ptr;
  printf("Thread %ld running add() with [%ld + %ld]\n", pthread_self(), to_add->a, to_add->b);

  pthread_t n = to_add->a + to_add->b;
  free(to_add);
  pthread_exit((void*)n);

}


int main()
{

  pthread_t children[NUM_CHILD*(NUM_CHILD - 1)];
  for(long i = 0; i < NUM_CHILD - 1; i++)
  {
    for(long j = 0; j < NUM_CHILD; j++)
    {
      struct toadd *to_add = malloc(sizeof(struct toadd));
      to_add->a = i + 1;
      to_add->b = j + 1;
      printf("Main starting thread add() for [%ld + %ld]\n", to_add->a, to_add->b);
      pthread_t tid;
      int val = pthread_create(&tid, NULL, add, (void *) to_add);
      if(val < 0)
      {
        return -1;
      }
      else
      {
        children[i*5+j] = tid;
      }
    }
  }

  for(long i = 0; i < NUM_CHILD - 1; i++)
  {
    for(long j = 0; j < NUM_CHILD; j++)
    {
    long* ret_val;
    pthread_join(children[i*5+j], (void**)&ret_val);
    printf("In main, collecting thread %ld computed [%ld + %ld] = %ld\n", children[i*5+j], i+1, j+1, (long)ret_val);
    }
  }
  return 0;
}
