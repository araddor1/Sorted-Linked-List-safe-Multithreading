#include "ThreadSafeList.h"
#include <iostream>
#include <assert.h>
#include <ctime> // temporary for random tests

#define magicNumber 100             // TODO set the magic number as you wish
#define threadsNumber 20            // TODO set the number of threads that your PC can handle

using namespace std;

template <typename T>
void* start_routine(void* l) {
    List<T>* list = (List<T>*)l;

    int amountToInsert = rand()%magicNumber+ 1; //generate random number in range [1,magicNumber]
    int range1 = 2*magicNumber;    //keys inserted into the list are in range [1,2*magicNumber]

    int numOfinserts = 0;
    for(int i = 0 ; i<amountToInsert ; i++) {
        if(list->insert(T(rand()%range1+1))) {
            numOfinserts++;
        }
    }

    assert(list->isSorted());

                           //TODO remove comment if you want to print. ! will increase your tests time !

    int elementsToRemove = rand()%(numOfinserts+1); // choose randomly how many nodes to remove from list
    int removalCounter=0;
    for(int i = 0 ; i<elementsToRemove ; i++) {
        if(list->remove(T(rand()% range1 +1))) { // random number in range [1,magicNumber*2]
            removalCounter++;
        }
    }
    assert(list->isSorted());

    //list->print();                        //TODO remove comment if you want to print. ! will increase your tests time !

    return nullptr;
}

template <typename T>
void Test() {
    List<T>* list = new List<T>();
    pthread_t threads[threadsNumber];
    for(int i = 0 ; i <threadsNumber  ; ++i) {
        assert(pthread_create(threads+i, nullptr, start_routine<T>, list)==0);
    }
    for(int i = 0 ; i <threadsNumber  ; ++i) {
        assert(pthread_join(threads[i], nullptr)==0);
    }
}

/*int results[100][2];
pthread_mutex_t results_mutex;

struct Arguments {
    List<int>* l;
    int number;
    Arguments(List<int>* l, int number) : l(l), number(number) {}
};

void* start_routine(void* arguments) {
    Arguments* args = (Arguments*)arguments;
    bool ret = true;
    for(int i = 0 ; i < 100  ; i ++) {
        ret = args->l->insert(i);
        if(ret)
            pthread_mutex_lock(&results_mutex);
            results[i][1] = args->number;
            //cout << "num: " << i << "inserted by thread: " << args->number << endl;
            pthread_mutex_unlock(&results_mutex);

    }
    return nullptr;
}

void insert_test_behavior(List<int>* l){
    pthread_mutex_init(&results_mutex, nullptr);
    for(int i = 0 ; i <100  ; ++i) {
        results[i][0]=i;
        results[i][1]=-1;
    }
    /// tests
    Arguments arg0(l,20);
    Arguments arg1(l,1);
    Arguments arg2(l,2);
    pthread_t thread0;
    pthread_t thread1;
    pthread_t thread2;
    assert(pthread_create(&thread0, nullptr, start_routine, &arg0)==0);
    assert(pthread_create(&thread1, nullptr, start_routine, &arg1)==0);
    assert(pthread_create(&thread2, nullptr, start_routine, &arg2)==0);
    pthread_join(thread0, nullptr);
    pthread_join(thread1, nullptr);
    pthread_join(thread2, nullptr);

    //l->print();
    int histogram[3]={0,0,0};
    for(int i = 0 ; i <100  ; ++i) {
        histogram[results[i][1]]++;
        //cout << "num: " << i << " inserted by thread: " << results[i][1] << endl;
    }
    cout << "\n Histogram:\n            thread: __0__|__1__|__2__\n            inserts: _" << histogram[0] << "__|_" << histogram[1] << "__|_" << histogram[2] << "__\n" << endl;

    pthread_mutex_destroy(&results_mutex);
}*/

int main() {

    clock_t start = clock();
    srand(time(NULL));                          //for random numbers in tests

    cout << "Start testing ThreadSafeList" << endl;
    for(int i = 1 ; i<=50 ; i++) {                  // TODO set the number of tests you need
       std::cout << "Start Test number: " << i << std::endl;     //TODO remove comments if you want to print. ! will increase your tests time !
       std::cout
               << "########################################################################### \n"
               << std::endl;
        Test<int>();                                 //TODO choose the class T you want to test.
       std::cout << "Finish Test number: " << i << std::endl;      //TODO remove comments if you want to print. ! will increase your tests time !
       std::cout
               << "########################################################################### \n"
               << std::endl;
    }
    cout << "Finish testing ThreadSafeList" << endl;

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    double minutes = elapsed/60;
    cout << "Finished normally... hope for good" << endl;
    printf("\nTime elapsed: %.5f sec == %.5f min \n", elapsed, minutes);

  return 0;
}

/*
/// for testing only  // TODO: add this func to "ThreadSafeList.h" before test, dot forget to remove before submit
    bool isSorted(){
        pthread_mutex_lock(&dummy_mutex);
        if(!head) {
            pthread_mutex_unlock(&dummy_mutex);
            return true;
        }else{
            pthread_mutex_lock(&head->mutex);
            pthread_mutex_unlock(&dummy_mutex);
        }
        Node* prev = head;
        Node* curr = head->next;
        while(curr) {
            pthread_mutex_lock(&curr->mutex);
            if(prev->data >= curr->data) {
                pthread_mutex_unlock(&curr->mutex);
                pthread_mutex_unlock(&prev->mutex);
                return false;
            }
            pthread_mutex_unlock(&prev->mutex);
            prev = curr;
            curr = curr->next;
        }
        pthread_mutex_unlock(&prev->mutex);
        return true;
    }*/
