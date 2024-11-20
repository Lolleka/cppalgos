#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include "utils.hpp"

namespace dmk{
	template<typename ITEM>
    class SimpleDoublyLinkedList{
        struct Node {
            ITEM item;
            Node *next, *prev;
            template<typename ARGUMENT>
            Node(ARGUMENT const& a): item(a), next(nullptr), prev(nullptr) {}
        } *root, *last;
        void cut(Node* n){
            assert(n);
            (n == last ? last : n->next->prev) = n->prev;
            (n == root ? root : n->prev->next) = n->next;
        }
    public:
        SimpleDoublyLinkedList(): root(nullptr), last(nullptr) {}
        template<typename ARGUMENT> void append(ARGUMENT const& a){
            Node* n = new Node(a);
            n->prev = last;
            if(last) last->next = n;
            last = n;
            if(!root) root = n;
        }
        class Iterator{
            Node* current;
        public:
            Iterator(Node* n): current(n) {}
            typedef Node* Handle;
            Handle getHandle(){return current;}
            Iterator& operator++(){
                assert(current);
                current = current->next;
                return *this;
            }
            Iterator& operator--(){
                assert(current);
                current = current->prev;
                return *this;
            }
            ITEM& operator*()const{assert(current); return current->item;}
            ITEM* operator->()const{assert(current); return &current->item;}
            bool operator==(Iterator const& rhs)const{return current == rhs.current;}
        };

        Iterator begin(){return Iterator(root);}
        Iterator rBegin(){return Iterator(last);}
        Iterator end(){return Iterator(0);}
        Iterator rEnd(){return Iterator(0);}
        void moveBefore(Iterator what, Iterator where){
            assert(what != end());
            if (what != where) { // first check for selfe reference
                Node *n = what.getHandle(), *w = where.getHandle();
                cut(n);
                n->next = w;
                if(w){
                    n->prev = w->prev;
                    w->prev = n;
                } else {
                    n->prev = last;
                    last = n;
                }
                if(n->prev) n->prev->next =n;
                if(w == root) root = n;
            }
        }
        template<typename ARGUMENT> void prepend(ARGUMENT const& a){
            append(a);
            moveBefore(rBegin(), begin());
        }
        void remove(Iterator what){
            assert(what != end());
            cut(what.getHandle());
            delete what.getHandle();
        }

        SimpleDoublyLinkedList(SimpleDoublyLinkedList const& rhs){
            for(Node* n = rhs.root; n; n = n->next){append(n->item);}
        }
        SimpleDoublyLinkedList& operator=(SimpleDoublyLinkedList const& rhs){
            return genericAssign(*this, rhs);
        }
        ~SimpleDoublyLinkedList(){
            while(root){
                Node* toBeDeleted = root;
                root = root->next;
                delete toBeDeleted;
            }
        }
	};
}

#endif // LINKEDLIST_H
