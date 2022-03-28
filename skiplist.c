#include <stdio.h>
#include <stdlib.h>


#define SKIPLIST_MAXLEVEL 32 /* Should be enough for 2^64 elements */
#define SKIPLIST_P 0.25      /* Skiplist P = 1/4 */


typedef struct skip_node skip_node_t;
typedef struct skip_list skip_list_t;

struct skip_node {
    int key;
    int value;
    skip_node_t *forward;
    skip_node_t *level[]; //指针数组
};

struct skip_list {
    unsigned long length;
    int level;
    skip_node_t *header; //头节点指针
    skip_node_t *tail;  //尾节点指针
};

skip_node_t *skip_node_create(int level, int key, int value){
    skip_node_t *node = malloc(sizeof(*node) + level*(sizeof(skip_node_t *)));
    node->key = key;
    node->value = value;
    return node;
}

skip_list_t* skip_list_create(){
    skip_list_t *slist = malloc(sizeof(*slist));
    slist->level = 1;
    slist->length = 0;

    slist->header = skip_node_create(SKIPLIST_MAXLEVEL, 0, 0);
    slist->header->forward = NULL;
    for(int i=0; i<SKIPLIST_MAXLEVEL; i++){
        slist->header->level[i] = NULL;
    }

    slist->tail = NULL;
    return slist;
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
        while(cur->level[i] && cur->level[i]->value < value){
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

    node->forward = update[0];
    if(node->level[0]){
        node->level[0]->forward = node;
    }else{
        l->tail = node;
    }

    l->length++;

    return node;
}

skip_node_t *skip_list_find(skip_list_t *l, int key){
    skip_node_t *cur = l->header;
    for (int i = l->level-1; i >= 0; i--) {
        while(cur->level[i] && key >= cur->level[i]->key){
            fprintf(stderr, "level: %d, cur: %d, level[i].key: %d\n", i, cur->key, cur->level[i]->key);

            cur = cur->level[i];
        }
        if(cur->key == key){
            return cur;
        }
    }
    return NULL;
}

#define K 1000
#define M (1000*1000)


int main(){
    skip_list_t *sl =  skip_list_create();

    for(int i=0; i<10*M; i++){
        skip_list_insert(sl, i, i*2);
    }

    skip_node_t *node;
    node = skip_list_find(sl, 10*M-1);
    if(node != NULL){
        fprintf(stderr, "found %d\n", node->value);
    }else{
        fprintf(stderr, "not found\n");
    }

    fprintf(stderr, "skiplist length is %lu, done.\n", sl->length);
    return 0;
}
