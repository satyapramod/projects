#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


//define declarations
#define SIZE     100
int count = 0 ;

float playerbtotal = 0;
float playeratotal = 0;
pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;

//structs
typedef struct msgbuf {
         long    mtype;
         int    data[SIZE];
         } data_buf;


//function declarations
void *send_data();
void *send_data_1();
void *receive_data();

main()
{
     pthread_t thread1;
     pthread_t thread2;
     pthread_t thread3;
     int ret_val_t1;
     int ret_val_t2;
     int ret_val_t3;

     //create thread1
     printf("Enter the number of rounds (Min of 20)\n");
     scanf("%d",&count);
     ret_val_t1 = pthread_create( &thread1, NULL, send_data, NULL);
     if(ret_val_t1)
     {
         fprintf(stderr,"Error - pthread_create() return value: %d\n",ret_val_t1);
         exit(EXIT_FAILURE);
     }

     //create thread2
     ret_val_t2 = pthread_create( &thread2, NULL, receive_data, NULL);
     if(ret_val_t2)
     {
         fprintf(stderr,"Error - pthread_create() return value: %d\n",ret_val_t2);
         exit(EXIT_FAILURE);
     }

     ret_val_t3 = pthread_create( &thread3, NULL, send_data_1, NULL);
     if(ret_val_t3)
     {
         fprintf(stderr,"Error - pthread_create() return value: %d\n",ret_val_t2);
         exit(EXIT_FAILURE);
     }

    //  printf("pthread_create() for thread 1 returns: %d\n",ret_val_t1);
    //  printf("pthread_create() for thread 2 returns: %d\n",ret_val_t2);
    //  printf("pthread_create() for thread 3 returns: %d\n",ret_val_t3);

     //wait untill threads are done with their routines before continuing with main thread
     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL);
     pthread_join( thread3, NULL);

     exit(EXIT_SUCCESS);
}

void *send_data(){

    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    data_buf sbuf;
    size_t buf_length;
    int choice1;

    //get the message queue id for the key with value 1234
    key = 1234;
    //(void) fprintf(stderr, "\nmsgget: Calling msgget(%#lx,\%#o)\n", key, msgflg);

    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        exit(1);
    }
    //send message type 1
    sbuf.mtype = 1;

    //(void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);

    //fill the array that is to be sent from thread1 to thread2
    int i = 0;
    srand(pthread_self());
    for(i = 0; i < count; i++){
      sbuf.data[i] =((rand()) % 3) + 1;
      //printf("Sending P1 - sbuf.data[%d] %d\n",i,sbuf.data[i]);
    }

    //(void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);

    buf_length = sizeof(data_buf);

    //send data from thread1 to thread
    if (msgsnd(msqid, &sbuf, buf_length, IPC_NOWAIT) < 0) {
        perror("msgsnd");
        exit(1);
    }

}

void *send_data_1(){

    int msqid;
    int msgflg = IPC_CREAT | 0666;
    key_t key;
    data_buf sbuf_1;
    size_t buf_length;

    //get the message queue id for the key with value 1234
    key = 4567;

    //(void) fprintf(stderr, "\nmsgget: Calling msgget(%#lx,\%#o)\n", key, msgflg);

    if ((msqid = msgget(key, msgflg )) < 0) {
        perror("msgget");
        exit(1);
    }
    else{
    //(void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);
    }

    //send message type 1
    sbuf_1.mtype = 1;

    //(void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);

    //fill the array that is to be sent from thread1 to thread2
    int i = 0;
    srand(pthread_self());
    for(i = 0; i < count; i++){
      sbuf_1.data[i] = (rand() % 3) + 1;
      //printf("Sending P2 - sbuf.data[i] %d %d\n",i,sbuf_1.data[i]);
    }

    //(void) fprintf(stderr,"msgget: msgget succeeded: msqid = %d\n", msqid);

    buf_length = sizeof(data_buf);

    //send data from thread1 to thread2

    if (msgsnd(msqid, &sbuf_1, buf_length, IPC_NOWAIT) < 0) {
        perror("msgsnd");
        exit(1);
    }
}

void *receive_data(){
    int msqid;
    key_t key;
    data_buf  rbuf_1,rbuf_2;

    //get the message queue id for the key with value 1234
    key = 1234;


    if ((msqid = msgget(key, 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

    //receive the answer with message type 1
    int temp;
    temp = msgrcv(msqid, &rbuf_1, sizeof(data_buf), 1, 0);
    if(temp == ENOMSG){
        usleep (100000);
        //continue;
    }
    if (temp < 0 ){
        perror("msgrcv");
        exit(1);
    }


    //print received answer
    int j = 0;
    for(j = 0; j < count; j++){
      //printf("Player 1 %d\n",rbuf_1.data[j]);
    }

    key = 4567;

    if ((msqid = msgget(key, 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

    temp = msgrcv(msqid, &rbuf_2, sizeof(data_buf), 1, 0);
    if (temp == ENOMSG) {
      usleep (100000);
    }
    if( temp < 0 ){
      perror("msgrcv");
      exit(1);
    }

    for(j = 0; j < count; j++){
      //printf("Player 2 %d\n",rbuf_2.data[j]);
    }

    for(j = 0; j < count; j++){
      printf("Player 1 Choice  %d\n",rbuf_1.data[j]);
      printf("Player 2 Choice  %d\n",rbuf_2.data[j]);
      if (rbuf_2.data[j] == 1 && rbuf_1.data[j] == 2){
        playeratotal = playeratotal + 1;
      }
      else if (rbuf_2.data[j] == 1 && rbuf_1.data[j] == 3){
        playeratotal = playeratotal + 1;
      }
      else if (rbuf_2.data[j] == 1 && rbuf_1.data[j] == 1){
        playeratotal = playeratotal + 0.5;
        playerbtotal = playerbtotal + 0.5;
      }
      else if (rbuf_2.data[j] == 2 && rbuf_1.data[j] == 1){
        playeratotal = playeratotal + 1;
      }
      else if (rbuf_2.data[j] == 2 && rbuf_1.data[j] == 3){
        playerbtotal = playerbtotal + 1;
      }
      else if (rbuf_2.data[j] == 2 && rbuf_1.data[j] == 2){
        playeratotal = playeratotal + 0.5;
        playerbtotal = playerbtotal + 0.5;
      }
      else if (rbuf_2.data[j] == 3 && rbuf_1.data[j] == 1){
        playerbtotal = playerbtotal + 1;
      }
      else if (rbuf_2.data[j] == 3 && rbuf_1.data[j] == 2){
        playeratotal = playeratotal + 1;
      }
      else
      {
        playeratotal = playeratotal + 0.5;
        playerbtotal = playerbtotal + 0.5;
      }
      printf("playeratotal %.2f\n",playeratotal );
      printf("playerbtotal %.2f\n\n",playerbtotal );

      if (playeratotal >= 10.0)
      {
        printf("Player 1 wins\n\n");
        exit(1);
      }
      if (playerbtotal >= 10.0)
      {
        printf("Player 2 wins\n");
        exit(1);
      }
    }
}
