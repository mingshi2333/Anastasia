file(GLOB ANA_CAMERA_SRC *.cpp)

ana_setup_target(camera ${ANA_CAMERA_SRC})
target_link_libraries(
    ana-camera PUBLIC ana-math
)