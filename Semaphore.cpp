#include "Semaphore.hpp"
Semaphore::Semaphore() {
    val = 0;
    pthread_cond_init(&my_cond , nullptr);
    pthread_mutex_init(&my_mutex, nullptr);
}

Semaphore::Semaphore(unsigned val) {
    this -> val = val;
    pthread_cond_init(&my_cond , nullptr);
    pthread_mutex_init(&my_mutex, nullptr);
}

Semaphore::~Semaphore() {
    pthread_cond_destroy(&my_cond);
    pthread_mutex_destroy(&my_mutex);
}

void Semaphore::up() {
    pthread_mutex_lock(&my_mutex);
    this->val++;
    pthread_cond_signal(&my_cond);
    pthread_mutex_unlock(&my_mutex);
}

void Semaphore::down() {
    pthread_mutex_lock(&my_mutex);
    while ( this->val == 0 ) {
        //cout << "wait for sem" << endl;
        pthread_cond_wait(&my_cond , &my_mutex);
    }
    this->val--;
    //cout << "sem down succeed" << endl;
    pthread_mutex_unlock(&my_mutex);
}









