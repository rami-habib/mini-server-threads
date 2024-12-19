

#ifndef WEBSERVER_FILES_QUEUE_H
#define WEBSERVER_FILES_QUEUE_H


#include <sys/time.h>
#include <stdlib.h>

typedef struct node{

    int m_thread_id;
    int m_fd_socket;
    struct timeval m_arrival_time;
    struct timeval m_dispatch_time;
    struct node* m_next;

} *Node;

typedef struct queue{
    int m_size;
    Node m_head;
    Node m_tail;
}*Queue;

Node createNode(int socket, struct timeval arrival_time);

Queue queueConstructor();

void append_node_to_queue(Queue my_queue, Node node_to_add);
void append_request_to_queue(Queue my_queue,int socket, struct timeval arrival_time );

Node remove_queue_head(Queue my_queue);
//Node remove_queue_tail(Queue my_queue);
Node remove_node(Queue my_queue, Node node_to_remove);
Node remove_by_index(Queue my_queue, int index);



typedef struct thread_stats{
    int m_thread_id;
    int m_num_of_http_requests;
    int m_num_static;
    int m_num_dynamic;
}*THREAD_STATS;

THREAD_STATS createThreadStats(int id);





#endif //WEBSERVER_FILES_QUEUE_H
