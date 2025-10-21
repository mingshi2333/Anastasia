file(GLOB ANA_WSI_SRC *.cpp)

ana_setup_target(wsi ${ANA_WSI_SRC})

target_link_libraries(
    ana-wsi PUBLIC ana-math ana-common ana-event
)