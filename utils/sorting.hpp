#ifndef SORTING_H
#define SORTING_H
#include "utils.hpp"
#include "vector.hpp"
#include "random.hpp"
#include <algorithm>

namespace dmk{

template<typename ITEM, typename COMPARATOR>
void insertionSort(ITEM* vector, int left, int right, COMPARATOR const& c){
    for(int i = left + 1; i <= right; i++){
        // take item at current position
        ITEM e = vector[i];
        // move it up towards the right position
        int j = i;
        for(;j > left && c(e, vector[j - 1]); --j) vector[j] = vector[j -1];
        vector[j] = e;
    }
}

template<typename ITEM, typename COMPARATOR>
int pickPivot(ITEM* vector, int left, int right, COMPARATOR c){
    int i = GlobalRNG().inRange(left, right),
        j = GlobalRNG().inRange(left, right),
        k = GlobalRNG().inRange(left, right);
    if (c(vector[j], vector[i])) std::swap(i, j);
    // i <= j, decide where k goes
    return c(vector[k], vector[i]) ? i : c(vector[k], vector[j]) ?  k : j;
}

template<typename ITEM, typename COMPARATOR>
void partition3(ITEM* vector, int left, int right, int& i, int& j, COMPARATOR const& c){
    // i, j are the current left/right pointers
    ITEM p = vector[pickPivot(vector, left, right, c)];
    int lastLeftEqual = i = left - 1, firstRightEqual = j = right + 1;
    for(;;) // the pivot is the sentinel for the first pass
    { //after one swap swapped items act as sentinels
        while(c(vector[++i], p));
        while(c(p, vector[--j]));
        if(i >= j) break; // pointers crossed
        std::swap(vector[i], vector[j]); // both pointers found swappable items
        // swap equal items to the sides
        if (c.isEqual(vector[i], p)) // i to the left
            std::swap(vector[++lastLeftEqual], vector[i]);
        if (c.isEqual(vector[j], p)) // i to the right
            std::swap(vector[--firstRightEqual], vector[j]);
    }
    // invariant: i == j if they stop at an item = pivot
    // and this can happen at both left and right item
    // or they cross over and i = j + 1
    if (i == j){++i; --j;} // don't touch pivot in the middle
    // swap side items to the middle; left with "<" section and right with ">"
    for (int k = left; k <= lastLeftEqual; ++k)
        std::swap(vector[k], vector[j--]);
    for (int k = right; k >= firstRightEqual; --k)
        std::swap(vector[k], vector[i++]);
}

template<typename ITEM, typename COMPARATOR>
void quickSort(ITEM* vector, int left, int right, COMPARATOR const& c){
    // use quicksort for large arrays
    while(right - left > 16){
        int i, j;
        partition3(vector, left, right, i, j, c);
        if (j - left < right -i) // pick smaller
        {
            quickSort(vector, left, j, c);
            left = i;
        } else {
            quickSort(vector, i, right, c);
            right = j;
        }
    }
    // use insertionSort for small arrays
    insertionSort(vector, left, right, c);
}

template<typename ITEM> void quickSort(ITEM* vector, int n){
    quickSort(vector, 0, n-1, DefaultComparator<ITEM>());
}

template<typename ITEM, typename COMPARATOR>
int binarySearch(ITEM const* vector, int left, int right,
                 ITEM const& key, COMPARATOR const& c){
    while(left <= right){
        // careful to avoid overflow in middle calculation
        int middle = left + (right - left)/2;
        if(c.isEqual(key, vector[middle])) return middle;
        c(key, vector[middle]) ? right = middle - 1 : left = middle + 1;
    }
    return -1; // not found
}

template<typename ITEM, typename COMPARATOR>
bool isSorted(ITEM const* vector, int left, int right, COMPARATOR const& c){
    for (int i = left + 1; i <= right; ++i)
        if (c(vector[i], vector[i - 1])) return false;
    return true;
}

}

#endif // !SORTING_H
