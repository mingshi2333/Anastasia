include_guard()

find_program(MOLD_LINKER mold)

# if(MOLD_LINKER)
# message(STATUS "Using mold as the linker")
# set(CMAKE_EXE_LINKER_FLAGS "-fuse-ld=mold")
# set(CMAKE_SHARED_LINKER_FLAGS "-fuse-ld=mold")
# else()
# message(STATUS "Mold not found, using default linker")
# endif()
