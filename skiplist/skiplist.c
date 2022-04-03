/*
一个支持多重key的skiplist
*/


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>


#define SKIPLIST_MAXLEVEL 32 /* Should be enough for 2^64 elements */
#define SKIPLIST_P 0.25      /* Skiplist P = 1/4 */


#define skip_list_foreach(node, l) \
        for ((node) = (l)->header->level[0]; (node)!=(l)->header; (node)=(node)->level[0])


#define skip_list_foreach_safe(node, l) \
        (node) = (l)->header->level[0]; \
        for (skip_node_t *tMp__=(node)->level[0]; (node)!=(l)->header; (node)=tMp__, tMp__=(node)->level[0])


#define skip_list_foreach_reverse(node, l) \
        for ((node) = (l)->tail; node!=(l)->header; (node)=(node)->backward)


#define skip_list_foreach_reverse_safe(node, l) \
        (node) = (l)->tail; \
        for (skip_node_t *tMp__=(node)->backward; (node)!=(l)->header; (node)=tMp__, tMp__=(node)->backward)


typedef struct skip_node skip_node_t;
typedef struct skip_list skip_list_t;

struct skip_node {
    int key;
    int value;
    skip_node_t *backward;
    skip_node_t *level[]; //比redis的skiplist,缺少了backword指针, 占用空间小, 但按节点地址删除复杂, 具有多重key的时候(多重字典), 删除性能差.
};

struct skip_list {
    unsigned long length;
    int level;
    skip_node_t *header; //循环列表, 方便逆序遍历
    skip_node_t *tail; // 和header->backward 完全等价, 只是为了方便.
};

skip_node_t *skip_node_create(int level, int key, int value){
    skip_node_t *node = malloc(sizeof(*node) + level*(sizeof(skip_node_t *)));
    node->key = key;
    node->value = value;
    return node;
}

void skip_node_destroy(skip_node_t *node){
    free(node);
}

skip_list_t* skip_list_create(){
    skip_list_t *slist = malloc(sizeof(*slist));
    slist->level = 1;
    slist->length = 0;

    slist->header = skip_node_create(SKIPLIST_MAXLEVEL, INT_MIN, INT_MIN); //方便debug,容易发现是头节点
    slist->header->backward = slist->header;
    for(int i=0; i<SKIPLIST_MAXLEVEL; i++){
        slist->header->level[i] = slist->header; // 使用循环链表, 方便实现 skip_list_for_each_safe
    }

    slist->tail = slist->header;  //方便倒序遍历
    return slist;
}

void skip_list_destroy(skip_list_t *l){
    skip_node_t *cur = l->header->level[0];
    for(skip_node_t *next=cur->level[0]; cur!=l->header; cur=next, next=cur->level[0]){
        skip_node_destroy(cur);
    }
    skip_node_destroy(l->header);
    free(l);
}

static int random_level(void) {
    static const int threshold = SKIPLIST_P*RAND_MAX;
    int level = 1;
    while (random() < threshold)
        level += 1;
    return (level<SKIPLIST_MAXLEVEL) ? level : SKIPLIST_MAXLEVEL;
}

skip_node_t *skip_list_insert(skip_list_t *l, int key, int value){
    skip_node_t *update[SKIPLIST_MAXLEVEL] = {};

    skip_node_t *cur = l->header;
    for(int i=l->level-1; i>=0; i--){
        while(cur->level[i] != l->header && cur->level[i]->key < key){
            cur = cur->level[i];
        }
        update[i] = cur;
    }

    int insert_level = random_level();
    if(insert_level > l->level){
        for(int i=l->level; i<insert_level; i++){
            update[i] = l->header;
        }
        l->level = insert_level;
    }

    skip_node_t *node = skip_node_create(insert_level, key, value);
    for(int i=0; i<insert_level ; i++){
        skip_node_t *next = update[i]->level[i];
        node->level[i] = next;

        skip_node_t *prev = update[i];
        prev->level[i] = node;
    }

    node->backward = update[0];
    if(node->level[0] != l->header){
        node->level[0]->backward = node;
    }else{
        l->header->backward = node;
        l->tail = node;
    }

    l->length++;

    return node;
}

skip_node_t *skip_list_find(skip_list_t *l, int key){
    skip_node_t *cur = l->header;
    for (int i = l->level-1; i >= 0; i--) {
        while(cur->level[i] != l->header && cur->level[i]->key < key){
//            fprintf(stderr, "level: %d, cur: %d, level[i].key: %d\n", i, cur->key, cur->level[i]->key);
            cur = cur->level[i];
        }

        skip_node_t *next = cur->level[i];
        if(next != l->header && next->key == key){
            return next;
        }
    }
    return NULL;
}

void skip_list_print(skip_list_t *l){
    for(int i=l->level-1; i>=0; i--){
        printf("level %d: ", i);
        for(skip_node_t *cur=l->header->level[i]; cur!=l->header; cur=cur->level[i]){
            printf("%d-", cur->key);
        }
        printf("NULL\n");
    }
}

int skip_list_remove(skip_list_t *l, int key){
    skip_node_t *update[SKIPLIST_MAXLEVEL] = {};

    skip_node_t *cur = l->header;
    for(int i=l->level-1; i>=0; i--){
        while(cur->level[i] != l->header && cur->level[i]->key < key){
            cur = cur->level[i];
        }
        update[i] = cur;
    }

    cur = cur->level[0];
    if(cur == l->header || cur->key != key){
        return ENOENT;
    }

    for(int i=l->level-1; i>=0 ; i--){
        skip_node_t *prev = update[i];
//        fprintf(stderr, "prev->level[%d]: %p == cur: %p; prev->level[i]: %d, %d\n", i, prev->level[i], cur, prev->level[i]->key, cur->key);

        if(prev->level[i] == cur){
            prev->level[i] = cur->level[i];
        }
    }

    skip_node_t *next = cur->level[0];

    if(next != l->header){
        next->backward = update[0];
    }else{
        l->header->backward = update[0];
        l->tail = update[0];
    }

    skip_node_destroy(cur);
    l->length--;

    while(l->header->level[l->level-1] == l->header){
        l->level--;
    }

    return 0;
}

int skip_list_remove_node(skip_list_t *l, skip_node_t *node){
    int key = node->key;

    skip_node_t *update[SKIPLIST_MAXLEVEL] = {};

    skip_node_t *cur = l->header;
    for(int i=l->level-1; i>=0; i--){
        while(cur->level[i] != l->header && cur->level[i]->key < key){
            cur = cur->level[i];
        }
        update[i] = cur;
    }

    for(cur=cur->level[0]; cur!=l->header && cur->key == key; cur=cur->level[0]){
        if(cur == node){
            //found
            break;
        }
    }

    if(cur == l->header || cur->key != key){
        return ENOENT;
    }

    //已经找到节点, cur == node
    int i;
    skip_node_t *prev;
    for(i=l->level-1; i>=0 ; i--){
        prev = update[i];

        //找到实际的前驱节点
        while(prev->level[i]->key == key && prev->level[i] != node && prev->level[i] != l->header){
            prev = prev->level[i];
        }

        if(prev->level[i] == node){
            //printf(">>> 更新前驱节点 prev->level[%d](%d).next = node.next(%d)\n", i, prev->key, node->level[i]->key);
            prev->level[i] = node->level[i];
        }
    }

    skip_node_t *next = node->level[0];

    if(next != l->header){
        next->backward = prev;
    }else{
        l->header->backward = prev;
        l->tail = prev;
    }

    skip_node_destroy(node);
    l->length--;

    while(l->header->level[l->level-1] == l->header){
        l->level--;
    }

    return 0;
}


#define K 1000
#define M (1000*1000)


int main(){
    skip_list_t *sl =  skip_list_create();

    int num_list[20];
    for(int i=0; i<20; i++){
        num_list[i] = random() % 20;
    }

    for(int i=0; i<20; i++){
        skip_list_insert(sl, num_list[i],  -num_list[i]);
    }

    fprintf(stderr, "skiplist count is %lu, level is %d.\n", sl->length, sl->level);
    skip_list_print(sl);

    int delete_ele[] = {11,22,33,3,3,3,5,7,7,7,19,19,-1};
    for(int i=0; delete_ele[i]!=-1; i++){
        int ret = skip_list_remove(sl, delete_ele[i]);
        fprintf(stderr, "remove: %d %s\n", delete_ele[i], ret==0?"success":"failed");
    }
    fprintf(stderr, "skiplist count is %lu, level is %d.\n", sl->length, sl->level);

    skip_node_t *node;
    node = skip_list_find(sl, 6);
    if(node != NULL){
        fprintf(stderr, "found key: %d, value is: %d\n", node->key, node->value);
    }else{
        fprintf(stderr, "not found\n");
    }

    fprintf(stderr, "==== test print\n");

    skip_list_print(sl);

    fprintf(stderr, "==== test reverse\n");
    skip_list_foreach_reverse(node, sl) {
        fprintf(stderr, "%d-", node->key);
    }
    fprintf(stderr, "\n\n");


    fprintf(stderr, "test skip_list_for_each_reverse_safe\n");
    skip_list_foreach_reverse_safe(node, sl){
        fprintf(stderr, "delete node key: %d,\n", node->key);
        skip_list_remove_node(sl, node);
    }
    skip_list_print(sl);


    fprintf(stderr, "test skip_list_for_each_safe\n");
    skip_list_foreach_safe(node, sl){
        fprintf(stderr, "%d-", node->key);
    }

    return 0;
}

