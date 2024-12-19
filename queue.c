#include "queue.h"


Node createNode(int socket, struct timeval arrival_time)
{
    Node new_node = (Node)malloc(sizeof(*new_node));
    new_node -> m_fd_socket = socket;
    new_node -> m_arrival_time = arrival_time;
    new_node -> m_next = NULL;

    return new_node;
}


Queue queueConstructor()
{
    Queue new_queue = (Queue)malloc(sizeof(*new_queue));
    new_queue -> m_size = 0;
    new_queue -> m_head = NULL;
    new_queue -> m_tail = NULL;

    return new_queue;
}


void append_node_to_queue(Queue my_queue, Node node_to_add)
{
    node_to_add ->m_next = NULL;

    if(my_queue->m_size == 0)
    {
        my_queue ->m_size ++;
        my_queue ->m_head = node_to_add;
        my_queue ->m_tail = node_to_add;
    }
    else
    {
        my_queue ->m_size ++;
        my_queue ->m_tail ->m_next = node_to_add;
        my_queue ->m_tail = node_to_add;
    }
}


void append_request_to_queue(Queue my_queue,int socket, struct timeval arrival_time )
{
    Node new_node = createNode(socket, arrival_time);
    append_node_to_queue(my_queue,new_node);
}


Node remove_queue_head(Queue my_queue)
{
    if(my_queue ->m_size ==0)
    {
        return  NULL;
    }
    Node node_to_return = my_queue ->m_head;

    if(my_queue ->m_size ==1)
    {
        my_queue -> m_size --;
        my_queue ->m_tail =NULL;
        my_queue ->m_head =NULL;
    }
    else
    {
        my_queue -> m_size --;
        my_queue ->m_head = node_to_return ->m_next ;
    }
    return  node_to_return;
}

/*
Node remove_queue_tail(Queue my_queue){

    if(my_queue ->m_size ==0)
    {
        return  NULL;
    }
    Node node_to_return = my_queue ->m_tail,prev = my_queue ->m_head;
     if(my_queue ->m_size ==1)
    {
        my_queue -> m_size --;
        my_queue ->m_tail =NULL;
        my_queue ->m_head =NULL;
    }
    else
    {
        for(int i=0 ; i < my_queue->m_size-1 ; i++)
        {
            prev = prev -> m_next;
        }
        my_queue -> m_size --;
        my_queue ->prev->m_next = NULL;
    }
    return  node_to_return;

}
*/

Node remove_node(Queue my_queue, Node node_to_remove)
{
    if(node_to_remove == NULL)
    {
        return NULL;
    }
    if(node_to_remove == my_queue ->m_head)
    {
        return remove_queue_head(my_queue);
    }
    else if(node_to_remove == my_queue ->m_tail)
    {
        my_queue ->m_size --;
        Node tmp_node = my_queue ->m_head;
        while (tmp_node ->m_next != my_queue ->m_tail)
        {
            tmp_node = tmp_node->m_next;
        }
        my_queue ->m_tail = tmp_node;

    }
    else
    {
        my_queue ->m_size --;
        Node prev_node = my_queue ->m_head;
        while (prev_node ->m_next != node_to_remove)
        {
            prev_node = prev_node->m_next;
        }
        prev_node->m_next = node_to_remove->m_next;
    }
    return node_to_remove;
}

Node remove_by_index(Queue my_queue, int index)
{
    if(index > my_queue->m_size - 1)
    {
        return NULL;
    }
    Node node_to_remove = my_queue->m_head;
    for(int i=0 ; i < index ; i++)
    {
        node_to_remove = node_to_remove->m_next;
    }
    return remove_node(my_queue, node_to_remove);
}


THREAD_STATS createThreadStats(int id){
    THREAD_STATS new_stats = malloc(sizeof(struct thread_stats));
    if(new_stats){
        new_stats->m_thread_id = id;
        new_stats->m_num_of_http_requests = 0;
        new_stats->m_num_static = 0;
        new_stats->m_num_dynamic = 0;
    }
    return new_stats;
}