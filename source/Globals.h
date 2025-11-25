#pragma once

#include <Includes.h>

inline const juce::ParameterID inputParamID { "input", 1 };
static constexpr const char* inputParamIDName = "Input";

inline const juce::ParameterID regenParamID { "regen", 1 };
static constexpr const char* regenParamIDName = "Regen";

inline const juce::ParameterID deRezParamID { "deRez", 1 };
static constexpr const char* deRezParamIDName = "DeRez";

inline const juce::ParameterID predelayParamID { "predelay", 1 };
static constexpr const char* predelayParamIDName = "Predelay";

inline const juce::ParameterID wetLvlParamID { "wetLvl", 1 };
static constexpr const char* wetLvlParamIDName = "WetLevel";

inline const juce::ParameterID bypassParamID { "bypass", 1 };
static constexpr const char* bypassParamIDName = "Bypass";

inline const juce::StringArray predelaySync
{
    "1/1", "1/2", "1/4", "1/8", "1/16", "1/32"
};

static constexpr float minDelayTime = 0.0f;
static constexpr float maxDelayTime = 500.0f;