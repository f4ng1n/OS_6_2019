#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

using namespace std;

FILE *fn;
sem_t *sem;
typedef struct someArgs_tag {
    bool flag1;
} someArgs_t;

void* func1(void *args) {
    someArgs_t *arg = (someArgs_t*) args;
while(arg->flag1){
        sleep(1);
        char a = '1';
        sem_wait(sem);
        for (int i=1;i<=5;i++)
       {
        cout<<"1"<<flush;
        fputs("1",fn);
        sleep(1);
        }
 
 sem_post(sem);
  }
}

int main() {
    pthread_t id1;
    fn = fopen("./file", "a");
    sem = sem_open("/mysem",O_CREAT,0644, 1);

    someArgs_t args;
  args.flag1=true;

   pthread_create(&id1, NULL, func1,(void*)&args);
getchar();
  args.flag1 = 0;
  pthread_join(id1, NULL);
  sem_close(sem);
  sem_unlink("/mysem");
  fclose(fn);
    return 0;
}

