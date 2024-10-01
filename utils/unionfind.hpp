#ifndef UNIONFIND_H
#define UNIONFIND_H

#include "vector.hpp"

namespace useful {

    class UnionFind{
        // Implements union-by-size with compression
        mutable Vector<int> parent;
    public:
        UnionFind(int size): parent(size, -1){}
        bool isRoot(int n)const{return parent[n] < 0;};
        int find(int n) const{
            // compress path by setting parent[n] to the root index
            return isRoot(n) ? n : (parent[n] = find(parent[n]));
        }
        bool areEquivalent(int i, int j) const{return find(i) == find(j);}
        int subsetSioze(int i) const{return -parent[find(i)];}
        void addSubset(){parent.append(-1);}
        void join(int i, int j){
            int parentI = find(i), parentJ = find(j);
            if(parentI != parentJ){
                if(parent[parentI] > parent[parentJ]) std::swap(parentI, parentJ);
                parent[parentI] += parent[parentJ];
                parent[parentJ] = parentI;
            }
        }
    };
}

#endif // UNIONFIND_H
