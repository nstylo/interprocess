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
#include <errno.h>          // for perror()
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>

#include "common.h"
#include "md5s.h"

static void rsleep (int t);

/**
 * @param mq_name_req   the name of request (farmer -> worker) message queue to open
 * @param mq_name_res   the name of response (worker -> farmer) message queue to open
 */
static void read_mq(char mq_name_req[255], char mq_name_res[255]) {
    mqd_t               mq_req;
    mqd_t               mq_res;

    MQ_REQ_MSG          req;
    MQ_RES_MSG          res;

    mq_req = mq_open(mq_name_req, O_RDONLY);
    mq_res = mq_open(mq_name_res, O_WRONLY);

    // read the message queue and store it in the request message
    printf("child: receiving...\n");
    mq_receive(mq_req, (char*)&req, sizeof(req), NULL);

    printf("child: received: '%s', '%s'\n", req.password, req.finished ? "true" : "false");

    // TEMP
    mq_close(mq_req);
    mq_close(mq_res);

    mq_unlink(mq_name_req);
    mq_unlink(mq_name_res);
}

/*
 * rsleep(int t)
 *
 * The calling thread will be suspended for a random amount of time
 * between 0 and t microseconds
 * At the first call, the random generator is seeded with the current time
 */
static void rsleep (int t)
{
    static bool first_call = true;

    if (first_call == true)
    {
        srandom (time (NULL) % getpid ());
        first_call = false;
    }
    usleep (random() % t);
}

int main (int argc, char *argv[])
{
    printf("We are in the child process %d\n", getpid());
    printf("Given the following arguments: \n\n");

    // mq names
    char mq_name_req[255];      // NAME_MAX, see http://man7.org/linux/man-pages/man7/mq_overview.7.html
    char mq_name_res[255];

    // read passed on arguments
    for (int i = 0; i < argc; i++) {
        printf("%d. %s\n", i, argv[i]);

        // read and store mq names
        if (i == 1) {
            strcpy(mq_name_req, argv[i]);
        } else if (i == 2) {
            strcpy(mq_name_res, argv[i]);
        }
    }

    // open message queues, read plaintext password and compute hash until
    // worker disseminates jobs with raised finished flag
    read_mq(mq_name_req, mq_name_res);

    // TODO:
    // (see message_queue_test() in interprocess_basic.c)
    //  * repeatingly:
    //      - read from a message queue the new job to do
    //      - wait a random amount of time (e.g. rsleep(10000);)
    //      - do that job
    //      - write the results to a message queue
    //    until there are no more tasks to do
    //  * close the message queues

    return (0);
}

