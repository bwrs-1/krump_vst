cmake_minimum_required(VERSION 3.15)

project(KrumpVST VERSION 0.1.0)

# C++17を要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCEとImGuiをサブディレクトリとして追加
add_subdirectory(lib/JUCE)
add_subdirectory(lib/imgui)

# プラグイン設定
juce_add_plugin(KrumpVST
    VERSION 0.1.0
    COMPANY_NAME "KrumpVST"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Krmp
    PLUGIN_CODE Krst
    FORMATS VST3
    PRODUCT_NAME "KrumpVST")

# ソースファイルの追加
target_sources(KrumpVST
    PRIVATE
        Source/Core/PluginProcessor.cpp
        Source/GUI/PluginEditor.cpp
        Source/DSP/ReverbEffect.cpp)

# JUCEモジュールのリンク
target_link_libraries(KrumpVST
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_core
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_opengl
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        imgui)

# インクルードディレクトリの設定
target_include_directories(KrumpVST
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/imgui
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/JUCE/modules
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/JUCE/modules/juce_audio_processors/format_types/VST3_SDK)

# デバッグ/リリース設定
set_target_properties(KrumpVST PROPERTIES
    DEBUG_POSTFIX "d"
    RELEASE_POSTFIX "")

# テスト設定
if(BUILD_TESTING)
    add_subdirectory(Tests)
endif()
