#include "PCQueue.hpp"

template <typename T>
PCQueue<T>::PCQueue() {

    this->global_mutex = nullptr;
    this->semaphore = new Semaphore();
    this->queue = new std::queue<T>;
    pthread_mutex_init(&global_mutex, nullptr);
}

template <typename T>
PCQueue<T>::~PCQueue(){
    delete semaphore;
    delete queue;
}

template <typename T>
T PCQueue<T>::pop() {

    this->semaphore->down();
    //pthread_mutex_lock( &global_mutex );
    T& return_val = this->queue.front();
    this->queue.pop();
    //pthread_mutex_unlock( &global_mutex );
    return return_val;
}

template <typename T>
PCQueue<T>::push(const T& item) {

    pthread_mutex_lock(&global_mutex);
    this->queue.push(item);
    pthread_mutex_unlock(&global_mutex);
    this->semaphore->up();
}