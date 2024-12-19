#include "segel.h"
#include "request.h"
#include <unistd.h>

// 
// server.c: A very, very simple web server
//
// To run:
//  ./server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//
//THREAD_STATS globalStatsArr;
// HW3: Parse the new arguments too
void getargs(int *port,int *threads, int *queue,int argc, char *argv[])
{
    if (argc < 5) {
	    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	    exit(1);
    }
    *port = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *queue = atoi(argv[3]);
}

/*
Queue runningQueue;
Queue waitingQueue;
pthread_mutex_t global_lock;
pthread_cond_t waiting_not_empty;
pthread_cond_t queues_not_full;
pthread_cond_t no_request_left;
*/


void* threadDoWork(void *args){

    THREAD_STATS stats = (THREAD_STATS)args;
    int thread_id = stats->m_thread_id;
    struct timeval arrivalTime;

    while(1){
        pthread_mutex_lock(&global_lock);

        while(waitingQueue->m_size == 0){
            pthread_cond_wait(&waiting_not_empty, &global_lock);  
        }
    ////////////////// now we want preapre the request before handling it 

        Node request = remove_queue_head(waitingQueue);
        gettimeofday(&arrivalTime, NULL);
        timersub(&arrivalTime ,&request->m_arrival_time, &request->m_dispatch_time);
        request->m_thread_id = thread_id;
        append_node_to_queue(runningQueue, request); 

        pthread_mutex_unlock(&global_lock);
    ////////////////// finshed preapring the request
   
   ////////////////// now we want to handle the request
   ///////////////// we uptade the stats of the thread in the request handler
   
       Node skipped=NULL;
       requestHandle(request->m_fd_socket, request, stats,skipped);
	   Close(request->m_fd_socket); 

       if(skipped != NULL){
       requestHandle(skipped->m_fd_socket, skipped, stats,NULL);
	   Close(skipped->m_fd_socket);
       }

   /////////////// finshed handling the request  

    ////////////////// now we want to remove the request from the running queue after ew handled it
       pthread_mutex_lock(&global_lock);
       //remove_request_from_queue(runningQueue,request);
       remove_node(runningQueue,request);
       if(skipped != NULL) remove_node(runningQueue,skipped);
       pthread_mutex_unlock(&global_lock);

       pthread_cond_signal(&queues_not_full); ////// make sure the cond is correct

       pthread_mutex_lock(&global_lock);
       if(waitingQueue->m_size == 0 && runningQueue->m_size == 0){
            pthread_cond_signal(&no_request_left);
       }
       pthread_mutex_unlock(&global_lock);
    ////////////////// finshed removing the request from the running queue
    }

    
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen, threads_num, max_queue_size;

    char* schedalg = (char*)malloc(sizeof(strlen(argv[4]) + 1));
    struct timeval arrivalTime;
    strcpy(schedalg, argv[4]);

    struct sockaddr_in clientaddr;

    runningQueue = queueConstructor();
    waitingQueue = queueConstructor();

    getargs(&port, &threads_num, &max_queue_size, argc, argv);

    pthread_mutex_init(&global_lock, NULL);
    pthread_cond_init(&waiting_not_empty,NULL);
    pthread_cond_init(&queues_not_full,NULL);
    pthread_cond_init(&no_request_left,NULL);

    pthread_t* threadsArr = (pthread_t*) malloc(sizeof (pthread_t) * threads_num);

     for (int i = 0; i < threads_num; ++i) {
        THREAD_STATS statsArr = createThreadStats(i);
        pthread_create(&threadsArr[i], NULL, threadDoWork, statsArr);
    }

    listenfd = Open_listenfd(port);
    while (1) {
	    clientlen = sizeof(clientaddr);
	    connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        gettimeofday(&arrivalTime, NULL);

        pthread_mutex_lock(&global_lock);
    /////////////////////////////////// main thread work starts here


    ////////////////////////////////// here we sepreate the behavior of the main thread according to the scheduler algorithm
        if(runningQueue->m_size + waitingQueue->m_size >= max_queue_size){
        
            if(strcmp(schedalg, "block") == 0){

                //while(runningQueue->m_size + waitingQueue->m_size == max_queue_size){ // ymkn no need for while :: try without
                        pthread_cond_wait(&queues_not_full, &global_lock);
                //}
            } 
            else if(strcmp(schedalg, "dt") == 0){
                    Close(connfd);
                    pthread_mutex_unlock(&global_lock);
                    continue;
            } 
            else if(strcmp(schedalg, "dh") == 0){
                    if (waitingQueue->m_size > 0) {
                        Node node_to_drop = remove_queue_head(waitingQueue);
                        Close(node_to_drop->m_fd_socket);
                    } else {
                        close(connfd);
                        pthread_mutex_unlock(&global_lock);
                        continue;
                    }
            }
            else if(strcmp(schedalg, "bf") == 0){
                    pthread_cond_wait(&no_request_left, &global_lock); 
                    Close(connfd);
                    pthread_mutex_unlock(&global_lock);
                    continue;
            }
            else if(strcmp(schedalg, "random") == 0){
                    if(waitingQueue->m_size == 0){
                        close(connfd);
                        pthread_mutex_unlock(&global_lock);
                        continue;
                    }

                    int half = waitingQueue->m_size/2;
                    half+= waitingQueue->m_size % 2;
                    for (int i = 0; i < half; ++i) {
                        Node node_to_drop = remove_by_index(waitingQueue, rand() % waitingQueue->m_size);
                        Close(node_to_drop->m_fd_socket);
                    }
            }
    }
    ////////////////////////////////
    append_request_to_queue(waitingQueue, connfd, arrivalTime);
    pthread_cond_signal(&waiting_not_empty);

    //////////////////////////////// main thread work ends here 
    pthread_mutex_unlock(&global_lock);
	// 
	// HW3: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	// 
    }
}


    


 
