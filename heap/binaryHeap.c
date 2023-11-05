/****************
 * 来源： 数据结构与算法分析：C语言描述 第二版
 * 
 * 本例是 最小堆
 * ***********/

#ifndef _BINHEAP_H_
#define _BINHEAP_H_

#define Error(msg, ...) do { \
    fprintf(stdout, "Error: file:%s, line:%d: ", __FILE__, __LINE__, msg, ##__VA_ARGS__);
} while(0)

#define MinPQSize 3
struct BinaryHeap;
typedef struct BinaryHeap *PriorityQueue;


PriorityQueue Initialize(int MaxElements);
void Destory(PriorityQueue H);
void MakeEmpty(PriorityQueue H);
void Insert(ElementType x, PriorityQueue H);
ElementType DeleteMin(PriorityQueue H);
ElementType FindMin(PriorityQueue H);
int isEmpty(PriorityQueue H);
int isFull(PriorityQueue H);

/**************
 * 其他堆操作
 * DecreaseKey 降低关键字的值
 * IncreaseKey 增加关键字的值
 * Delete 删除
 * BuildHeap 构建堆
 ***********************/

#endif //_BINHEAP_H_

struct BinaryHeap {
    int Capacity;
    int Size;

    //从下标1开始存数据。下标0用来存MinData,哨兵算法
    ElementType Elements[];
};

PriorityQueue Initialize(int MaxElements) {
    PriorityQueue H;
    if(MaxElements < MinPQSize){
        MaxElements = MinPQSize;
    }

    int elementsSize = (MaxElements + 1) * sizeof(ElementType);
    H = malloc(sizeof(*H) + elementsSize);
    if( H == NULL ){
        return NULL;
    }

    H->Capacity = MaxElements;
    H->Size = 0;
    H->Elements[0] = MinData;
    return H;
}


void Insert(ElementType x, PriorityQueue H) {
    int i;
    if(IsFull(H)) {
        Error("PriorityQueue is full");
        return;
    }

    for(i = ++H->Size; H->Elements[i/2] > x; i /= 2) {
        H->Elements[i] = H->Elements[i/2];
    }
    H->Elements[i] =x;
}

ElementType DeleteMin(PriorityQueue H) {
    int i, Child;
    ElementType MinElement, LastElement;

    if(isEmpty(H)) {
        Error("PriorityQueue is empty");
        return H->Elements[0];
    }
    
    MinElement = H->Elements[1];
    LastElement = H->Elements[ H->Size-- ];

    for(i = 1; i*2 <= H->Size; i = Child) {
        Child = i * 2;
        //找到最小的child
        if (Child != H->Size && H->Elements[Child +1]
                              < H->Elements[Child] ) {
            Child++;
        }

        //提升一级
        if(LastElement > H->Elements[Child]) {
            H->Elements[i] = H->Elements[Child];
        } else {
            break;
        }
    }

    H->Elements[i] = LastElement;
    return MinElement;
}


