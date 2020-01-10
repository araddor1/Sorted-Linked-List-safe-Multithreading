#ifndef THREAD_SAFE_LIST_H_
#define THREAD_SAFE_LIST_H_

#include <pthread.h>
#include <iostream>
#include <iomanip> // std::setw

using namespace std;

template <typename T>
class List 
{
    public:
        /**
         * Constructor
         */
        List():size(0) { 
          pthread_mutex_init(&list_mutex,NULL);
          head=nullptr;
         }

        /**
         * Destructor
         */
        ~List(){
          Node* curr=head;
          Node* temp=nullptr;
          while (curr !=nullptr){
            temp=curr;
            delete curr;
            curr=temp->next;
          }
          pthread_mutex_destroy(&list_mutex);
        }

        class Node {
         public:
          Node(const T& data):data(data),next(nullptr){
            pthread_mutex_init(&this->node_mutex,NULL);
          };
          T data;
          Node *next;
          pthread_mutex_t node_mutex;
          ~Node(){
            pthread_mutex_destroy(&node_mutex);
          }
        };

        /**
         * Insert new node to list while keeping the list ordered in an ascending order
         * If there is already a node has the same data as @param data then return false (without adding it again)
         * @param data the new data to be added to the list
         * @return true if a new node was added and false otherwise
         */
        bool insert(const T& data) {
          Node* new_n=new Node(data);
          // if list is empty / new data is the smallest
          if (head == nullptr || data < head->data ){
            pthread_mutex_lock(&(head->node_mutex));
            new_n->next = head;
            head = new_n;
            size++;
            __add_hook(); 
             pthread_mutex_unlock(&(head->node_mutex));
            return true;
          }
          
          Node* prev = head;
          pthread_mutex_lock(&(prev->node_mutex));
          if (head->next == nullptr && head->data == data){
                pthread_mutex_unlock(&(prev->node_mutex));
                  return false;
          }
          Node* curr = head->next;
          pthread_mutex_lock(&(curr->node_mutex));

          while (curr !=nullptr &&   curr->data < data){
            pthread_mutex_unlock(&(prev->node_mutex));  
            prev = curr;
              if (curr->next == nullptr){
                curr =curr->next;
                break;
              }
            curr =curr->next;
            pthread_mutex_lock(&(curr->node_mutex)); 
          }

          if (curr != nullptr && (curr->data == data || prev->data == data) ){
            pthread_mutex_unlock(&(curr->node_mutex));
            pthread_mutex_unlock(&(prev->node_mutex));  
            return false;
          }
          prev->next =new_n;
          new_n->next = curr;
          size++; 
          __add_hook();
          pthread_mutex_unlock(&(curr->node_mutex));
          pthread_mutex_unlock(&(prev->node_mutex));  
          return true;   
        }

        /**
         * Remove the node that its data equals to @param value
         * @param value the data to lookup a node that has the same data to be removed
         * @return true if a matched node was found and removed and false otherwise
         */
        bool remove(const T& value) {
          Node* prev = head;
          pthread_mutex_lock(&(prev->node_mutex));
          Node* curr = head->next;
          pthread_mutex_lock(&(curr->node_mutex));

          while (curr->next != nullptr && curr->data <= value){
              if (curr->data == value){
                prev->next = curr->next;
                size--;
                pthread_mutex_unlock(&(curr->node_mutex));
                pthread_mutex_unlock(&(prev->node_mutex)); 
                delete curr;
                __remove_hook();
                return true;
            }
            pthread_mutex_unlock(&(prev->node_mutex));  
            prev = curr;
            curr =curr->next;
            pthread_mutex_lock(&(curr->node_mutex)); 
          }
        pthread_mutex_unlock(&(curr->node_mutex));
        pthread_mutex_unlock(&(prev->node_mutex)); 
        return false;
        }

        /**
         * Returns the current size of the list
         * @return the list size
         */
        unsigned int getSize() {
          return size;
        }

		// Don't remove
        void print() {
          pthread_mutex_lock(&list_mutex);
          Node* temp = head;
          if (temp == NULL)
          {
            cout << "";
          }
          else if (temp->next == NULL)
          {
            cout << temp->data;
          }
          else
          {
            while (temp != NULL)
            {
              cout << right << setw(3) << temp->data;
              temp = temp->next;
              cout << " ";
            }
          }
          cout << endl;
          pthread_mutex_unlock(&list_mutex);
        }

		// Don't remove
        virtual void __add_hook() {}
		// Don't remove
        virtual void __remove_hook() {}

    private:
        Node* head;
        int size;
        pthread_mutex_t list_mutex;

};

#endif //THREAD_SAFE_LIST_H_b