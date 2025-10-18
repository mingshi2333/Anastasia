include_guard(GLOBAL)

function(ana_option name description default_value)
    set(valid_strings)

    if(ARGC GREATER 3)
        list(SUBLIST ARGV 3 -1 valid_strings)
    endif()

    if("${default_value}" STREQUAL "ON" OR "${default_value}" STREQUAL "OFF")
        option(${name} "${description}" ${default_value})
    else()
        set(${name} "${default_value}" CACHE STRING "${description}")

        if(valid_strings)
            set_property(CACHE ${name} PROPERTY STRINGS ${valid_strings})
        endif()
    endif()
endfunction()

function(ana_setup_target TARGET_SUFFIX)
    set(TARGET ana-${TARGET_SUFFIX})

    if(ANA_SHARED)
        add_library(${TARGET} SHARED ${ARGN})
    else()
        add_library(${TARGET} STATIC ${ARGN})
    endif()

    ana_compiler_options(${TARGET})
    target_include_directories(${TARGET} PRIVATE ${ANA_SRC_DIR})

    add_library(Anastasia::${TARGET_SUFFIX} ALIAS ${TARGET})
endfunction()