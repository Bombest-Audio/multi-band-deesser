cmake_minimum_required(VERSION 3.22)
project(MultiBandDeesser VERSION 0.1.0)

add_subdirectory(JUCE)

juce_add_plugin(MultiBandDeesser
    COMPANY_NAME "Bombest Audio"
    BUNDLE_ID "com.bombestaudio.MultiBandDeesser"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE BomB
    PLUGIN_CODE MBDs
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Multi-Band De-Esser")

target_sources(MultiBandDeesser PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/DeEsserDSP.cpp
)

target_link_libraries(MultiBandDeesser PRIVATE
    juce::juce_core
    juce::juce_data_structures
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_dsp
)

target_compile_definitions(MultiBandDeesser PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
)
