#ifndef FREELIST_H
#define FREELIST_H

#include "utils.hpp"
#include "linkedlist.hpp"
#include "vector.hpp"

namespace dmk{
	template<typename ITEM>
    struct StaticFreelist{
        int capacity, size, maxSize; // size <= maxSize <= capacity
                                     //
        struct Item{
            ITEM item;
            union {
                Item* next; // used when empty cell
                void* userData; // used when allocated
            };
        } *nodes, *returned;

        StaticFreelist(int fixedSize):
            capacity(fixedSize),
            size(0),
            maxSize(0),
            returned(nullptr),
            nodes(rawMemory<Item>(fixedSize)){}
        bool isFull(){return size == capacity;}
        bool isEmpty(){return size <= 0;}

        Item* allocate(){
            assert(!isFull());
            Item* result = returned;
            if(result) returned = returned->next;
            else result = &nodes[maxSize++];
            ++size;
            return result;
        }

        void remove(Item* item){
            // item must come from this list
            assert(item - nodes >= 0 && item - nodes < maxSize);
            item->item.~ITEM();
            item->next = returned;
            returned = item;
            --size;
        }

        ~StaticFreelist(){
            if(!isEmpty()){
                Vector<bool> toDestruct(maxSize, true);
                while (returned){
                    toDestruct[returned - nodes] = false;
                    returned = returned->next;
                }
                for(int i =0; i < maxSize; ++i)
                    if (toDestruct[i]) nodes[i].item.~ITEM();
            }
            rawDelete(nodes);
        }
	};

    template<typename ITEM>
    class Freelist{
        enum{MAX_BLOCK_SIZE = 8192, MIN_BLOCK_SIZE = 9, DEFAULT_SIZE = 32};
        int blockSize;
        typedef SimpleDoublyLinkedList<StaticFreelist<ITEM> > ListType;
        typedef typename StaticFreelist<ITEM>::Item Item;
        typedef typename ListType::Iterator I;
        ListType blocks;
        Freelist(Freelist const&);
        Freelist& operator=(Freelist const&);
    public:
        Freelist(int initialSize = DEFAULT_SIZE) :
            blockSize(std::max<int>(MIN_BLOCK_SIZE,
                          std::min<int>(initialSize, MAX_BLOCK_SIZE))) {}
        ITEM* allocate(){
            I first = blocks.begin();
            if(first == blocks.end() || first->isFull()){
                //make new first block if needed
                blocks.prepend(blockSize);
                first = blocks.begin();
                blockSize = std::min<int>(blockSize * 2, MAX_BLOCK_SIZE);
            }
            // request new allocation from first block
            Item* result = first->allocate();
            // block list pointer stored as user data
            result->userData= (void*)first.getHandle();
            //move full blocks to the end
            if(first->isFull()) blocks.moveBefore(first, blocks.end());
            return (ITEM*)result;
        }

        void remove(ITEM* item){
            // NOTE: undefined behavior if item not form this list
            if(!item) return; // handle null pointer
            Item* node = (Item*) (item); // cast back from first member
            // retreive original block pointer
            I cameFrom((typename I::Handle)node->userData);
            cameFrom->remove(node);
            if(cameFrom->isEmpty()){
                // delete block if empty, else reduce its size
                // beware that block boundary delete/remove thrashes, but unlikely
                blockSize = std::max<int>(MIN_BLOCK_SIZE, blockSize - cameFrom->capacity);
                blocks.remove(cameFrom);
            } // move available blocks to the front
            else blocks.moveBefore(cameFrom, blocks.begin());

        }
    };
}

#endif // FREELIST_H
