#------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------
# MAIN PROJECT's SETTINGS

cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME "orm")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------
# EXECUTABLE

if($ENV{GITHUB_RUN_NUMBER})
    project(${PROJECT_NAME} LANGUAGES CXX VERSION 0.0.$ENV{GITHUB_RUN_NUMBER})
else()
    project(${PROJECT_NAME} LANGUAGES CXX VERSION 0.0.1)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(${PROJECT_NAME} "src/main.cpp")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

#------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------
# LIBRARIES

set(INC_VER "lib/version/include")
set(SRC_VER "lib/version/src")

add_library(
    version
    "${SRC_VER}/version_lib.cpp"
)

target_include_directories(version PUBLIC ${INC_VER})
target_compile_options(version PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE version)

configure_file("${INC_VER}/version.h.in"   "${CMAKE_BINARY_DIR}/version.h")
target_include_directories(version PRIVATE "${CMAKE_BINARY_DIR}")
target_include_directories(version PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

#------------------------------------------------------------------------------------------------------

set(INC_LIBEORM "lib/eorm/include")
set(SRC_LIBEORM "lib/eorm/src")

add_library(
    libeorm
    "${SRC_LIBEORM}/eorm_table.cpp"
    "${SRC_LIBEORM}/eorm_tablecolumn.cpp"
)

target_include_directories(libeorm PUBLIC ${INC_LIBEORM})
target_compile_options(libeorm PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE libeorm)

#------------------------------------------------------------------------------------------------------

set(INC_LIBEORM_SQLITE "lib/eorm_sqlite/include")
set(SRC_LIBEORM_SQLITE "lib/eorm_sqlite/src")

add_library(
    libeorm_sqlite
    "${SRC_LIBEORM_SQLITE}/eorm_sqlite.cpp"
)

find_package (SQLite3)
target_link_libraries (libeorm_sqlite libeorm ${SQLite3_LIBRARIES})

target_include_directories(libeorm_sqlite PUBLIC ${INC_LIBEORM_SQLITE} ${SQLite3_INCLUDE_DIRS})
target_compile_options(libeorm_sqlite PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(${PROJECT_NAME} PRIVATE libeorm_sqlite)

#------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------
# TESTS

find_package(Boost COMPONENTS unit_test_framework REQUIRED)
enable_testing()

#------------------------------------------------------------------------------------------------------

set(SRC_TEST_VER "lib/version/test")

add_executable(
    test_version
    "${SRC_TEST_VER}/test_version.cpp"
)

target_include_directories(test_version PUBLIC ${INC_VER})
set_target_properties(test_version PROPERTIES COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

target_compile_options(test_version PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(test_version PRIVATE ${Boost_LIBRARIES} version)

add_test(test_version test_version)

#------------------------------------------------------------------------------------------------------

set(SRC_TEST_EORM "lib/eorm/test")

add_executable(
    test_eorm
    "${SRC_TEST_EORM}/test_eorm.cpp"
)

target_include_directories(test_eorm PUBLIC ${INC_LIBEORM})
set_target_properties(test_eorm PROPERTIES COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

target_compile_options(test_eorm PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(test_eorm PRIVATE ${Boost_LIBRARIES} libeorm)

add_test(test_eorm test_eorm)

#------------------------------------------------------------------------------------------------------

set(SRC_TEST_EORM_SQLITE "lib/eorm_sqlite/test")

add_executable(
    test_eorm_sqlite
    "${SRC_TEST_EORM_SQLITE}/test_eorm_sqlite.cpp"
)

target_include_directories(test_eorm_sqlite PUBLIC ${INC_LIBEORM_SQLITE})
set_target_properties(test_eorm_sqlite PROPERTIES COMPILE_DEFINITIONS BOOST_TEST_DYN_LINK INCLUDE_DIRECTORIES ${Boost_INCLUDE_DIR})

target_compile_options(test_eorm_sqlite PRIVATE -Wall -Wextra -pedantic -Werror)
target_link_libraries(test_eorm_sqlite PRIVATE ${Boost_LIBRARIES} libeorm_sqlite)

add_test(test_eorm_sqlite test_eorm_sqlite)

#------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------
# DEPLOYMENT

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT r3d9477@gmail.com)

include(CPack)

#------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------
# DOXYGEN

#option(BUILD_DOC "Build documentation" ON)

#find_package(Doxygen)

#if (DOXYGEN_FOUND)

#    set(DOXYGEN_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
#    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
#    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

#    message("Doxygen build started")

#    add_custom_target( doc_doxygen ALL
#        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
#        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
#        COMMENT "Generating API documentation with Doxygen"
#        VERBATIM
#    )

#else (DOXYGEN_FOUND)
#    message("Doxygen need to be installed to generate the doxygen documentation")

#endif (DOXYGEN_FOUND)
