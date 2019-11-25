/*
 * Operating Systems {2INCO} Practical Assignment
 * Interprocess Communication
 *
 * Niklas Stylianou (1284037)
 * STUDENT_NAME_2 (STUDENT_NR_2)
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

#define STUDENT_NAME "niklas"

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

/*
 * Initializes message farmer -> workers and workers -> farmer queues
 */
static void init_message_queues(void) {
    mqd_t               mq_req;         /* Message queue farmer -> worker */
    mqd_t               mq_res;         /* Message queue worker -> farmer */
    struct mq_attr      attr;           /* Message queue attributes */

    // Assign queue names
    sprintf(mq_name_req, "/mq_req_%s_%d", STUDENT_NAME, getpid());
    sprintf(mq_name_res, "/mq_res_%s_%d", STUDENT_NAME, getpid());

    // Init request queue
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 10; // TODO size of struct
    mq_req = mq_open(mq_name_req, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);

    // Init response queue
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 10; // TODO size of struct
    mq_res = mq_open(mq_name_res, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);

    // print to console
    getattr(mq_req);
    getattr(mq_res);
}

int main(int argc, char * argv[])
{
    if (argc != 1)
    {
        fprintf (stderr, "%s: invalid arguments\n", argv[0]);
    }

    init_message_queues();
    sleep(60);

    // TODO:
    //  * create the message queues (see message_queue_test() in interprocess_basic.c)
    //  * create the child processes (see process_test() and message_queue_test())
    //  * do the farming
    //  * wait until the chilren have been stopped (see process_test())
    //  * clean up the message queues (see message_queue_test())

    // Important notice: make sure that the names of the message queues contain your
    // student name and the process id (to ensure uniqueness during testing)

    return (0);
}
