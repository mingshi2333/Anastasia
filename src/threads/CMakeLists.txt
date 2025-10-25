file(GLOB ANA_THREADS_SRC *.h *.cpp)

ana_setup_target(threads ${ANA_THREADS_SRC})
target_link_libraries(
    ana-threads
    PUBLIC
    Threads::Threads
)