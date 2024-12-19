#ifndef __REQUEST_H__
#include "queue.h"


//////////////////
   Queue runningQueue;
   Queue waitingQueue;
   pthread_mutex_t global_lock;
   pthread_cond_t waiting_not_empty;
   pthread_cond_t queues_not_full;
   pthread_cond_t no_request_left;
   struct timeval arrivalTime;
   //////////////////

void requestError(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg, THREAD_STATS stats, Node request);
void requestServeDynamic(int fd, char *filename, char *cgiargs,THREAD_STATS stats,Node request);
void requestServeStatic(int fd, char *filename, int filesize,THREAD_STATS stats,Node request);
void requestHandle(int fd, Node request, THREAD_STATS stats,Node skipped);

#endif
