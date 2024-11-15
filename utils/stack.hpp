#ifndef STACK_H
#define STACK_H

#include "vector.hpp"

namespace dmk{

	template<typename ITEM, typename VECTOR = Vector<ITEM> >
    struct Stack{
        VECTOR storage;
        void push(ITEM const& item){storage.append(item);}

        ITEM pop(){
            assert(!isEmpty());
            ITEM result = storage.lastItem();
            storage.removeLast();
            return result;
        }

        ITEM& getTop(){
            assert(!isEmpty());
            return storage.lastItem();
        }
        
        bool isEmpty(){return !storage.getSize();}
	};

}

#endif // STACK_H
