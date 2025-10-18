file(GLOB ANA_EVENT_SRC *.cpp)

ana_setup_target(event ${ANA_EVENT_SRC})

# target_link_libraries(
# ana-camera PUBLIC ana-math
# )