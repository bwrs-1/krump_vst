cmake_minimum_required(VERSION 3.22)

project(krump_vst VERSION 0.0.1)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/utf-8)
endif()

add_subdirectory(lib/JUCE)

juce_add_plugin(NewProject
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Dem0

    FORMATS AU VST3 Standalone
    PRODUCT_NAME "KRUMP_VST")

target_sources(NewProject
    PRIVATE
        src/PluginEditor.cpp
        src/PluginProcessor.cpp)

target_compile_definitions(NewProject
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)

# juce_add_binary_data(Resources SOURCES
#     Resources/sp404_base.jpg
#     Resources/pad_on.jpg
#     Resources/pad_off.jpg
#     Resources/knob.jpg
# )

target_link_libraries(NewProject
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
