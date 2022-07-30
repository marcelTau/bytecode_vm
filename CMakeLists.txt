set(NAME Tests)

add_executable(${NAME}
    tests.cpp
)


target_link_libraries(Tests PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main ${PROJECT_NAME})

add_test(
    NAME ${NAME}
    COMMAND ${NAME}
)

