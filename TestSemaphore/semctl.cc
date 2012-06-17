#include <cstdio>
#include <cstdlib>
#include <cstdlib>
#include <sys/types.h>
#include <sys/sem.h>
#include <ctime> 


struct semid_ds semid_ds;
static void do_semctl( int semid, int semnum, int  cmd, union semun arg);
static void do_stat();

char warning_message[]="If you remove read permission for yourself, this program will fail frequently!";

int
main(){
  
  union semun arg;
  int cmd;
  int i;
  int semid;
  int semnum;

  fprintf(stderr, "Enter semid value: ");
  scanf( "%i",&semid );

  fprintf(stderr, "Valid semctl cmd values are:\n");
  fprintf(stderr, "\tGETALL = %d\n", GETALL);
  fprintf(stderr, "\tGETNCNT = %d\n", GETNCNT);
  fprintf(stderr, "\tGETPID = %d\n", GETPID);
  fprintf(stderr, "\tGETVAL = %d\n", GETVAL);
  fprintf(stderr, "\tGETZCNT = %d\n", GETZCNT);
  fprintf(stderr, "\tIPC_RMID = %d\n", IPC_RMID);
  fprintf(stderr, "\tIPC_SET = %d\n", IPC_SET);
  fprintf(stderr, "\tIPC_STAT = %d\n", IPC_STAT);
  fprintf(stderr, "\tSETALL = %d\n", SETALL);
  fprintf(stderr, "\tSETVAL = %d\n", SETVAL);
  fprintf(stderr, "\nEnter cmd: ");
  scanf("%i", &cmd);
 
  switch (cmd) {
  case GETVAL:
  case SETVAL:
  case GETNCNT:
  case GETZCNT:
    /* Get the semaphore number for these commands. */
    fprintf(stderr, "\nEnter semnum value: ");
    scanf("%i", &semnum);
    break;
  case GETALL:
  case SETALL:
    /* Allocate a buffer for the semaphore values. */
    fprintf(stderr,"Get number of semaphores in the set.\n");
    arg.buf = &semid_ds;
    do_semctl(semid, 0, IPC_STAT, arg);
    if (arg.array =(ushort *)malloc((unsigned)(semid_ds.sem_nsems * sizeof(ushort)))) {
      /* Break out if you got what you needed. */
      break;
    }
    fprintf(stderr,"semctl: unable to allocate space for %d values\n",semid_ds.sem_nsems);
    return 2;
  }

  /* Get the rest of the arguments needed for the specified command. */
  switch (cmd) {
  case SETVAL:
    /* Set value of one semaphore. */
    fprintf(stderr, "\nEnter semaphore value: ");
    scanf("%i", &arg.val);
    do_semctl(semid, semnum, SETVAL, arg);
    /* Fall through to verify the result. */
    fprintf(stderr,"Do semctl GETVAL command to verify results.\n");
  case GETVAL:
    /* Get value of one semaphore. */
    arg.val = 0;
    do_semctl(semid, semnum, GETVAL, arg);
    break;
  case GETPID:
    /* Get PID of last process to successfully complete a
       semctl(SETVAL), semctl(SETALL), or semop() on the
       semaphore. */
    arg.val = 0;
    do_semctl(semid, 0, GETPID, arg);
    break;
  case GETNCNT:
    /* Get number of processes waiting for semaphore value toincrease. */
    arg.val = 0;
    do_semctl(semid, semnum, GETNCNT, arg);
    break;
  case GETZCNT:
    /* Get number of processes waiting for semaphore value to
       become zero. */
    arg.val = 0;
    do_semctl(semid, semnum, GETZCNT, arg);
    break;
  case SETALL:
    /* Set the values of all semaphores in the set. */
    fprintf(stderr,
	    "There are %d semaphores in the set.\n",
	    semid_ds.sem_nsems);
    fprintf(stderr, "Enter semaphore values:\n");
    for (i = 0; i < semid_ds.sem_nsems; i++) {
      fprintf(stderr, "Semaphore %d: ", i);
      scanf("%i", &(arg.array[i]));
    }
    do_semctl(semid, 0, SETALL, arg);
    /* Fall through to verify the results. */
    fprintf(stderr,
	    "Do semctl GETALL command to verify results.\n");
  case GETALL:
    /* Get and print the values of all semaphores in the
       set.*/
    do_semctl(semid, 0, GETALL, arg);
    fprintf(stderr,
	    "The values of the %d semaphores are:\n",
	    semid_ds.sem_nsems);
    for (i = 0; i < semid_ds.sem_nsems; i++)
      fprintf(stderr, "%d ", arg.array[i]);
    fprintf(stderr, "\n");
    break;
  case IPC_SET:
    /* Modify mode and/or ownership. */
    arg.buf = &semid_ds;
    do_semctl(semid, 0, IPC_STAT, arg);
    fprintf(stderr, "Status before IPC_SET:\n");
    do_stat();
    fprintf(stderr, "Enter sem_perm.uid value: ");
    scanf("%i", &semid_ds.sem_perm.uid);
    fprintf(stderr, "Enter sem_perm.gid value: ");
    scanf("%i", &semid_ds.sem_perm.gid);
    fprintf(stderr, "%s\n", warning_message);
    fprintf(stderr, "Enter sem_perm.mode value: ");
    scanf("%i", &semid_ds.sem_perm.mode);
    do_semctl(semid, 0, IPC_SET, arg);
    /* Fall through to verify changes. */
    fprintf(stderr, "Status after IPC_SET:\n");
  case IPC_STAT:
    /* Get and print current status. */
    arg.buf = &semid_ds;
    do_semctl(semid, 0, IPC_STAT, arg);
    do_stat();
    break;
  case IPC_RMID:
    /* Remove the semaphore set. */
    arg.val = 0;
    do_semctl(semid, 0, IPC_RMID, arg);
    break;
  default:
    /* Pass unknown command to semctl. */
    arg.val = 0;
    do_semctl(semid, 0, cmd, arg);
    break;
  }
  return 0;
}

static void
do_semctl( int semid, int semnum, int  cmd, union semun arg)
{
  register int      i;   /* work area */
  fprintf(stderr, "\nsemctl: Calling semctl(%d, %d, %d,", semid, semnum, cmd);
  switch (cmd) {
  case GETALL:
    fprintf(stderr, "arg.array = %x)\n",arg.array);
    break;
  case IPC_STAT:
  case IPC_SET:
    fprintf(stderr, "arg.buf = %x)\n", arg.buf);
    break;
  case SETALL:
    fprintf(stderr, "arg.array = [", arg.buf);
    for (i = 0;i < semid_ds.sem_nsems;) {
      fprintf(stderr, "%d", arg.array[i++]);
      if (i < semid_ds.sem_nsems)
       fprintf(stderr, ", ");
    }
    fprintf(stderr, "])\n");
    break;
  case SETVAL:
  default:
    fprintf(stderr, "arg.val = %d)\n", arg.val);
    break;
  }
  i = semctl(semid, semnum, cmd, arg);
  if (i == -1) {
    perror("semctl: semctl failed");
    return; 
  }
  fprintf(stderr, "semctl: semctl returned %d\n", i);
  return;
}

static void
do_stat()
{
  fprintf(stderr, "sem_perm.uid = %d\n",semid_ds.sem_perm.uid);
  fprintf(stderr, "sem_perm.gid = %d\n",semid_ds.sem_perm.gid);
  fprintf(stderr, "sem_perm.cuid = %d\n",semid_ds.sem_perm.cuid);
  fprintf(stderr, "sem_perm.cgid = %d\n",semid_ds.sem_perm.cgid);
  fprintf(stderr, "sem_perm.mode = %o, ",semid_ds.sem_perm.mode);
  fprintf(stderr, "access permissions = %o\n",semid_ds.sem_perm.mode & 0777);
  fprintf(stderr, "sem_nsems = %d\n",semid_ds.sem_nsems);
  fprintf(stderr, "sem_otime = %s", semid_ds.sem_otime ? ctime(&semid_ds.sem_otime) : "Not Set\n");
  fprintf(stderr, "sem_ctime = %s", ctime(&semid_ds.sem_ctime));
}

    
