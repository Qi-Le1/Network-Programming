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
  if(to_add->b == 1)
    return(void*)to_add->a;
  if(to_add->b != 1)
  {
    to_add->b--;
    return (void*)(1+add((void*)to_add));
  }
}


int main()
{
  struct toadd to_add_ = {0, 0};
  struct toadd *to_add = &to_add_;
  pthread_t *children;
  children = (pthread_t *)malloc(sizeof(pthread_t)*(NUM_CHILD*(NUM_CHILD - 1)));
  for(long i = 0; i < NUM_CHILD - 1; i++)
  {
    for(long j = 0; j < NUM_CHILD; j++)
    {
      to_add->a = i + 1;
      to_add->b = j + 1;
      printf("Main starting thread add() for [%ld + %ld]\n", to_add->a, to_add->b);
      if(to_add->a == 1 && to_add->b == 1)
        continue;
      if(to_add->b == 1)
      {
        to_add->b = NUM_CHILD; to_add->a--;
      }
      else
      {
        to_add->b--;
      }
      pthread_t tid;
      int val = pthread_create(&tid, NULL, add, to_add);
      printf("Thread %ld running add() with [%ld + %ld]\n", tid, to_add->a, to_add->b);
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
  to_add->b++;
  pthread_t tid;
  int val = pthread_create(&tid, NULL, add, to_add);
  printf("Thread %ld running add() with [%ld + %ld]\n", tid, to_add->a, to_add->b);
  if(val < 0)
  {
    return -1;
  }
  else
  {
    children[NUM_CHILD*(NUM_CHILD - 1)-1] = tid;
  }

  for(long i = 0; i < NUM_CHILD - 1; i++)
  {
    for(long j = 0; j < NUM_CHILD; j++)
    {
    long* ret_val;
    pthread_join(children[i*5+j], (void**)&ret_val);
    printf("In main, collecting thread %ld computed [%ld + %ld] = %ld\n", children[i*5+j], i+1, j+1, (long)ret_val);
    free(ret_val);
    }
  }
  return 0;
}
