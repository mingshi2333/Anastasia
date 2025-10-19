file(GLOB ANA_COMMON_SRC *.cpp)

ana_setup_target(common ${ANA_EVENT_SRC})

target_link_libraries(
    ana-common
    PUBLIC unordered_dense::unordered_dense

    # $<$<BOOL:${ANA_ENABLE_TRACING}>:Tracy::TracyClient>
)