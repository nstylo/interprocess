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
    char hash[128];
} MQ_REQ_MSG;

typedef struct
{
    char password[MAX_MESSAGE_LENGTH];
    bool finished;
} MQ_RES_MSG;

#endif

