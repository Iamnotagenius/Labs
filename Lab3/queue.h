struct node_q;
typedef struct node_q {
    void *item;
    struct node_q *next;
    struct node_q *previous;
} queue_node;
 
typedef struct {
    size_t size;
    queue_node *head;
    queue_node *tail;
	int length;
} queue;

queue *create_queue(size_t size);
void delete_queue(queue *s);
void push_q(queue *s, void *element);
int pop_q(queue *s, void *buf);
void *head_q(queue *q);
void *tail_q(queue *q);
