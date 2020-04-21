cmake_minimum_required(VERSION 3.5)

project(indexing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(LibArchive REQUIRED)
find_package(Threads REQUIRED)
find_package(Boost COMPONENTS locale system REQUIRED)
find_package(ICU REQUIRED COMPONENTS uc i18n)
set(SRC src/word_counting/map_merging.cpp src/archive_handler/archive_handler.cpp
        src/word_counting/word_counting.cpp src/conf_handler/confhandler.cpp)

set(INC include/conc_queue/synch_queue.h
        include/word_counting/map_merging.h include/conf_handler/confhandler.h
        include/archive_handler/archive_handler.h include/word_counting/word_counting.h)
add_executable(word_indexing main.cpp ${SRC} ${INC})

target_link_libraries(word_indexing ${LibArchive_LIBRARIES}
    Threads::Threads ${Boost_LIBRARIES} Boost::locale Boost::system
        ICU::uc ICU::i18n)


if(MINGW)
    target_link_libraries(word_indexing ws2_32)
endif()
