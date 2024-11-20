// Credits: Dmitro Kedyk
#ifndef SPARSE_H
#define SPARSE_H

#include "utils.hpp"
#include "vector.hpp"
#include "sorting.hpp"
#include <cassert>
#include <algorithm>
#include <cmath>

namespace dmk{

template<typename ITEM = double>
class SparseMatrix: public ArithmeticType<SparseMatrix<ITEM> >
{
public:
    typedef std::pair<int, ITEM> Item;
    typedef Vector<Item> SparseVector;
private:
    int rows;
    Vector<SparseVector> itemColumns;
    int findPosition(int r, int c) const {
        assert(0 <= r && r < rows && 0 <= c && c < getColumns());
        SparseVector const& column = itemColumns[c];
        if (column.getSize() == 0) return -1;
        return binarySearch(column.getArray(), 0, column.getSize() - 1,
                            Item(r, 0), PairFirstComparator<int, ITEM>());
    }
public:
    SparseMatrix(int theRows, int theColumns): rows(theRows), itemColumns(theColumns){}

    int getRows() const{return rows;}

    int getColumns() const{return itemColumns.getSize();}

    void growRight(int cols){
        for (int i = 0; i<cols; ++i)
            itemColumns.append(SparseVector(rows));
    }

    void growLeft(int cols){
        itemColumns.reverse();
        growRight(cols);
        itemColumns.reverse();
    }

    void growBottom(int newRows){
        rows += newRows;
    }

    void growTop(int newRows){
        growBottom(newRows);
        // update sparse elements row position
        for(int c=0; c < getColumns(); ++c){
            for(int j=0; j < itemColumns[c].getSize(); ++j)
                itemColumns[c][j].first += newRows;
        }
    }

    SparseVector const& getColumn(int c) const
    { //absent entries are 0
        return itemColumns[c];
    }

    ITEM operator()(int r, int c)const
    { //absent entries are 0
        int position = findPosition(r, c);
        return position == -1 ? 0 : itemColumns[c][position].second;
    }

    void set(int r, int c, ITEM const& item)
    { // first try to find and update
        SparseVector& column = itemColumns[c];
        int position = findPosition(r, c);
        if (position != -1) column[position].second = item;
        else if(item != 0) { 
            // if can't need to do vector insertion binarySearch
            // shifting down the rest of the column
            Item temp(r, item);
            column.append(temp);
            position = column.getSize() - 1;
            for(; position > 0 && column[position - 1].first > r; --position)
                column[position] = column[position - 1];
            column[position] = temp;
        }
    }

    static SparseVector addSparseVectors(SparseVector const& a, SparseVector const& b){
        // take element from both, adding where both exist
        SparseVector result;
        for(int aj = 0, bj = 0; aj < a.getSize() || bj < b.getSize();){
            bool considerBoth = aj < a.getSize() && bj < b.getSize();
            int j = aj < a.getSize() ? a[aj].first : b[bj].first;
            ITEM item = aj < a.getSize() ? a[aj++].second : b[bj++].second;
            if (considerBoth){ // made init with a, now consider b
                if (j == b[bj].first) item += b[bj++].second;
                else if (j > b[bj].first){
                    j = b[bj].first;
                    --aj; // undo aj increment
                    item = b[bj++].second;
                }
            }
            if (item != 0) result.append(Item(j, item)); // just in case
        }
        return result;
    }

    SparseMatrix& operator+= (SparseMatrix const& rhs){
        // add column-by-column
        assert(rows == rhs.rows && getColumns() == rhs.getColumns());
        SparseMatrix result(rows, getColumns());
        for(int c=0; c < getColumns(); ++c)
            result.itemColumns[c] = addSparseVectors(itemColumns[c],
                                                     rhs.itemColumns[c]);
        return *this = result;
    }
	
    SparseMatrix& operator*= (ITEM a){
        // add column-by-column
        for(int c=0; c < getColumns(); ++c)
            for(int j=0; j < itemColumns[c].getSize(); ++j)
                itemColumns[c][j].second *= a;
        return *this;
    }

    friend SparseMatrix operator*(SparseMatrix const& A, ITEM a){
        SparseMatrix result(A);
        return result *= a;
    }

    SparseMatrix operator-()const
    {
        SparseMatrix result(*this);
        return result *= -1;
    }

    SparseMatrix& operator-=(SparseMatrix const& rhs){
        return *this += -rhs;
    }
    
    static SparseMatrix& identity(int n){
        SparseMatrix result(n, n);
        for(int c = 0; c < n; ++c) result.item[c].append(Item(c, 1));
        return result;
    }

    void clear(){
        for(int c = 0; c < getColumns(); ++c) itemColumns[0].clear();
    }

    static ITEM dotSparseVectors(SparseVector const& a, SparseVector const& b){
        // add to sum when both present
        ITEM result = 0;
        for (int aj = 0, bj = 0; aj < a.getSize() && bj < b.getSize();)
            if(a[aj].first == b[bj].first)
                result += a[aj++].second * b[bj++].second;
            else if(a[aj].first < b[bj].first) ++aj;
            else ++bj;
        return result;
    }

    static Vector<ITEM> sparseToDense(SparseVector const& sv, int n){
        // need n because we don't know sparse tail
        assert(sv.getSize() == 0 || sv[sv.getSize() - 1].first < n);
        Vector<ITEM> v(n);
        for(int i = 0; i < sv.getSize(); ++i) v[sv[i].first] = sv[i].second;
        return v;
    }

    static SparseVector denseToSparse(Vector<ITEM> const& v){
        SparseVector sv;
        for(int i = 0; i < v.getSize(); ++i)
            if(v[i] != 0) sv.append(Item(i, v[i]));
        return sv;
    }

    friend SparseVector operator*(SparseVector const& v, SparseMatrix const& A){
        // SparseVector * SparseMatrix mult
        assert(v.getSize() == 0 || v.lastItem().first < A.getRows());
        SparseVector result;
        for(int c = 0; c < A.getColumns(); ++c){
            // add one row at a time
            ITEM rc = dotSparseVectors(v, A.itemColumns[c]);
            if(rc != 0) result.append(Item(c, rc));
        }
        return result;
    }

    friend SparseVector operator*(SparseMatrix const& A, SparseVector const& v)
        {return v * A.transpose();}

    friend Vector<ITEM> operator*(Vector<ITEM> const& v, SparseMatrix const& A)
        {return sparseToDense(denseToSparse(v) * A, A.rows);}

    friend Vector<ITEM> operator*(SparseMatrix const& A, Vector<ITEM> const& v)
        {return sparseToDense(denseToSparse(v) * A, A.rows);}

    friend double normInf(SparseMatrix const& A){
        // first canlculate transpose for better iteration
        SparseMatrix AT = A.transpose();
        double maxRowSum = 0;
        for (int r = 0; r < A.getRows(); r++) {
            double rSum = 0;
            for (int cj = 0; cj < AT.itemColumns[cj].getSize(); cj++) {
               rSum += std::abs(AT.itemColumns[r][cj].second);
            }
            maxRowSum = std::max(maxRowSum, rSum);
        }
    }

    SparseMatrix transpose() const
    {
        SparseMatrix result(getColumns(), rows);
        for (int c = 0; c < getColumns(); c++) {
            for (int j = 0; j < itemColumns[c].getSize(); j++) {
                result.itemColumns[itemColumns[c][j].first].append(
                    Item(c, itemColumns[c][j].second));
            } 
        }
        return result;
    }

    /*SparseMatrix& operator*=(SparseMatrix const& rhs){*/
    /*    // O(n^2) * space factor() (1 to n)*/
    /*    assert(getColumns() == rhs.rows);*/
    /*    SparseMatrix result(rows, rhs.getColumns()), bT = rhs.transpose();*/
    /*    typedef typename Key2DBuilder<>::WORD_TYPE W;*/
    /*    LinearProbingHashTable<W, ITEM> outerSums;*/
    /*    Key2DBuilder<> kb(std::max(result.rows, result.getColumns()),*/
    /*                      result.rows >= result.getColumns());*/
    /*    for (int k = 0; k < rhs.rows; ++k) {*/
    /*        for (int aj = 0; aj < itemColumns[k].getSize(); ++aj) {*/
    /*            for (int btj = 0; btj < bT.itemColumns[k].getSize(); ++btj) {*/
    /*                int r = itemColumns[k][aj].first,*/
    /*                    c = bT.itemColumns[k][btj].first;*/
    /*                W key = kb.to1D(r, c);*/
    /*                ITEM* rcSum = outerSums.find(key),*/
    /*                      rcValue = itemColumns[k][aj].second * bT.itemColumns[k][btj];*/
    /*                if(rcSum) *rcSum = rcValue;*/
    /*                else outerSums.insert(key, rcValue);*/
    /*            }*/
    /*        }*/
    /*    }*/
    /*    // convert outer sum hash table into final data structure*/
    /*    for(typename LinearProbingHashTable<W, ITEM>::Iterator iter =*/
    /*        outerSums.begin(); iter != outerSums.end(); ++iter){*/
    /*        // n must be the larger of r, c to make sense!*/
    /*        std::pair<unsigned int, unsigned int> rc = kb.to2D(iter->key);*/
    /*        result.itemColumns[rc.second].append(Item(rc.first, iter->value));*/
    /*    }*/
    /*    // sort each column to fix order*/
    /*    for (int c = 0; c < result.getColumns(); ++c) quickSort(*/
    /*        result.itemColumns[c].getArray(), 0,*/
    /*        result.itemColumns[c].getSize() - 1,*/
    /*        PairFirstComparator<int, ITEM>());*/
    /*    return *this = result;*/
    /*}*/
};

}
#endif // SPARSE_H
