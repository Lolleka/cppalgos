#ifndef SPARSE_H
#define SPARSE_H

#include "utils.hpp"
#include "vector.hpp"
#include "sorting.hpp"
#include <cassert>

namespace dmk{

template<typename ITEM = double>
class SparseMatrix: public ArithmeticType<SparseMatrix<ITEM> >
{
    int rows;
    typedef std::pair<int, ITEM> Item;
    typedef Vector<Item> SparseVector;
    Vector<SparseVector> itemColumns;
    int findPosition(int r, int c) const {
        assert(0 <= r && r < rows && 0 <= c && c < getColumns());
        SparseVector const& column = itemColumns[c];
        return binarySearch(column.getArray(), 0, column.getSize() - 1,
                            Item(r, 0), PairFirstComparator<int, ITEM>());
    }
public:
    SparseMatrix(int theRows, int theColumns): row(theRows), itemColumns(theColumns){}

    int getRows() const{return rows;}

    int getColumns() const{return itemColumns.getSize();}

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
            if (considerBoth) // made init with a, now consider b
                if (j == b[bj].first) item += b[bj++].second;
                else if (j > b[bj].first){
                    j = b[bj].first;
                    --aj; // undo aj increment
                    item = b[bj++].second;
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

    static Vector<ITEM> sparseToDense(SparseVector const& sb, int n){
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

    friend SparseVector operator*(SparseVector const& v, SpareMatric const& A){
        assert(v.getSize() == 0 || v.lastItem().first < A.getRows());
        SparseVector result;
        for(int c = 0; c < A.getColumns(); ++c){
            // add one row at a time
            ITEM rc = dotSparseVectors(v, A.itemColumns[c]);
            if(rc != 0) result.append(Item(c, rc));
        }
        return result;
    }

};

}
#endif // SPARSE_H
