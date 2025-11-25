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

        return layout;

    }



}

