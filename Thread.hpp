#ifndef __THREAD_H
#define __THREAD_H

#define NUMBER_OF_SPECIES 7
#define PHASE_ONE 1
#define PHASE_TWO 2
#define PHASE_THREE 3
#include "Headers.hpp"
#include "Game.hpp"
#include "PCQueue.hpp"
class Thread
{
public:
    Thread(uint thread_id)
    {
        // Only places thread_id
        this-> m_thread_id = thread_id;
    }
    virtual ~Thread() {} // Does nothing

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    // Creates the internal thread via pthread_create
    bool start()
    {
     int ret = pthread_create( &m_thread, nullptr, *entry_func, this);
        return ret == 0;
    }

    /** Will not return until the internal thread has exited. */
    void join()
    {
        pthread_join( m_thread, nullptr);
    }

    /** Returns the thread_id **/
    uint thread_id()
    {
        return m_thread_id;
    }
protected:
    // Implement this method in your subclass with the code you want your thread to run.
    virtual void thread_workload() = 0;
    uint m_thread_id; // A number from 0 -> Number of threads initialized, providing a simple numbering for you to use

private:
    static void * entry_func(void * thread) { ((Thread *)thread)->thread_workload(); return NULL; }
    pthread_t m_thread;
};

class Job {
    // All here are derived from ARGV, the program's input parameters.
public:
    unsigned int start_row;
    unsigned int end_row;
    unsigned int width;
    unsigned int height;
    int phase;
    Job(unsigned int start_row1,unsigned int end_row1 ,unsigned int width1,unsigned int height1 , int phase1) :
            start_row(start_row1), end_row(end_row1) , width(width1) , height(height1) ,
            phase(phase1){}
};


class PhaseWork : public Thread {

   // Job* job;
    int_mat* current;
    int_mat* next;
    PCQueue<Job>* pcq;
    int* job_counter;
    pthread_mutex_t* job_mutex;
    pthread_cond_t* job_cond;
    vector<double>* m_tile_hist;

public:

    PhaseWork(uint thread_id1, int_mat* current1, int_mat* next1 ,PCQueue<Job>* pcq1,int* job_counter1
            ,pthread_mutex_t* job_mutex1 , pthread_cond_t* job_cond1 , vector<double>* m_tile_hist1) :Thread (thread_id1) {
//    this->job = nullptr;
    this->next = next1;
    this->current = current1;
    this->pcq = pcq1;
    this->job_counter = job_counter1;
    this->job_mutex = job_mutex1;
    this->job_cond = job_cond1;
    this->m_tile_hist = m_tile_hist1;
    };


    void thread_workload() override {
        while (true) {
            //cout << "loop " << this->m_thread_id << endl;
            Job job = pcq->pop();
           // this->job = &(pcq->pop());
            if (job.phase == PHASE_THREE) {
                (*this->job_counter)++;
                //  delete job;
                break;
            }
            auto tile_start = std::chrono::system_clock::now();
            int_mat *temp;
            if (job.phase == PHASE_TWO) {
                temp = next;    //Swap pointers
                next = current;
                current = temp;
            }
            for (unsigned int i = job.start_row; i <= job.end_row; i++) {
                for (unsigned int j = 0; j < job.width; j++) {
                    unsigned int alive_species = 0;
                    unsigned int species_hist[] = {0, 0, 0, 0, 0, 0, 0, 0};
                    if (this->current->at(i).at(j) % 8 == 0 && job.phase == PHASE_TWO) {
                        this->next->at(i).at(j) = this->current->at(i).at(j);
                    } else {
                        if (j == 0) {
                            //left column
                            if (i == 0) {
                                //upper-left cell
                                if (job.height != 1 && job.width == 1) {
                                    if (this->current->at(i + 1).at(j) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i + 1).at(j) % 8]++;
                                }
                                if (job.height == 1 && job.width != 1) {
                                    if (this->current->at(i).at(j + 1) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i).at(j + 1) % 8]++;
                                }
                                if (job.height != 1 && job.width != 1) {
                                    if (this->current->at(i).at(j + 1) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i).at(j + 1) % 8]++;

                                    if (this->current->at(i + 1).at(j + 1) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i + 1).at(j + 1) % 8]++;
                                    if (this->current->at(i + 1).at(j) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i + 1).at(j) % 8]++;
                                }
                            } else {
                                if (i == job.height - 1) {
                                    //lower-left cell
                                    if (job.width != 1) {
                                        if (this->current->at(i - 1).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j + 1) % 8]++;
                                        if (this->current->at(i).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j + 1) % 8]++;
                                    }
                                    if (this->current->at(i - 1).at(j) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i - 1).at(j) % 8]++;
                                } else {
                                    //regular left column cell
                                    if (job.width != 1) {
                                        if (this->current->at(i - 1).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j + 1) % 8]++;
                                        if (this->current->at(i).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j + 1) % 8]++;
                                        if (this->current->at(i + 1).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j + 1) % 8]++;
                                    }
                                    if (this->current->at(i - 1).at(j) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i - 1).at(j) % 8]++;
                                    if (this->current->at(i + 1).at(j) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i + 1).at(j) % 8]++;
                                }
                            }
                        } else {
                            //j != 0
                            if (j == job.width - 1) {
                                //right column
                                if (i == 0) {
                                    //upper-right cell
                                    if (this->current->at(i).at(j - 1) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i).at(j - 1) % 8]++;
                                    if (job.height != 1) {
                                        if (this->current->at(i + 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j - 1) % 8]++;
                                        if (this->current->at(i + 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j) % 8]++;
                                    }
                                } else {
                                    if (i == job.height - 1) {
                                        //lower-left cell
                                        if (this->current->at(i - 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j - 1) % 8]++;
                                        if (this->current->at(i).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j - 1) % 8]++;
                                        if (this->current->at(i - 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j) % 8]++;
                                    } else {
                                        //regular right column cell
                                        if (this->current->at(i - 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j - 1) % 8]++;
                                        if (this->current->at(i).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j - 1) % 8]++;
                                        if (this->current->at(i + 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j - 1) % 8]++;
                                        if (this->current->at(i - 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j) % 8]++;
                                        if (this->current->at(i + 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j) % 8]++;
                                    }
                                }
                            } else {
                                //j != 0 && j != (width - 1)
                                if (i == 0) {
                                    //regular first row cell
                                    if (this->current->at(i).at(j + 1) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i).at(j + 1) % 8]++;
                                    if (this->current->at(i).at(j - 1) % 8 != 0) {
                                        alive_species++;
                                    }
                                    species_hist[this->current->at(i).at(j - 1) % 8]++;
                                    if (job.height != 1) {
                                        if (this->current->at(i + 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j - 1) % 8]++;
                                        if (this->current->at(i + 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j) % 8]++;
                                        if (this->current->at(i + 1).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j + 1) % 8]++;
                                    }
                                } else {
                                    if (i == job.height - 1) {
                                        //regular last row cell
                                        if (this->current->at(i).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j + 1) % 8]++;
                                        if (this->current->at(i).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j - 1) % 8]++;
                                        if (this->current->at(i - 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j - 1) % 8]++;
                                        if (this->current->at(i - 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j) % 8]++;
                                        if (this->current->at(i - 1).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j + 1) % 8]++;
                                    } else {
                                        //regular cell
                                        if (this->current->at(i - 1).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j + 1) % 8]++;
                                        if (this->current->at(i).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j + 1) % 8]++;
                                        if (this->current->at(i + 1).at(j + 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j + 1) % 8]++;
                                        if (this->current->at(i - 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j - 1) % 8]++;
                                        if (this->current->at(i).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i).at(j - 1) % 8]++;
                                        if (this->current->at(i + 1).at(j - 1) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j - 1) % 8]++;
                                        if (this->current->at(i - 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i - 1).at(j) % 8]++;
                                        if (this->current->at(i + 1).at(j) % 8 != 0) {
                                            alive_species++;
                                        }
                                        species_hist[this->current->at(i + 1).at(j) % 8]++;
                                    }
                                }
                            }
                        }
                        //finished checking neighbors

                        //phase1
                        if (job.phase == PHASE_ONE) {
                            unsigned int dominant_species = NUMBER_OF_SPECIES;
                            for (unsigned int k = NUMBER_OF_SPECIES; k > 0; k--) {
                                if (species_hist[k] * k >= species_hist[dominant_species] * dominant_species) {
                                    dominant_species = k;
                                }
                            }
                            if (alive_species == 3 && this->current->at(i).at(j) == 0) //try
                            {
                                this->next->at(i).at(j) = dominant_species;
                            } else {
                                if (alive_species == 2 || alive_species == 3) {
                                    if (this->current->at(i).at(j) % 8 != 0) {
                                        this->next->at(i).at(j) = this->current->at(i).at(j);
                                    }
                                } else {
                                    this->next->at(i).at(j) = 0;
                                }
                            }
                        }
                        //phase2
                        if (job.phase == PHASE_TWO) {
                            //adding the cell to the calculation
                            species_hist[this->current->at(i).at(j) % 8]++;
                            alive_species++;
                            //find average
                            double sum = 0;
                            for (unsigned int m = 1; m <= NUMBER_OF_SPECIES; m++) {
                                sum += species_hist[m] * m;
                            }
                            sum = sum / alive_species;
                            this->next->at(i).at(j) = std::round(sum);
                        }
                    }
                }
            }
            if (job.phase == PHASE_TWO){
                temp = next; //Swap pointers back
                next = current;
                current = temp;
            }
            auto tile_end = std::chrono::system_clock::now();
            pthread_mutex_lock(this->job_mutex);
            m_tile_hist->push_back((double) std::chrono::duration_cast<std::chrono::microseconds>(tile_end - tile_start).count());
            //delete this->job;
            (*this->job_counter)++;
            pthread_cond_signal(this->job_cond);
            pthread_mutex_unlock(this->job_mutex);
        }

    }

};



#endif
