project(GameOfLife)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall -oedantic-errors -lpthread -pthread")
add_executable(         GameOfLife main2.cpp Semaphore.hpp Semaphore.cpp
                        PCQueue.hpp  Thread.hpp
                         utils.hpp utils.cpp
                        Game.hpp Game.cpp)