/*
 * Operating Systems {2INCO} Practical Assignment
 * Interprocess Communication
 *
 * Niklas Stylianou (1284037)
 * Maurice Wimmer (1250175)
 *
 * Grading:
 * Students who hand in clean code that fully satisfies the minimum requirements will get an 8.
 * Extra steps can lead to higher marks because we want students to take the initiative.
 * Extra steps can be, for example, in the form of measurements added to your code, a formal
 * analysis of deadlock freeness etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>         // for execlp
#include <mqueue.h>         // for mq

#include "settings.h"
#include "common.h"

#define STUDENT_NAME "Niklas&Maurice"

static char mq_name_req[80];
static char mq_name_res[80];

/*
 * reads and prints the attributes of a message queue to the console
 */
static void getattr(mqd_t mq)
{
    struct mq_attr      attr;
    int                 rtnval;

    rtnval = mq_getattr(mq, &attr);
    if (rtnval == -1)
    {
        perror ("mq_getattr() failed");
        exit (1);
    }
    fprintf (stderr, "%d: mqdes=%d max=%ld size=%ld nrof=%ld\n",
                getpid(),
                mq, attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);
}

/**
 * Initializes message farmer -> workers and workers -> farmer queues
 */
static void init_message_queues(mqd_t *mq_req, mqd_t *mq_res) {
    struct mq_attr      attr;           /* Message queue attributes */

    // Assign queue names
    sprintf(mq_name_req, "/mq_req_%s_%d", STUDENT_NAME, getpid());
    sprintf(mq_name_res, "/mq_res_%s_%d", STUDENT_NAME, getpid());

    // Init request queue
    attr.mq_maxmsg = MQ_MAX_MESSAGES;
    attr.mq_msgsize = sizeof(MQ_REQ_MSG);
    *mq_req = mq_open(mq_name_req, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

    // Init response queue
    attr.mq_msgsize = sizeof(MQ_RES_MSG);
    *mq_res = mq_open(mq_name_res, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);


    // print to console
    getattr(*mq_req);
    getattr(*mq_res);
}



/**
 *  @param pid_t ID of the child.
 * Creates a single worker as child process
 */
static void create_worker(pid_t *processID) {

    printf ("parent pid:%d\n", getpid());
    *processID = fork();
    if (*processID < 0)
    {
        perror("fork() failed");
        exit (1);
    }
    else
    {
        if (*processID == 0)
        {
            printf ("child  pid:%d\n", getpid());
            execlp ("./worker", "./worker", mq_name_req, mq_name_res, NULL);  // pass on queue names to worker

            // we should never arrive here...
            perror ("execlp() failed");
        }
        // else: we are still the parent (which continues this program)

    }
}

static void create_workers(pid_t workers_processID[]){
    for (int i = 0; i < NROF_WORKERS; i++) {
        create_worker(&workers_processID[i]);
        printf("we have created %d\n", workers_processID[i]);
    }
}

static void close_workers(pid_t workers_processID[], mqd_t mq_req){
    MQ_REQ_MSG req;
    req.quit_flg = true;
    for (int i = 0; i < NROF_WORKERS; i++) {
        req.quit_flg = true;
        mq_send(mq_req, (char *) &req, sizeof (req), 0);
        printf("quit messages sent\n");
    }
}

static void wait_workers(pid_t workers_processID[]) {
    for (int i = 0; i < NROF_WORKERS; i++) {
        waitpid (workers_processID[i], NULL, 0);   // wait for the child
        printf ("child %d has been finished\n\n", workers_processID[i]);
    }
}


int main(int argc, char *argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }

    mqd_t mq_req;         /* Message queue farmer -> worker */
    mqd_t mq_res;         /* Message queue worker -> farmer */
    pid_t workers_processID[NROF_WORKERS]; /* Process ID from fork() */


    init_message_queues(&mq_req, &mq_res);
    create_workers(workers_processID);


    // TODO: this is only for testing purposes
    MQ_REQ_MSG req;
    req.hash = UINT128(0xcfcbe5bdf31116aa,0x3dcffb7e7470333e); // just a test message
    req.first_letter = 'b';
    req.quit_flg = false;
    req.alphabet_size = ALPHABET_NROF_CHAR; // TODO maybe this one could be better passed as argument?

    mq_send(mq_req, (char *) &req, sizeof (req), 0);
    // see what came in. (for testing)
    MQ_RES_MSG res;
    mq_receive(mq_res, (char*)  &res, sizeof(MQ_RES_MSG), NULL);
    printf("solution received: %s %s \n", res.password, res.finished ? "true" : " false");


    sleep(5);
    //shut down the workers.
    close_workers(workers_processID, mq_req);
    wait_workers(workers_processID);

    getattr(mq_req);
    sleep(1);

    // TODO:
    //  * do the farming
    //  * wait until the chilren have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)

    return (0);
}
