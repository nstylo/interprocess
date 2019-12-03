/* 
 * Operating Systems {2INCO} Practical Assignment
 * Interprocess Communication
 *
 * Contains definitions which are commonly used by the farmer and the workers
 *
 */
#include <stdbool.h>

#ifndef COMMON_H
#define COMMON_H


// maximum size for any message in the tests
#define MAX_MESSAGE_LENGTH	6


// TODO: put your definitions of the datastructures here
typedef struct
{
    uint128_t hash;
    char first_letter;
    int alphabet_size;
    bool quit_flg;  //instructs the worker to terminate.
    int ID;
} MQ_REQ_MSG;

typedef struct
{
    bool finished;  //tells the farmer that this message contains a result.
    char password[MAX_MESSAGE_LENGTH + 1];
    int ID;
} MQ_RES_MSG;

#endif

