#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MultiBandDeesserAudioProcessorEditor : public juce::AudioProcessorEditor {
public:
    explicit MultiBandDeesserAudioProcessorEditor(MultiBandDeesserAudioProcessor&);
    ~MultiBandDeesserAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    MultiBandDeesserAudioProcessor& processor;

    juce::Slider thresholdSlider;
    juce::Slider band1Slider;
    juce::Slider band2Slider;
    juce::Slider band3Slider;
    juce::Slider band4Slider;
    juce::ToggleButton hfLimitButton;

    juce::Label thresholdLabel;
    juce::Label band1Label;
    juce::Label band2Label;
    juce::Label band3Label;
    juce::Label band4Label;

    juce::AudioProcessorValueTreeState::SliderAttachment thresholdAttachment;
    juce::AudioProcessorValueTreeState::SliderAttachment band1Attachment;
    juce::AudioProcessorValueTreeState::SliderAttachment band2Attachment;
    juce::AudioProcessorValueTreeState::SliderAttachment band3Attachment;
    juce::AudioProcessorValueTreeState::SliderAttachment band4Attachment;
    juce::AudioProcessorValueTreeState::ButtonAttachment hfLimitAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiBandDeesserAudioProcessorEditor)
};
