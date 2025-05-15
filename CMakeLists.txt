cmake_minimum_required(VERSION 3.22)

project(juce_cmake_template VERSION 0.0.1)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/utf-8)
endif()

add_subdirectory(/Users/BWRS1_dev/krump_vst/lib/JUCE)

juce_add_plugin(NewProject
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Dem0

    FORMATS AU VST3 Standalone
    PRODUCT_NAME "NewProject")

target_sources(NewProject
    PRIVATE
        src/PluginEditor.cpp
        src/PluginProcessor.cpp)

target_compile_definitions(NewProject
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(NewProject
    PRIVATE
        juce::juce_audio_utils
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
