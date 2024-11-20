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

template<typename ITEM, typename COMPARATOR>
void merge(ITEM* vector, int left, int middle, int right, COMPARATOR const& c, ITEM* storage){
    // i for the left half, j for the right half, merge unitll the vector is filled up
    for (int i = left, j = middle + 1; left <= right; ++left) {
        // either i or j can get out of bounds
        bool useRight = i > middle || ( j <= right && c(storage[j], storage[i]));
        vector[left] = storage[(useRight ? j : i)++];
    }
}

template<typename ITEM, typename COMPARATOR>
void mergeSortHelper(ITEM* vector, int left, int right, COMPARATOR const& c, ITEM* storage){
    if(right - left > 16)
    {
        // sort storage using vector as storage, then merge into vector
        int middle = (right + left) / 2;
        mergeSortHelper(storage, left, middle, c, vector);
        mergeSortHelper(storage, middle + 1, right, c, vector);
        merge(vector, left, middle, right, c, storage);
    }
    else insertionSort(vector, left, right, c);
}

template<typename ITEM, typename COMPARATOR>
void mergeSort(ITEM* vector, int n, COMPARATOR const& c){
    if(n <= 1) return;
    Vector<ITEM> storage(n, vector[0]); // reserve space for n with 1st item
    for (int i = 0; i < n; ++i) storage[i] = vector[i];
    mergeSortHelper(vector, 0, n-1, c, storage.getArray());
}

void countingSort(int* vector, int n, int N);

template<typename ITEM, typename ORDERED_HASH>
void KSort(ITEM* a, int n, int N, ORDERED_HASH const& h){
    ITEM* temp = rawMemory<ITEM>(n);
    Vector<int> prec(N + 1, 0);
    for (int i = 0; i < n; ++i) ++prec[h(a[i]) + 1];
    for (int i = 0; i < N; ++i) prec[i + 1] += prec[i]; // accumulate counts
    // rearrange items
    for (int i = 0; i < n; ++i) new(&temp[prec[h(a[i])]++]) ITEM(a[i]);
    for (int i = 0; i < n; ++i) a[i] = temp[i];
    rawDelete(temp);
}

}

#endif // SORTING_H
