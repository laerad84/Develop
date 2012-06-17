#include   <cstdio>
#include   <cstdlib>
#include   <sys/types.h>
#include   <sys/ipc.h>
#include   <sys/sem.h>

int 
main()
{
  key_t  key;   /* key to pass to semget() */
  int  semflg;   /* semflg to pass to semget() */
  int  nsems;   /* nsems to pass to semget() */
  int  semid;   /* return value from semget() */
  
  fprintf(stderr,"All numeric input must follow C conventions:\n");
  fprintf(stderr,"\t0x... is interpreted as hexadecimal,\n");
  fprintf(stderr,"\t0... is interpreted as octal,\n");
  fprintf(stderr,"\totherwise, decimal.\n");
  fprintf(stderr,"IPC_PRIVATE == %x\n", IPC_PRIVATE);
  fprintf(stderr,"Enter key: ");
  scanf("%i", &key);  
  fprintf(stderr, "Enter nsems value: ");
  scanf("%i", &nsems);
  fprintf(stderr, "\nExpected flags for semflg are:\n");
  fprintf(stderr, "\tIPC_EXCL = \t%8.8o\n", IPC_EXCL);
  fprintf(stderr, "\tIPC_CREAT = \t%8.8o\n",IPC_CREAT);
  fprintf(stderr, "\towner read = \t%8.8o\n", 0400);
  fprintf(stderr, "\towner alter = \t%8.8o\n", 0200);
  fprintf(stderr, "\tgroup read = \t%8.8o\n", 040);
  fprintf(stderr, "\tgroup alter = \t%8.8o\n", 020);
  fprintf(stderr, "\tother read = \t%8.8o\n", 04);
  fprintf(stderr, "\tother alter = \t%8.8o\n", 02);
  fprintf(stderr, "Enter semflg value: ");
  scanf("%i", &semflg);
  fprintf(stderr, "\nsemget: Calling semget(%x, %d, %o)\n",key, nsems, semflg);
  if ((semid = semget(key, nsems, semflg)) == -1) {
    perror("semget: semget failed");
    exit(1);
  } else {
    fprintf(stderr, "semget: semget succeeded: semid =%d\n",semid);
    exit(0);
  }
}
