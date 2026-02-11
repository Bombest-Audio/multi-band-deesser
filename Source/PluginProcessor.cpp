#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout MultiBandDeesserAudioProcessor::createParameterLayout() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "threshold", "Threshold",
        juce::NormalisableRange<float>(-24.0f, 0.0f, 0.1f), -12.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "band1Amount", "2-4k",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "band2Amount", "4-7k",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "band3Amount", "7-10k",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "band4Amount", "10k+",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "hfLimit", "HF Limit", true));

    return { params.begin(), params.end() };
}

MultiBandDeesserAudioProcessor::MultiBandDeesserAudioProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMS", createParameterLayout()) {}

MultiBandDeesserAudioProcessor::~MultiBandDeesserAudioProcessor() {}

void MultiBandDeesserAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    dsp.prepare(sampleRate, samplesPerBlock);
}

void MultiBandDeesserAudioProcessor::releaseResources() {
    dsp.reset();
}

bool MultiBandDeesserAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    return layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet();
}

void MultiBandDeesserAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) {
    juce::ScopedNoDenormals noDenormals;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    const float threshold = apvts.getRawParameterValue("threshold")->load();
    float bandAmounts[DeEsserDSP::NUM_BANDS];
    bandAmounts[0] = apvts.getRawParameterValue("band1Amount")->load();
    bandAmounts[1] = apvts.getRawParameterValue("band2Amount")->load();
    bandAmounts[2] = apvts.getRawParameterValue("band3Amount")->load();
    bandAmounts[3] = apvts.getRawParameterValue("band4Amount")->load();
    const bool hfLimit = apvts.getRawParameterValue("hfLimit")->load() > 0.5f;

    dsp.process(buffer, threshold, bandAmounts, hfLimit);
}

juce::AudioProcessorEditor* MultiBandDeesserAudioProcessor::createEditor() {
    return new MultiBandDeesserAudioProcessorEditor(*this);
}

void MultiBandDeesserAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void MultiBandDeesserAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid())
        apvts.replaceState(tree);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    return new MultiBandDeesserAudioProcessor();
}
