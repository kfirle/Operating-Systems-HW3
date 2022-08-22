#ifndef __GAMERUN_H
#define __GAMERUN_H

#include "Headers.hpp"
#include "Thread.hpp"
#include "utils.hpp"
#include "PCQueue.hpp"
/*--------------------------------------------------------------------------------
								  Species colors
--------------------------------------------------------------------------------*/
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black - 7 */
#define RED     "\033[31m"      /* Red - 1*/
#define GREEN   "\033[32m"      /* Green - 2*/
#define YELLOW  "\033[33m"      /* Yellow - 3*/
#define BLUE    "\033[34m"      /* Blue - 4*/
#define MAGENTA "\033[35m"      /* Magenta - 5*/
#define CYAN    "\033[36m"      /* Cyan - 6*/


/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {
    // All here are derived from ARGV, the program's input parameters.
    uint n_gen;
    uint n_thread;
    string filename;
    bool interactive_on;
    bool print_on;
};
/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/
class Game {
public:

    Game(game_params& g): m_gen_num(g.n_gen), interactive_on(g.interactive_on), print_on(g.print_on) {
        vector<string> t_board = utils::read_lines(g.filename);
        this->board = new int_mat;
        this->temp_board = new int_mat;
        this->board_height = t_board.size();
        this->board->resize(this->board_height);
        this->temp_board->resize(this->board_height);
        if(this->board_height < g.n_thread){
            this->m_thread_num = this->board_height;
        }
        else{
            this->m_thread_num = g.n_thread;
        }
        vector<string> t_row;
        unsigned int row = 0;
        unsigned int col=0;
        for (const auto &line :t_board) {
            t_row = utils::split(line, DEF_MAT_DELIMITER);
            this->board->at(row).resize(t_row.size());
            this->temp_board->at(row).resize(t_row.size());
            for (col = 0; col < t_row.size(); col++) {
                this->board->at(row).at(col) = stoul(t_row.at(col));
            }
            row++;
        }
        this->board_width = col;
        this->job_counter = 0;
        pthread_mutex_init(&this->job_mutex, nullptr);
        pthread_cond_init(&this->job_cond, nullptr);

        this->interactive_on = g.interactive_on;
        this->print_on = g.print_on;
        this->m_gen_num = g.n_gen;

        /*this->m_threadpool;
        //this->m_gen_hist;
        //this->m_tile_hist;
        //this->m_threadpool = *(new vector<Thread*>);
        //this->m_gen_hist = *(new vector<double>);
        //this->m_tile_hist = *(new vector<double>);*/

    }

  /*  ~Game(){
        delete board;
        delete temp_board;
        //delete &this->m_threadpool;
        //delete &this->m_gen_hist;
        //delete &this->m_tile_hist;
    };*/

    void run(); // Runs the game

    const vector<double> gen_hist() const {
        return this->m_gen_hist;
    }; // Returns the generation timing histogram
    const vector<double> tile_hist() const {
        return this->m_tile_hist;
    }; // Returns the tile timing histogram
    uint thread_num() const {
         return this->m_thread_num;
    }; //Returns the effective number of running threads = min(thread_num, field_height)


protected: // All members here are protected, instead of private for testing purposes

    // See Game.cpp for details on these three functions
    void _init_game();
    void _step(uint curr_gen);
    void _destroy_game();
    inline void print_board(const char* header);

    uint m_gen_num; 			 // The number of generations to run
    uint m_thread_num; 			 // Effective number of threads = min(thread_num, field_height)
    vector<double> m_tile_hist; 	 // Shared Timing history for tiles: First (2 * m_gen_num) cells are the calculation durations for tiles in generation 1 and so on.
    // Note: In your implementation, all m_thread_num threads must write to this structure.
    vector<double> m_gen_hist;  	 // Timing history for generations: x=m_gen_hist[t] iff generation t was calculated in x microseconds
    vector<Thread*> m_threadpool; // A storage container for your threads. This acts as the threadpool.

    bool interactive_on; // Controls interactive mode - that means, prints the board as an animation instead of a simple dump to STDOUT
    bool print_on; // Allows the printing of the board. Turn this off when you are checking performance (Dry 3, last question)

    // TODO: Add in your variables and synchronization primitives
    //vector<string> map_game;
    int_mat* board;
    int_mat* temp_board;
    unsigned int board_height;
    unsigned int board_width;
    PCQueue<Job> pcq;
    int job_counter;
    pthread_mutex_t job_mutex;
    pthread_cond_t job_cond;

};



#endif
