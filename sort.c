#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void bubble_sort(int32_t arr[], uint32_t len){
    for(int i=1; i<len; i++){  //i为第几次遍历, 一共需要len-1次遍历
        for(int j=0; j<(len-i); j++){  //第1次遍历,需要比较len-1次
            if(arr[j] > arr[j+1]){  // 最大的元素放在最后面
                int32_t tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = tmp;
            }
        }
    }
}

void selection_sort(int32_t arr[], uint32_t len){
    for(int i=0; i<len-1; i++){ //共需要len-1次选择
        //每次查找最小的元素, 和arr[i]交换位置(放前面), 下次选择可以从i+1开始.
        uint32_t min_index = i; 
        for(int j=i+1; j<len; j++){
            if(arr[j] < arr[min_index]){
                min_index = j;
            }
        }
        uint32_t tmp = arr[i];
        arr[i] = arr[min_index];
        arr[min_index] = tmp;
    }
}

void _quick_sort(int32_t arr[], int32_t left, int32_t right){
    if(left >= right)
        return;

    int32_t low = left;
    int32_t high = right;
    int32_t pivot = arr[low];

    while(low < high){
        if(arr[high] >= pivot && low < high){
            high--;
        }
        arr[low] = arr[high];

        if(arr[low] <= pivot && low < high){
            low++;
        }
        arr[high] = arr[low];
    }
    arr[low] = pivot;

    _quick_sort(arr, left, low-1);
    _quick_sort(arr, low+1, right);
}

void quick_sort(int32_t arr[], uint32_t len){
    _quick_sort(arr, 0, len-1);
}

void _merge(int32_t arr[], int32_t tmp_arr[], int32_t lpos, int32_t rpos, int32_t rend){
    uint32_t lend = rpos-1;
    uint32_t tmp_pos = lpos;
    uint32_t start = lpos;

    while(lpos <= lend && rpos <= rend){
        if(arr[lpos] <= arr[rpos]){
            tmp_arr[tmp_pos++] = arr[lpos++];
        }else{
            tmp_arr[tmp_pos++] = arr[rpos++];
        }
    }

    while(lpos <= lend){
        tmp_arr[tmp_pos++] = arr[lpos++];
    }
    while(rpos <= rend){
        tmp_arr[tmp_pos++] = arr[rpos++];
    }

    while(start <= rend){
        arr[start] = tmp_arr[start];
        start++;
    }
}
void _merge_sort(int32_t arr[], int32_t tmp_arr[], int32_t left, int32_t right){
    if(left < right){
        int32_t center = (right+left)/2;
        _merge_sort(arr, tmp_arr, left, center);
        _merge_sort(arr, tmp_arr, center+1, right);
        _merge(arr, tmp_arr, left, center+1, right);
    }
}

void merge_sort(int32_t arr[], int32_t len){
    int32_t *tmp_arr = malloc(sizeof(*tmp_arr) * len);
    if(tmp_arr != NULL){
        _merge_sort(arr, tmp_arr, 0, len-1);
        free(tmp_arr);
    }
}

int main(){
    int32_t arr[] = {7,8,9,1,2,3,0,6,5,4};
    int32_t len = sizeof(arr)/sizeof(*arr);
    
    merge_sort(arr, len);

    for(int i=0; i<len; i++){
        printf("%d ", arr[i]);
    }
}

