#pragma once

#ifndef SOJOURNER_PARAMETERS_H
#define SOJOURNER_PARAMETERS_H

#include <Includes.h>


namespace MarsDSP {

    class Parameters {

    public:

        explicit Parameters(juce::AudioProcessorValueTreeState& vts);
        static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
        ~Parameters() = default;

        template<typename T>
        static void castParameter(juce::AudioProcessorValueTreeState& vts, const juce::ParameterID& id, T& destination)
        {
            destination = dynamic_cast<T>(vts.getParameter(id.getParamID()));
            jassert(destination);
        }

        juce::AudioParameterFloat* input {nullptr};
        juce::AudioParameterFloat* regen {nullptr};
        juce::AudioParameterFloat* deRez {nullptr};
        juce::AudioParameterFloat* predelay {nullptr};
        juce::AudioParameterFloat* wetLvl {nullptr};
        juce::AudioParameterBool*  bypass {nullptr};
    };
}

#endif