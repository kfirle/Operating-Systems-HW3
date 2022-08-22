#ifndef _QUEUEL_H
#define _QUEUEL_H
#include "Headers.hpp"
#include "Semaphore.hpp"
// Single Producer - Multiple Consumer queue
template <typename T>
class PCQueue
{
public:
    PCQueue(){

        this->queue = new std::queue<T>;
        this->semaphore = new Semaphore();
        producer_in_queue = false;
        pthread_cond_init(&prod_cond, nullptr);
        pthread_mutex_init(&global_mutex, nullptr);
    };
    ~PCQueue(){
        delete semaphore;
        delete queue;
        pthread_cond_destroy(&prod_cond);
        pthread_mutex_destroy(&global_mutex);
    };

    // Blocks while queue is empty. When queue holds items, allows for a single
    // thread to enter and remove an item from the front of the queue and return it.
    // Assumes multiple consumers.
    T pop(){
        this->semaphore->down();
        pthread_mutex_lock(&global_mutex);
        while(producer_in_queue){
            pthread_cond_wait(&prod_cond , &global_mutex);
        }
        T& return_val = this->queue->front();
        this->queue->pop();
        pthread_cond_signal(&prod_cond);
        pthread_mutex_unlock(&global_mutex );
        return return_val;
    };

    // Allows for producer to enter with *minimal delay* and push items to back of the queue.
    // Hint for *minimal delay* - Allow the consumers to delay the producer as little as possible.
    // Assumes single producer
    void push(const T& item){
        producer_in_queue = true;
        pthread_mutex_lock(&global_mutex);
        this->queue->push(item);
        this->semaphore->up();
        producer_in_queue = false;
        pthread_cond_signal(&prod_cond);
        pthread_mutex_unlock(&global_mutex);
    }


private:
    std::queue<T>* queue;
    Semaphore* semaphore;
    pthread_mutex_t global_mutex;
    bool producer_in_queue;
    pthread_cond_t prod_cond;
};
// Recommendation: Use the implementation of the std::queue for this exercise
#endif