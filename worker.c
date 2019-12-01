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
#include <errno.h>          // for perror()
#include <unistd.h>         // for getpid()
#include <mqueue.h>         // for mq-stuff
#include <time.h>           // for time()
#include <complex.h>

#include "uint128.h"

#include "common.h"
#include "md5s.h"




static void rsleep (int t);

mqd_t               mq_req;
mqd_t               mq_res;

/**
 * @param mq_name_req   the name of request (farmer -> worker) message queue to open
 * @param mq_name_res   the name of response (worker -> farmer) message queue to open
 */
static void init_mq(char mq_name_req[255], char mq_name_res[255]) {
    mq_req = mq_open(mq_name_req, O_RDONLY);
    mq_res = mq_open(mq_name_res, O_WRONLY);
}

/**
 * @param mq_name_req   the name of request (farmer -> worker) message queue to open
 * @param mq_name_res   the name of response (worker -> farmer) message queue to open
 */
static void close_mq(char mq_name_req[255], char mq_name_res[255]) {
    mq_close(mq_req);
    mq_close(mq_res);

    mq_unlink(mq_name_req);
    mq_unlink(mq_name_res);
}

static void get_message(MQ_REQ_MSG *req) {
    mq_receive(mq_req, (char*)  req, sizeof(MQ_REQ_MSG), NULL);
}

static void send_message(MQ_RES_MSG res) {
    mq_send(mq_res, (char *) &res, sizeof (res), 0);
}

static bool tryHash( char password[], uint128_t hash) {
    uint128_t pre_hash;
    pre_hash = md5s (password, strlen (password));

    if (pre_hash == hash ) {
        return true;
    }

    return false;
}

static bool solve(char firstLetter, char str[], uint128_t hash, int alphabet_size) {
    if (strlen(str) == 0) {
        strncat(str, &firstLetter, 1);
        if (tryHash(str, hash)) {
            return true;
        }
    }
    if(strlen(str) < MAX_MESSAGE_LENGTH){
        for (int j = 0; j < alphabet_size ; j++) {
            char new = j+97;
            strncat(str, &new, 1);
            if (tryHash(str, hash)) {
                return true;
            }
            if (solve(firstLetter, str, hash, alphabet_size)) {
                return true;
            }
            str[strlen(str) - 1] = '\0';
        }
    }
    return false;
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

    // open messaging queues
    init_mq(mq_name_req, mq_name_res);

    // get message from queue
    MQ_REQ_MSG req;
    get_message(&req);
    printf ("hash: 0x%016lx%016lx with letter %c and size %d \n", HI(req.hash), LO(req.hash), req.first_letter,
            req.alphabet_size);

    //solve bruteforce.
    MQ_RES_MSG res;
    strcpy(res.password, ""); //set to empty string to start recursion.
     if (solve(req.first_letter, res.password, req.hash, req.alphabet_size)) {  //TODO returns true if there is a solution, still need to use result;
         res.finished = true;
         printf("the string found: %s\n", res.password);
     } else {
         res.finished = false;
         printf("no solution found\n"  );
     }

     send_message(res); //send the reply TODO check what happens if the queue is full?

    close_mq(mq_name_req, mq_name_res);
    printf("child done..\n"  );



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

