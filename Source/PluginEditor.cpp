#include "PluginEditor.h"

MultiBandDeesserAudioProcessorEditor::MultiBandDeesserAudioProcessorEditor(MultiBandDeesserAudioProcessor& p)
    : AudioProcessorEditor(&p),
      processor(p),
      thresholdAttachment(p.getAPVTS(), "threshold", thresholdSlider),
      band1Attachment(p.getAPVTS(), "band1Amount", band1Slider),
      band2Attachment(p.getAPVTS(), "band2Amount", band2Slider),
      band3Attachment(p.getAPVTS(), "band3Amount", band3Slider),
      band4Attachment(p.getAPVTS(), "band4Amount", band4Slider),
      hfLimitAttachment(p.getAPVTS(), "hfLimit", hfLimitButton) {
    thresholdSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    thresholdSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(thresholdSlider);
    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(thresholdLabel);

    band1Slider.setSliderStyle(juce::Slider::LinearVertical);
    band1Slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(band1Slider);
    band1Label.setText("2-4k", juce::dontSendNotification);
    band1Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(band1Label);

    band2Slider.setSliderStyle(juce::Slider::LinearVertical);
    band2Slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(band2Slider);
    band2Label.setText("4-7k", juce::dontSendNotification);
    band2Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(band2Label);

    band3Slider.setSliderStyle(juce::Slider::LinearVertical);
    band3Slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(band3Slider);
    band3Label.setText("7-10k", juce::dontSendNotification);
    band3Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(band3Label);

    band4Slider.setSliderStyle(juce::Slider::LinearVertical);
    band4Slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
    addAndMakeVisible(band4Slider);
    band4Label.setText("10k+", juce::dontSendNotification);
    band4Label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(band4Label);

    hfLimitButton.setButtonText("HF Limit");
    addAndMakeVisible(hfLimitButton);

    setSize(420, 220);
}

MultiBandDeesserAudioProcessorEditor::~MultiBandDeesserAudioProcessorEditor() {}

void MultiBandDeesserAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour(0xff1a1a1a));
}

void MultiBandDeesserAudioProcessorEditor::resized() {
    auto r = getLocalBounds().reduced(10);
    const int labelH = 16;
    const int pad = 8;

    auto thresholdR = r.removeFromLeft(100);
    thresholdLabel.setBounds(thresholdR.removeFromTop(labelH));
    thresholdSlider.setBounds(thresholdR);

    r.removeFromLeft(pad);

    auto bandsR = r.withHeight(120);
    const int bandW = (bandsR.getWidth() - pad * 3) / 4;

    auto b1 = bandsR.removeFromLeft(bandW);
    band1Label.setBounds(b1.removeFromTop(labelH));
    band1Slider.setBounds(b1);

    bandsR.removeFromLeft(pad);
    auto b2 = bandsR.removeFromLeft(bandW);
    band2Label.setBounds(b2.removeFromTop(labelH));
    band2Slider.setBounds(b2);

    bandsR.removeFromLeft(pad);
    auto b3 = bandsR.removeFromLeft(bandW);
    band3Label.setBounds(b3.removeFromTop(labelH));
    band3Slider.setBounds(b3);

    bandsR.removeFromLeft(pad);
    auto b4 = bandsR.removeFromLeft(bandW);
    band4Label.setBounds(b4.removeFromTop(labelH));
    band4Slider.setBounds(b4);

    auto bottomR = r.withTrimmedTop(r.getHeight() - 36);
    hfLimitButton.setBounds(bottomR.removeFromLeft(90).reduced(4));
}
