cmake_minimum_required(VERSION 3.15)

project(KrumpVST VERSION 0.0.1)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/utf-8)
endif()

add_subdirectory(lib/JUCE)

juce_add_plugin(KrumpVST
    VERSION 0.0.1
    COMPANY_NAME "BWRS1"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE BWRS
    PLUGIN_CODE Krum
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "KrumpVST")

target_sources(KrumpVST
    PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/MainComponent.cpp
        src/EffectChain.cpp)

target_compile_definitions(KrumpVST
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

target_link_libraries(KrumpVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
