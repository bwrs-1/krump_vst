add_executable(KrumpVSTTests
    PluginTests.cpp)

target_link_libraries(KrumpVSTTests
    PRIVATE
        KrumpVST)

add_test(NAME KrumpVSTTests COMMAND KrumpVSTTests) 