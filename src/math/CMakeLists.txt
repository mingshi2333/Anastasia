file(GLOB ANA_MATH_SRC *.cpp)

ana_setup_target(math ${ANA_MATH_SRC})
target_link_libraries(
    ana-math
    PUBLIC glm
)