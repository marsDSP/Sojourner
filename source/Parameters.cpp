#include "Parameters.h"
#include "Converters.h"
#include "Globals.h"

namespace MarsDSP {

    Parameters::Parameters(juce::AudioProcessorValueTreeState& vts)
    {
        castParameter (vts, inputParamID, input);

    }

    juce::AudioProcessorValueTreeState::ParameterLayout Parameters::createParameterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        layout.add(std::make_unique<juce::AudioParameterFloat>(inputParamID, inputParamIDName,
            juce::NormalisableRange<float> { -12.0f, 12.0f }, 0.0f,
            juce::AudioParameterFloatAttributes().withStringFromValueFunction(Converter::stringFromDecibels)));

        layout.add(std::make_unique<juce::AudioParameterFloat>
            (regenParamID, regenParamIDName, juce::NormalisableRange<float>
                {0.0f, 100.0f, 1.0f}, 0.0f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(Converter::stringFromPercent)));

        layout.add(std::make_unique<juce::AudioParameterFloat>
            (deRezParamID, deRezParamIDName, juce::NormalisableRange<float>
                {0.0f, 100.0f, 1.0f}, 0.0f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(Converter::stringFromPercent)));

        layout.add(std::make_unique<juce::AudioParameterFloat>
            (predelayParamID, predelayParamIDName, juce::NormalisableRange<float>
                {minDelayTime, maxDelayTime, 0.001f, 0.25f}, 0.0f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(Converter::stringFromMilliseconds)
            .withValueFromStringFunction(Converter::millisecondsFromString)));

        layout.add(std::make_unique<juce::AudioParameterFloat>
            (wetLvlParamID, wetLvlParamIDName, juce::NormalisableRange<float>
                {0.0f, 100.0f, 1.0f}, 50.0f,
            juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(Converter::stringFromPercent)));

        return layout;

    }
}

