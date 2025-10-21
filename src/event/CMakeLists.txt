file(GLOB ANA_EVENT_SRC *.cpp *.h)

ana_setup_target(event ${ANA_EVENT_SRC})

target_link_libraries(
    ana-event PUBLIC ana-math ana-common
)