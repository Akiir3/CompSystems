/* 
 * tsh - A tiny shell program with job control
 * 
 * <Erika Bailon, erba0445>
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* Builtin types */
#define BLTN_UNK 0
#define BLTN_IGNR 1
#define BLTN_BGFG 2
#define BLTN_JOBS 3
#define BLTN_EXIT 4
#define BLTN_KILLALL 5

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);
int is_builtin_cmd(char **argv);
void do_exit(void);
void do_show_jobs(void);
void do_ignore_singleton(void);
void do_killall(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);
void sigalrm_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int removejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getprocessid(struct job_t *jobs, pid_t pid);
struct job_t *getjobid(struct job_t *jobs, int jid); 
int get_jid_from_pid(pid_t pid); 
void showjobs(struct job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/*
 * main - The shell's main routine 
 */
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
	    break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
	    break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
	    break;
	default:
            usage();
	}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */
    Signal(SIGALRM, sigalrm_handler);  /* Alarm indicates killing all children */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

    /* Execute the shell's read/eval loop */
    while (1) {

	/* Read command line */
	if (emit_prompt) {
	    printf("%s", prompt);
	    fflush(stdout);
	}
	if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
	    app_error("fgets error");
	if (feof(stdin)) { /* End of file (ctrl-d) */
	    fflush(stdout);
	    exit(0);
	}

	/* Evaluate the command line */
	eval(cmdline);
	fflush(stdout);
	fflush(stdout);
    } 

    exit(0); /* control never reaches here */
}
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (exit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char *cmdline) 
{
	char * argv[MAXARGS];	//the argument list - page 755
	int bg = parseline(cmdline, argv);  //should the job run fg or bg - PAGE 755

	pid_t pid; //obtaning processes Id's Page 739 and 755
	sigset_t mask; // tool that will help block 

	if (!is_builtin_cmd(argv)) //if it returns 0 the shell createdd a child process and executes the requested program
	{	//inside the child. If the user has asked to run it in the BG the shell returns to the tope of the loop
		//and waits for the next command line otherwise the shell uses waitpid to wait for the job to terminate PAGE 754
		
		//** NEED TO BLOCK - office hours and **PAGE 765
		//printf("blocking problem\n");
		sigemptyset(&mask); //Block sigs of type being handled - PAGE 775
       	sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL); //add the signals in set to black - PAGE 765 - bolck SIGINT and save prevoius blacked set

		//We need this because then the Unix system encounters an error they typycally return -1  - CHECK FOR ERROR - PAGE 737
		if((pid = fork()) < 0)	//Given this function, our call to fork reduces ffrom 4 lines to 2 lines PAGE 738
		{
            unix_error("forking error");
        }	
		else if (pid == 0) //this means that is the child ** CHILD runs user job - PAGE 755
		{	
			sigprocmask(SIG_UNBLOCK, &mask, NULL); //make sure sign is not blocked
           	setpgid(0, 0);
			if (execvp(argv[0], argv) <0)  //PAGE 753 and 755 - used execvp to not use environ - ASYNC-signalsafe-function PAGE 767
			{
				printf("%s: Command not found\n", argv[0]); // PAGE 755
				exit(0);	
			}
		}	

		
		else
		{		
			if(!bg)  //parent waits for foreground job to terminate - PAGE 755
			{
               addjob(jobs, pid, FG, cmdline); //add the job to the fg
            }
           else 
           {
               addjob(jobs, pid, BG, cmdline); //add the job to the bg
           }

           sigprocmask(SIG_UNBLOCK, &mask, NULL); //make sure sign is not blocked

           if (!bg)
           {
               //this waits for fg - office hours 
               waitfg(pid);
           } 
           else 
           {
               //need to print this for bg - PAGE 755
               printf("[%d] (%d) %s", get_jid_from_pid(pid), pid, cmdline); 
           } 
        }
		
	}
	    
	return ;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
	buf++;
	delim = strchr(buf, '\'');
    }
    else {
	delim = strchr(buf, ' ');
    }

    while (delim) {
	argv[argc++] = buf;
	*delim = '\0';
	buf = delim + 1;
	while (*buf && (*buf == ' ')) /* ignore spaces */
	       buf++;

	if (*buf == '\'') {
	    buf++;
	    delim = strchr(buf, '\'');
	}
	else {
	    delim = strchr(buf, ' ');
	}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
	return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
	argv[--argc] = NULL;
    }
    return bg;
}


/*
 * is_builtin_cmd - If the user has typed a built-in command then
 * return the type of built in command, otherwise indicate that it
 * isn't a built in command
 */
int is_builtin_cmd(char **argv)
{
		// found the basics on page 755
	if (strcmp( argv[0],"exit") == 0)
	{
		exit(0);
		return BLTN_EXIT;
	}
	else if (strcmp( argv[0],"fg") == 0 || strcmp( argv[0],"bg") == 0)
	{
		do_bgfg(argv);
		return BLTN_BGFG;
	}
	else if (strcmp( argv[0],"&") == 0) //Did this one after talking to Golnar because the pdf say to consider it
	{
		return 1; //*PAGE 755
	}
	else if (strcmp( argv[0],"jobs") == 0)
	{
		do_show_jobs();		
		return BLTN_JOBS;
	}
	else if (strcmp( argv[0],"killall") == 0)
	{		
		do_killall(argv);
		return BLTN_KILLALL;
	}
	
	return BLTN_UNK;     /* not a builtin command */
}

/*
 * do_exit - Execute the builtin exit command
 */
void do_exit(void)
{
	exit (0);  
	return;
}

/*
 * do_show_jobs - Execute the builtin jobs command
 */
void do_show_jobs(void)
{
	showjobs(jobs);  //easy pessyyyyyy
	return;
}

/*
 * do_ignore_singleton - Display the message to ignore a singleton '&'
 */
void do_ignore_singleton(void)
{
  return;
}

void do_killall(char **argv)
{
	int alertTime = atoi(argv[1]); //need to convert to int
	//printf("Is goo up to here KILLALL\n");
	alarm (alertTime); //PAGE 762 arranges for the Kernel to send a SIGALARM to the calling process in seconds 
	
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv)  //function of checking .. lots of checking
{
    struct job_t *job;
    int jid;
    pid_t pid; //obtaning processes ID's page 739
    
    if(argv[1] == NULL) //if user didn't give the pid|jobid in the command line
    {
        printf("%s command requires PID or Jjobid argument\n", argv[0]);
        return;
    }
    
    
    if(argv[1][0] == 'J') //if the user gave a J job
    {  
        jid = atoi(&argv[1][1]);
        job = getjobid(jobs, jid);

        if(job == NULL) //when the user gave job that is not running
        {  
            printf("%s: No such job\n", argv[1]);  
            return;  
        }
        else
        {
            pid = job->pid; //set the pid of that job to the pid so it can be kill
        }
    } 
  
    else if(isdigit(argv[1][0]))  //checking if is a pid
    { 
        pid = atoi(argv[1]); //converting the argument into an id to be passed 
        job = getprocessid(jobs, pid); //to get the job
        if(job == NULL)
        {  
            printf("(%d): No such process\n", pid);  
            return;  
        }  
    }  
    else 
    {
        printf("%s: argument must be a PID or Jjobid\n", argv[0]);
        return;
    }
  
    kill(-pid, SIGCONT); //Now we kill them!!!!
    
    if(strcmp("fg", argv[0]) == 0) 
    {
        job->state = FG; //changing where is running
        waitfg(job->pid);   //needs to wait for fg
    } 
    else
    {
        //print for bg jobs
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
        job->state = BG; //changing where is running
    }

		
	return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    struct job_t *fgjob = getprocessid(jobs, pid);
	if(fgjob == NULL) return;
	while (fgjob->pid == pid && fgjob->state ==FG)
	{
		sleep(1);
	}
	return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 */
void sigchld_handler(int sig) 
{
	int status = -1;  
    pid_t pid;
	//REAPING child processes PAGE 743 and PAGE 747
    
    //code from page 747 "Parent reaps n children in no particular order"
    while ((pid = waitpid(-1, &status, WUNTRACED|WNOHANG)) > 0) //objective - REAP all possible zombies
    {	//when pid is greater than 0, the wait set are determined by the pid argument - PAGE 744 
    	//HERE  - WNOHANG | WUNTRACED option
    	//returns with a return value of 0, if none of the children in the wait set has stopped or terminated, 
    	//or otherwise with a return value equal to the PID of one of the stopped or terminated children. 
    	//This is when you should start reaping - FROM GOLNAR   
		//printf("Is goo up to here CHILD\n");	

		//Before reaping the zombie child, you should check the exit status of the reaped child

		int jid = get_jid_from_pid(pid);
        if (WIFSTOPPED(status)) //WIFSTOPPED returns true if the child that cause the return is currently stopped
        { 
        	//here you should change job status to ST (stopped)
			printf("Job [%d] (%d) stopped by signal %d\n", jid, pid, WSTOPSIG(status));            
            getprocessid(jobs, pid)->state = ST;
        }  
        else if (WIFSIGNALED(status)) //WIFSIGNALED returns true if the child process terminated because of a signal that was not caught
		{
   			//you print a message and remove the job
			printf("Job [%d] (%d) terminated by signal %d\n", jid, pid, WTERMSIG(status));
        	removejob(jobs, pid);
        }  
        else if (WIFEXITED(status)) //WIFEXITED which I did not see in the above code. It returns true if the child 
        {	//terminated normally, and you should probably print a message and delete the job form the job list 
            //you should probably print a message and delete the job form the job list
            removejob(jobs, pid);  
        }  
    }  

  return;
}

/*
 * sigalrm_handler - The kernel sends a SIGALRM to the shell after
 * alarm(timeout) times out. Catch it and send a SIGINT to every
 * EXISTING (pid != 0) job
 */
void sigalrm_handler(int sig)  //PAGE 762
{
	//check if job is active 
	int i = 0;
	for (i; i < MAXJOBS; i++)
	{
		if (jobs[i].pid != 0)
		{
			//printf("Is goo up to here ALARM\n");
			//printf("Job[%i](%i) terminated by signal \n", jobs[i].jid, jobs[i].pid);			
			kill (jobs[i].pid, SIGINT);	//need to pass the signal
	       	clearjob (&jobs[i]);	//to clear the address of the jobs
		}

	}
	
	return;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
	pid_t tempSig= fgpid(jobs); //getting it

	if (tempSig!= 0)
	{
		//printf("Is goo up to here HANDLER\n");		
		//printf("Job [%d](%d) terminated by signal \n", j,t);
		
		kill(-tempSig, SIGINT);
	}   
	return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
    pid_t pid = fgpid(jobs);  //getting it
  
    if (pid != 0) 
    { 
        kill(-pid, sig);  
    }  
	return;
}

/*********************
 * End signal handlers
 *********************/

/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == 0) {
	    jobs[i].pid = pid;
	    jobs[i].state = state;
	    jobs[i].jid = nextjid++;
	    if (nextjid > MAXJOBS)
		nextjid = 1;
	    strcpy(jobs[i].cmdline, cmdline);
  	    if(verbose){
	        printf("Added job [%d] %d %s\n", jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
            }
            return 1;
	}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* removejob - Delete a job whose PID=pid from the job list */
int removejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid == pid) {
	    clearjob(&jobs[i]);
	    nextjid = maxjid(jobs)+1;
	    return 1;
	}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].state == FG)
	    return jobs[i].pid;
    return 0;
}

/* getprocessid  - Find a job (by PID) on the job list */
struct job_t *getprocessid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid)
	    return &jobs[i];
    return NULL;
}

/* getjobid  - Find a job (by JID) on the job list */
struct job_t *getjobid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* get_jid_from_pid - Map process ID to job ID */
int get_jid_from_pid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* showjobs - Print the job list */
void showjobs(struct job_t *jobs) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
	if (jobs[i].pid != 0) {
	    printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
	    switch (jobs[i].state) {
		case BG: 
		    printf("Running ");
		    break;
		case FG: 
		    printf("Foreground ");
		    break;
		case ST: 
		    printf("Stopped ");
		    break;
	    default:
		    printf("showjobs: Internal error: job[%d].state=%d ", 
			   i, jobs[i].state);
	    }
	    printf("%s", jobs[i].cmdline);
	}
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

/*
 * unix_error - unix-style error routine
 */
void unix_error(char *msg)
{
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

/*
 * app_error - application-style error routine
 */
void app_error(char *msg)
{
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

/*
 * Signal - wrapper for the sigaction function
 */
handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask); /* block sigs of type being handled */
    action.sa_flags = SA_RESTART; /* restart syscalls if possible */

    if (sigaction(signum, &action, &old_action) < 0)
	unix_error("Signal error");
    return (old_action.sa_handler);
}

/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}



