#include <threads.h>
#include <stdio.h>

#define POP_ENA

enum { Q_OK, Q_ERROR };

typedef struct { /* Queue node */
    void *value;
    void *next;
} node_t;

typedef struct { /* Two lock queue */
    node_t *first, *last;
    mtx_t *first_mutex, *last_mutex;
} con_queue_t;

/* Free the queue struct. It assumes that the queue is depleted, and it will
 * not manage allocated elements inside of it.
 */
void con_free(con_queue_t *);

#include <stdlib.h>
#include <string.h>

inline static node_t *_con_node_init(void *value)
{
    node_t *node = malloc(sizeof(node_t));
    if (!node)
        return NULL;

    node->value = value;
    node->next = NULL;

    return node;
}

/* Allocates and initializes queue.
 * Returns a pointer to an allocated struct for the synchronized queue or NULL
 * on failure.
 */
con_queue_t *con_init()
{
    /* Allocate queue */
    con_queue_t *queue = malloc(sizeof(con_queue_t));
    if (!queue)
        return NULL;

    if ((queue->first_mutex = malloc(sizeof(mtx_t))) == NULL) {
        free(queue);
        return NULL;
    }
    if ((queue->last_mutex = malloc(sizeof(mtx_t))) == NULL) {
        free(queue->first_mutex);
        free(queue);
        return NULL;
    }

    if (mtx_init(queue->first_mutex, mtx_plain) != thrd_success ||
        mtx_init(queue->last_mutex, mtx_plain) != thrd_success) {
        con_free(queue);
        return NULL;
    }

    node_t *dummy = _con_node_init(NULL);
    if (!dummy) {
        con_free(queue);
        return NULL;
    }

    queue->first = queue->last = dummy;

    return queue;
}

void con_free(con_queue_t *queue)
{
    if (!queue)
        return;

    if (!queue->first)
        free(queue->first);

    if (queue->first_mutex) {
        mtx_destroy(queue->first_mutex);
        free(queue->first_mutex);
    }
    if (queue->last_mutex) {
        mtx_destroy(queue->last_mutex);
        free(queue->last_mutex);
    }

    free(queue);
}

/* Add element to queue. The client is responsible for freeing elementsput into
 * the queue afterwards. Returns Q_OK on success or Q_ERROR on failure.
 */
int con_push(con_queue_t *restrict queue, void *restrict new_element)
{
    /* Prepare new node */
    node_t *node = _con_node_init(new_element);
	//printf("node => value = %d\n", *(int*)node->value);
    if (!node)
        return Q_ERROR;

    /* Add to queue with lock */
    mtx_lock(queue->last_mutex);
	// TOFILL: AAA;
	queue->last->next = node;
    queue->last = node;
    mtx_unlock(queue->last_mutex);
/*	mtx_lock(queue->first_mutex);
	//if(queue->first->value == NULL){
	if( queue->first->next == NULL){
	//	printf("queue->first->value=NULL\n");
		queue->first->next = node;
	}
	mtx_unlock(queue->first_mutex);
*/
#ifdef DEBUG
	if(queue->first->value && queue->last->value)
		printf("@@queue->first->value=%d, last->value=%d\n", *(int*)(queue->first->value), *(int*)(queue->last->value));
	node_t *real_first = queue->first->next;
	if(real_first && real_first->value && queue->last->value)
		printf("@@queue->first->next->value=%d, last->value=%d\n", *(int*)(real_first->value), *(int*)(queue->last->value));
#endif
    return Q_OK;
}

/* Retrieve element and remove it from the queue.
 * Returns a pointer to the element previously pushed in or NULL of the queue is
 * emtpy.
 */
void *con_pop(con_queue_t *queue)
{
    mtx_lock(queue->first_mutex);
	
    node_t *node = queue->first;             /* Node to be removed */ //DUMMY node
    node_t *new_header = queue->first->next; /* become the first in the queue */ // node to be removed
/*	if(node && node->value)
		printf("***con_pop get lock, node value = %d\n", *(int*)node->value);
	else if(!node)
		printf("***node = NULL\n");
	else
		printf("***node->value = NULL\n");*/

/*	
	if(new_header && new_header->value)
		printf("***con_pop get lock, new_header value = %d\n", *(int*)new_header->value);
	else if(!new_header)
		printf("***new_header = NULL\n");
	else
		printf("***new_header->value = NULL\n");
*/
    /* Queue is empty */
    if (!new_header) {
        mtx_unlock(queue->first_mutex);
	//	printf("con_pop return empty header\n");
        return NULL;
    }
	//printf("con_pop new_header value = %d\n", *(int*)new_header->value);
    /* Queue not empty: retrieve data and rewire */
    void *return_value = new_header->value;// TOFILL:BBB;
    // TOFILL: CCC;
    queue->first->next = new_header->next;
	node = new_header;
	mtx_lock(queue->last_mutex);
	if(!queue->first->next)
		queue->last = queue->first;
	mtx_unlock(queue->last_mutex);
	//printf("con_pop return new header\n");
	//printf("after pop first->value=%d, last->value=%d\n", *(int*)queue->first->value, *(int*)queue->last->value);

    mtx_unlock(queue->first_mutex);
	
    /* Free removed node and return */
     free(node);
    //free(new_header);
    return return_value;
}



void *con_pop_refined(con_queue_t *queue)
{
    mtx_lock(queue->first_mutex);
	
    node_t *dummy = queue->first;        /* First: dummy node */ 
    node_t *node = queue->first->next;  /* Node to be removed */

    if (!node) {
        mtx_unlock(queue->first_mutex);
        return NULL;
    }

    /* Queue not empty: retrieve data and rewire */
    void *return_value = node->value;
    queue->first->next = node->next;
	mtx_lock(queue->last_mutex);
	if(!queue->first->next)
		queue->last = queue->first;
	mtx_unlock(queue->last_mutex);


    mtx_unlock(queue->first_mutex);
	
    /* Free removed node and return */
    free(node);
    return return_value;
}

#include <assert.h>
#include <stdio.h>

#define N_PUSH_THREADS 4
#define N_POP_THREADS 4
#define NUM 1000000
//#define NUM 10

/* This thread writes integers into the queue */
int push_thread(void *queue_ptr)
{
    con_queue_t *queue = (con_queue_t *) queue_ptr;

    /* Push ints into queue */
    for (int i = 0; i < NUM; ++i) {
        int *pushed_value = malloc(sizeof(int));
        *pushed_value = i;
        if (con_push(queue, pushed_value) != Q_OK)
            printf("Error pushing element %i\n", i);
    }

    thrd_exit(0);
}

/* This thread reads ints from the queue and frees them */
int pop_thread(void *queue_ptr)
{
    con_queue_t *queue = (con_queue_t *) queue_ptr;

    /* Read values from queue. Break loop on -1 */
    while (1) {
        int *popped_value = con_pop(queue);
        if (popped_value) {
            if (*popped_value == -1) {
                free(popped_value);
                break;
            }

            free(popped_value);
        }
    }

    thrd_exit(0);
}

int main()
{
    thrd_t push_threads[N_PUSH_THREADS], pop_threads[N_POP_THREADS];

    con_queue_t *queue = con_init();

    for (int i = 0; i < N_PUSH_THREADS; ++i) {
		printf("pushing thread, i =%d\n",i);
        if (thrd_create(&push_threads[i], push_thread, queue) != thrd_success)
            printf("Error creating push thread %i\n", i);
    }
#ifdef POP_ENA
    for (int i = 0; i < N_POP_THREADS; ++i) {
		printf("popping thread, i =%d\n",i);
        if (thrd_create(&pop_threads[i], pop_thread, queue) != thrd_success)
            printf("Error creating pop thread %i\n", i);
    }
#endif
    for (int i = 0; i < N_PUSH_THREADS; ++i) {
		printf("joining pushing thread, i =%d\n",i);
        if (thrd_join(push_threads[i], NULL) != thrd_success)
            continue;
    }

    /* Push kill signals */
    for (int i = 0; i < N_POP_THREADS; ++i) {
		printf("killing thread, i =%d\n",i);
        int *kill_signal = malloc(sizeof(int)); /* signal pop threads to exit */
        *kill_signal = -1;
        con_push(queue, kill_signal);
    }
#ifdef POP_ENA

    for (int i = 0; i < N_POP_THREADS; ++i) {
		printf("joining popping thread, i =%d\n",i);
        if (thrd_join(pop_threads[i], NULL) != thrd_success)
            continue;
    }
#endif
    con_free(queue);
    return 0;
}
