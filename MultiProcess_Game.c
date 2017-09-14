#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// Sample program to create a child and parent processes
static int ready = 0;

void SendReady(int sig)
{
    if (sig == SIGINT)
    {
        ready = 1;
    }
}


int read_from_pipe (int file, int player)
{
  FILE *stream;
  int c,ret;

  stream = fdopen (file, "r");
  //printf("Waiting %d\n",getpid() );
  while (!feof(stream)){
      fscanf (stream, "%d", &c);
      ret = c;
}
  fclose (stream);
  return ret;
}

void write_to_pipe (int file,int rannumb)
{
  FILE *stream;

  stream = fdopen (file, "w");
  rannumb = ((rand()%3)+1);
  //printf("Player A %d\n", rannumb );
  fprintf(stream, "%d", rannumb);
  fclose (stream);
}

void write_to_pipe_1 (int file,int rannumb_1)
{
  FILE *stream;
  stream = fdopen (file, "w");
  fprintf(stream, "%d", rannumb_1);
  fclose (stream);
}

int main (){
  pid_t p1,p2;
  int mypipe[2],pipe2[2];
  int i,count;

  int choice1,choice2;
  float playeratotal;
  float playerbtotal;

  signal(SIGINT, &SendReady);
  printf ("Enter the number of rounds (Min of 20)\n");
  scanf("%d",&count);
  for (i = 0;i <count;i++){

  pipe (mypipe);
  pipe (pipe2);

  p1 = fork();
  if(p1 == 0){ // This is the lazy child process.
    while (!ready);
    int rannumb;
    time_t t;
    srand((int)time(&t) % getpid());
    close (mypipe[0]);
    rannumb = ((rand()%3)+1);
    write(mypipe[1],&rannumb,100);
  }
  else if (p1 > 0){
    p2 = fork();
    if(p2 == 0){ // This is the active child process.
      while (!ready);
      close (pipe2[0]);
      int rannumb_1;
      rannumb_1 = ((getpid()%3)+1);
      write(pipe2[1],&rannumb_1,100);
    }
    else if(p2 > 0){ // This is the father process.

      //for (i = 0;i < 1;i++)
      //{
        raise(SIGINT);
        close (mypipe[1]);
        close (pipe2[1]);
        read(mypipe[0],&choice1,sizeof(choice1) - 1);
        read(pipe2[0],&choice2,sizeof(choice2) - 1) ;
        printf("Player A Choice - %d\n",choice1 );
        printf("Player B Choice - %d\n",choice2 );
        int tie = 0;

      // 1 - paper
      // 2 - scissor
      // 3 - rock
      /* Rock > scissor
      paper > rock
      scissor > paper
      */
        if(choice1 == 1 && choice2 == 2){
	       playerbtotal = playerbtotal + 1;
        }else if( choice1 == 1 && choice2 == 3){
          playerbtotal = playerbtotal + 1;
        }else if( choice1 == 1 && choice2 == 1){
          tie = tie + 1;
          playerbtotal = playerbtotal + 0.5;
          playeratotal = playeratotal + 0.5;
        }else if(choice1 == 2 && choice2 == 1){
          playeratotal = playeratotal + 1;
        }else if(choice1 == 2 && choice2 == 3){
          playerbtotal = playerbtotal + 1;
        }else if(choice1 == 2 & choice2 == 2){
          tie = tie + 1;
          playerbtotal = playerbtotal + 0.5;
          playeratotal = playeratotal + 0.5;
        }else if(choice1 == 3 && choice2 == 1){
          playerbtotal = playerbtotal + 1;
        }else if(choice1 == 3 && choice2 == 2){
          playeratotal = playeratotal + 1;
        }else{
          tie = tie + 1;
          playerbtotal = playerbtotal + 0.5;
          playeratotal = playeratotal + 0.5;
        }
        kill(p1,SIGKILL);
        kill(p2,SIGKILL);
        printf("Player A points - %.2f \n",playeratotal);
        printf("Player B points - %.2f \n",playerbtotal);
        if (playerbtotal >= 10 ){
          printf("Player B is winner\n" );
          exit(1);
        }
        if (playeratotal >= 10){
          printf("Player A is winnner\n" );
          exit(1);
        }

        ready = 0;
      //}
    }
    else{ // Fork doesnt work.
      printf("fork error\n");
      exit(1);
    }
  }
 else { // Fork doesnt work.
      printf("fork error\n");
      exit(1);
    }
}
if(p2 > 0 ){
  printf("point a %.2f \n",playeratotal);
  printf("point b %.2f \n",playerbtotal);
}
  return 0;
}
