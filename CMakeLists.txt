cmake_minimum_required(VERSION 3.15)

project(SP404MKIIClone VERSION 1.0.0)

enable_language(CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/utf-8)
endif()

# JUCEのパスを設定
set(JUCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib/JUCE")
add_subdirectory(${JUCE_PATH})

# ソースファイル
set(SOURCE_FILES
    src/PluginProcessor.cpp
    src/PluginEditor.cpp
    src/MainComponent.cpp
    src/EffectChain.cpp
    src/audio/effects/DelayEffect.cpp
    src/audio/effects/FilterEffect.cpp
    src/audio/effects/ReverbEffect.cpp
    src/audio/effects/PitchEffect.cpp
    src/audio/effects/SaturationEffect.cpp
    src/audio/effects/DistortionEffect.cpp
    src/gui/components/EffectComponent.cpp
    src/gui/components/MainComponent.cpp
    src/gui/components/RotaryKnob.cpp
    src/gui/components/EffectButton.cpp
    src/gui/components/PresetButton.cpp
    src/gui/components/DisplayLabel.cpp
    src/gui/LookAndFeel/SP404LookAndFeel.cpp
    src/presets/PresetManager.cpp
    src/core/MidiManager.cpp
)

# ヘッダーファイル
set(HEADER_FILES
    src/PluginProcessor.h
    src/PluginEditor.h
    src/MainComponent.h
    src/EffectChain.h
    src/audio/effects/Effect.h
    src/audio/effects/DelayEffect.h
    src/audio/effects/FilterEffect.h
    src/audio/effects/ReverbEffect.h
    src/audio/effects/PitchEffect.h
    src/audio/effects/SaturationEffect.h
    src/audio/effects/DistortionEffect.h
    src/gui/components/EffectComponent.h
    src/gui/components/MainComponent.h
    src/gui/components/RotaryKnob.h
    src/gui/components/EffectButton.h
    src/gui/components/PresetButton.h
    src/gui/components/DisplayLabel.h
    src/gui/LookAndFeel/SP404LookAndFeel.h
    src/presets/PresetManager.h
    src/core/MidiManager.h
)

# プラグインターゲットを作成
juce_add_plugin(SP404MKIIClone
    VERSION "1.0.0"
    COMPANY_NAME "LIL RUGA"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE "LRUG"
    PLUGIN_CODE "SP4C"
    FORMATS
        VST3
        AU
        Standalone
    PRODUCT_NAME "SP404MKII Clone"
)

# ターゲットにソースを追加
target_sources(SP404MKIIClone
    PRIVATE
        ${SOURCE_FILES}
        ${HEADER_FILES}
)

# JUCEモジュールの依存関係を設定
target_compile_definitions(SP404MKIIClone
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
)

target_link_libraries(SP404MKIIClone
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
