#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define MINSIZE 5

/*******
 * 环形数组实现的队列
 * queue元素和大小也可以用front, rear（队尾元素下标）表示，不使用size, 这样的队列能存储的元素是数组长度减一。（由于队列size的状态比数组元素个数多一。 假设长度为1的数组表示队列，不使用size, 怎么区分队列空和满？）
 *
 * 通过使用size变量，判断队列空和满最简单。并且（front+size） % capacity 永远是队尾元素下一个元素的下标。
 * 这样也能省略掉rear变量了。维护的变量可以少一个。
 */

struct queue {
    uint32_t capacity;
    uint32_t front;
    uint32_t size;
    uint32_t array[];
};

typedef struct queue queue;

queue *create_queue(uint32_t max_elements) {
    uint32_t capacity = max_elements > MINSIZE? max_elements: MINSIZE;
    queue *q = malloc(sizeof(*q) + sizeof(uint32_t) * capacity);
    q->capacity = capacity;
    q->front = 0;
    q->size = 0;
    return q;
}

void destroy_queue(queue *q) {
    free(q);
}

bool is_empyt(queue *q) {
    return q->size == 0;
}

bool is_full(queue *q) {
    return q->size == q->capacity;
}

void make_empty(queue *q) {
    q->front = 0;
    q->size = 0;
}


bool enqueue(queue *q, uint32_t value) {
    if(is_full(q)) {
        return false;
    }
    uint32_t index = (q->front + q->size) % q->capacity;
    q->array[index] = value;
    q->size++;
    return true;
}

bool getfront(queue *q, uint32_t *value) {
    if(is_empyt(q)){
        return false;
    }

    *value = q->array[q->front];
    return true;
}


bool dequeue(queue *q, uint32_t *value) {
    if(is_empyt(q)) {
        return false;
    }
    *value = q->array[q->front];
    q->front++;
    if(q->front == q->capacity) {
        q->front = 0;
    }
    q->size--;
}

bool getrear(queue *q, uint32_t *value) {
    if(is_empyt(q)){
        return false;
    }
    uint32_t index = (q->front + q->size - 1) % q->capacity;
    *value = q->array[index];
    return true;
}

void print_queue_detail(queue *q) {
    uint32_t rear = (q->front + q->size - 1) % q->capacity;
    printf("queue size:%d, front: %d, rear: %d\n", q->size, q->front, rear);
    
    uint32_t display[q->capacity];
    memset(display, -1, sizeof(display));

    for(int i = 0; i < q->size; i++) {
        uint32_t index = (q->front + i) % q->capacity;
        display[index] = q->array[index];
    }

    for(int i = 0; i < q->capacity; i++) {
        if(display[i] == UINT32_MAX){
            printf("|  ");
        }else{
            printf("|%2u", display[i]);
        }
    }
    printf("\n\n");
}

int main() {
    queue *q = create_queue(5);

    print_queue_detail(q);
    for(int i=0; i< 3; i++) {
        enqueue(q, i);
        printf("enqueue %d\n", i);
        print_queue_detail(q);
    }

    for(int i=1; i< 7; i++) {
        uint32_t x;
        dequeue(q, &x);
        printf("dequeue %d\n", i);
        print_queue_detail(q);
    }

    for(int i=0; i< 7; i++) {
        enqueue(q, i);
        printf("enqueue %d\n", i);
        print_queue_detail(q);
    }

    return 0;
}
