/*
 * This is the main queue1 structure
 */
typedef struct queue1
{
        int capacity;
        int size;
        int front;
        int rear;
        int *elements;
}queue11;

/*
 * This method creates a queue1 with maxElement number of elements and returns it
 */
queue1* createqueue1(int maxElements)
{
        /*Create a queue1*/
        queue1 *Q;
        Q = (queue1 *)malloc(sizeof(queue1));

        /* Initialize it's properties */
        Q->elements = (int *)malloc(sizeof(int)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = 0;
        Q->rear = -1;

        /*Return the pointer*/
        return Q;
}

/*
 * This method returns 1 if empty, and 0 if not empty
 */
int empty(queue1 *Q)
{
        if(Q->size == 0)
        {
                return 1;
        }
        else
        {
                return 0;
        }
}

/*This method pops and element off the queue1*/
void pop(queue1 *Q)
{
        if(Q->size != 0)
        {
                Q->size--;
                Q->front++;
                
                if(Q->front == Q->capacity)
                {
                        Q->front = 0;
                }
        }
}

/*Returns the top most element of the queue1*/
int peek(queue1 *Q)
{
        /* Return the element which is at the front*/
        return Q->elements[Q->front];
}

/*Pushes a new element onto the queue1*/
void push(queue1 *Q, int element)
{
        if(Q->size != Q->capacity)
        {
                Q->size++;
                Q->rear = Q->rear + 1;

                if(Q->rear == Q->capacity)
                {
                        Q->rear = 0;
                }

                Q->elements[Q->rear] = element;
        }
}
