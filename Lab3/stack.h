struct node;
typedef struct node {
	void *current;
	struct node *prev;
} stack_node;

typedef struct {
	size_t size;
	stack_node *last;
} stack;

int pop(stack *s, void *buf);
void push(stack *s, void *element);
void delete_stack(stack *s);
stack *create_stack(size_t size, void (*free_func)(void *));

