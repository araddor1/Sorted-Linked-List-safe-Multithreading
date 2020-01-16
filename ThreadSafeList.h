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
        List() { 
          pthread_mutex_init(&list_mutex,NULL);
          head=nullptr;
         }

        /**
         * Destructor
         */
        ~List(){
          Node* curr=head;
          Node* next=nullptr;
          while (curr !=nullptr){
            next=curr->next;
            delete curr;
            curr=next;;
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
          
          
           pthread_mutex_lock(&(list_mutex));

           // if list is empty 
           if (head ==0){
              head = new Node(data);
              __add_hook(); 
              pthread_mutex_unlock(&(list_mutex));
              return true;
           }

           // if list not empty 
            pthread_mutex_lock(&(head->node_mutex));
            
            Node* prev=head;
            Node* curr=head->next;

            // if head is only node | new data is before head
            if (curr == 0 || head->data >= data){
                if (head->data > data){
                  Node* temp =new Node(data);
                  temp->next = prev;
                  head =  temp;
                   __add_hook(); 
                  pthread_mutex_unlock(&(prev->node_mutex));
                  pthread_mutex_unlock(&(list_mutex));
                  return true;
              }
              if (head->data == data){
                pthread_mutex_unlock(&(prev->node_mutex));
                pthread_mutex_unlock(&(list_mutex));
                return false;
              }
              if (head->data < data){
                head->next =  new Node(data);
                 __add_hook(); 
                pthread_mutex_unlock(&(prev->node_mutex));
                pthread_mutex_unlock(&(list_mutex));
                return true; 
              }
            }
            pthread_mutex_unlock(&(list_mutex));

            // traveling the list
            while(curr!=0){
                     pthread_mutex_lock(&(curr->node_mutex));
                     // if new data is already in 
                    if (curr->data ==data){
                      pthread_mutex_unlock(&(curr->node_mutex));
                      pthread_mutex_unlock(&(prev->node_mutex));
                      return false;
                    }
                    // if  new data is smaller then curr and greater than prev 
                    if (curr->data > data){
                      prev->next = new Node(data);
                      prev->next->next = curr;
                      __add_hook(); 
                      pthread_mutex_unlock(&(curr->node_mutex));
                      pthread_mutex_unlock(&(prev->node_mutex));
                      return true;
                    }
                    pthread_mutex_unlock(&(prev->node_mutex));
                    prev=curr;
                    curr=curr->next;
            }

            // if  new data is the largest
                prev->next = new Node(data);
                prev->next->next = nullptr;
                __add_hook(); 
                pthread_mutex_unlock(&(prev->node_mutex));
                return true;
          } 

        /**
         * Remove the node that its data equals to @param value
         * @param value the data to lookup a node that has the same data to be removed
         * @return true if a matched node was found and removed and false otherwise
         */
        bool remove(const T& value) {
          pthread_mutex_lock(&(list_mutex));
           
           // if  list empty
          if (head == 0){
              pthread_mutex_unlock(&(list_mutex));
              return false;
          }
          pthread_mutex_lock(&(head->node_mutex));
          Node* prev = head;     
          Node* curr = head->next;

          // if  head need to be deleted
          if (head->data == value ){
             pthread_mutex_unlock(&(prev->node_mutex));
              head = curr;
              delete prev;
               __remove_hook();
              pthread_mutex_unlock(&(list_mutex));
            return true;
          }
          pthread_mutex_unlock(&(list_mutex));

          // traveling the list
          while (curr != 0){
                   pthread_mutex_lock(&(curr->node_mutex));
                if (curr->data == value){
                    prev->next = curr->next;
                    __remove_hook();
                    pthread_mutex_unlock(&(curr->node_mutex));
                    delete curr;
                    pthread_mutex_unlock(&(prev->node_mutex)); 
                    return true;
                }
            pthread_mutex_unlock(&(prev->node_mutex));  
            prev = curr;
            curr =curr->next;  
          }
        if (curr !=0 )pthread_mutex_unlock(&(curr->node_mutex));
        if (prev !=0 ) pthread_mutex_unlock(&(prev->node_mutex)); 
        return false;
        }

        /**
         * Returns the current size of the list
         * @return the list size
         */
        unsigned int getSize() {
          pthread_mutex_lock(&(list_mutex));
          int num=0;
          Node* curr=head;
          Node* next=nullptr;
          while (curr !=nullptr){
            next=curr->next;
            curr=next;
            num++;
          }
          pthread_mutex_unlock(&(list_mutex));
          return num;
        }

  bool isSorted(){
        pthread_mutex_lock(&list_mutex);
        if(!head) {
            pthread_mutex_unlock(&list_mutex);
            return true;
        }else{
            pthread_mutex_lock(&head->node_mutex);
            pthread_mutex_unlock(&list_mutex);
        }
        Node* prev = head;
        Node* curr = head->next;
        while(curr) {
            pthread_mutex_lock(&curr->node_mutex);
            if(prev->data >= curr->data) {
                pthread_mutex_unlock(&curr->node_mutex);
                pthread_mutex_unlock(&prev->node_mutex);
                return false;
            }
            pthread_mutex_unlock(&prev->node_mutex);
            prev = curr;
            curr = curr->next;
        }
        pthread_mutex_unlock(&prev->node_mutex);
        return true;
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
        pthread_mutex_t list_mutex;

};

#endif //THREAD_SAFE_LIST_H_b