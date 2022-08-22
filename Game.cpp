#include "Game.hpp"
#include "utils.hpp"
#include "Thread.hpp"


static const char *colors[7] = {BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN};
/*--------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
void Game::run() {

    _init_game(); // Starts the threads and all other variables you need
    print_board("Initial Board");
    for (uint i = 0; i < m_gen_num; ++i) {
        auto gen_start = std::chrono::system_clock::now();
        _step(i); // Iterates a single generation
        auto gen_end = std::chrono::system_clock::now();
        m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
        print_board(nullptr);
    } // generation loop
    print_board("Final Board");
    _destroy_game();
}

void Game::_init_game() {

    // Create game fields - Consider using utils:read_file, utils::split

    // Create & Start threads
    for(unsigned int i=0 ; i < this->m_thread_num ; i++){
       auto* phase_work_i = new PhaseWork(i,this->board,this->temp_board,&this->pcq,
               &this->job_counter,&this->job_mutex, &this->job_cond ,&this->m_tile_hist );
       this->m_threadpool.push_back(phase_work_i);
       phase_work_i->start();
    }


    // Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
    // Push jobs to queue
    unsigned int i;
    unsigned int k;
   for(i = 0, k = 1  ; i < this->board_height && k < this->m_thread_num+1 ; i=i+this->board_height/this->m_thread_num,k++) {
        unsigned int endrow = i+this->board_height/this->m_thread_num - 1;
        if(k >= this->m_thread_num) endrow = this->board_height - 1;
      //  Job* job1 = new Job(i,endrow,this->board_width,this->board_height,PHASE_ONE);
        this->pcq.push( Job(i,endrow,this->board_width,this->board_height,PHASE_ONE) );
    }

    // Wait for the workers to finish calculating
        pthread_mutex_lock(&this->job_mutex);
        while (this->job_counter != this->m_thread_num){
            pthread_cond_wait(&this->job_cond,&this->job_mutex);
        }
        this->job_counter = 0;
        pthread_mutex_unlock(&this->job_mutex);

    // Swap pointers between current and next field (found in PhaseWork)
    for(i = 0, k = 1  ; i < this->board_height && k < this->m_thread_num+1 ; i=i+this->board_height/this->m_thread_num,k++) {
        unsigned int endrow = i+this->board_height/this->m_thread_num - 1;
        if(k >= this->m_thread_num) endrow = this->board_height - 1;
      //  Job* job1 = new Job(i,endrow,this->board_width,this->board_height,PHASE_TWO);
        this->pcq.push( Job(i,endrow,this->board_width,this->board_height,PHASE_TWO) );
    }

    pthread_mutex_lock(&this->job_mutex);
    while (this->job_counter != this->m_thread_num){
        pthread_cond_wait(&this->job_cond,&this->job_mutex);
    }
    this->job_counter = 0;
    pthread_mutex_unlock(&this->job_mutex);



    // NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade
}

void Game::_destroy_game(){

    // Destroys board and frees all threads and resources
    // Not implemented in the Game's destructor for testing purposes.
    // All threads must be joined here

    delete this->board;
    delete this->temp_board;
    for (uint i = 0; i <= m_thread_num+1; i++)
    {
        this->pcq.push( Job(0,0,0,0,PHASE_THREE));
        //cout << "job " << i << endl;
    }
    for (uint i = 0; i < m_thread_num; i++)
    {
        //cout <<"wait " << i << endl;
        m_threadpool[i]->join();
    }
    for (uint i = 0; i < m_thread_num; i++)
    {
        delete(m_threadpool[i]);
    }

}

/*--------------------------------------------------------------------------------

--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

    if(print_on){

        // Clear the screen, to create a running animation
        if(interactive_on)
            system("clear");

        // Print small header if needed
        if (header != nullptr)
            cout << "<------------" << header << "------------>" << endl;

        // TODO: Print the board


        cout << u8"╔" << string(u8"═") * this->board_width << u8"╗" << endl;
        for (uint i = 0; i < this->board_height; ++i) {
            cout << u8"║";
            for (uint j = 0; j < this->board_width ; ++j) {
                if ((*board)[i][j] > 0)
                    cout << colors[(*board)[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
            }
            cout << u8"║" << endl;
        }
        cout << u8"╚" << string(u8"═") * this->board_width << u8"╝" << endl;

        // Display for GEN_SLEEP_USEC micro-seconds on screen
        if(interactive_on)
            usleep(GEN_SLEEP_USEC);
    }


}



/* Function sketch to use for printing the board. You will need to decide its placement and how exactly
	to bring in the field's parameters.

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
                if (field[i][j] > 0)
                    cout << colors[field[i][j] % 7] << u8"█" << RESET;
                else
                    cout << u8"░";
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 



