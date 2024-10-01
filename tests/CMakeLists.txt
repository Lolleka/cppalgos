cmake_minimum_required( VERSION 3.10 )
set( CMAKE_CXX_COMPILER "/usr/bin/g++" )

project( cppalgos LANGUAGES CXX )

# message( STATUS "Examples included" )


# Some one-offs first:
# 1) Tests and main in one file
add_executable( 010-TestCase
    test_vector.cpp
)

# target_link_libraries(231-Cfg_OutputStreams Catch2_buildall_interface)
# target_compile_definitions(231-Cfg_OutputStreams PUBLIC CATCH_CONFIG_NOSTDOUT)

# set(ALL_EXAMPLE_TARGETS
#   010-TestCase
# )

# foreach( name ${ALL_EXAMPLE_TARGETS} )
#     target_link_libraries( ${name} Catch2WithMain )
# endforeach()


# list(APPEND CATCH_WARNING_TARGETS ${ALL_EXAMPLE_TARGETS})
# set(CATCH_WARNING_TARGETS ${CATCH_WARNING_TARGETS} PARENT_SCOPE)
