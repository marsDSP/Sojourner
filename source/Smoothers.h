#pragma once

#include <juce_dsp/juce_dsp.h>
#include <cmath>
#include <vector>
#include <array>
#include "Parameters.h"

namespace MarsDSP
{
    // class designed to provide smooth interpolation between parameter changes..
    class Smoothers
    {
    public:
        explicit Smoothers(const Parameters& p) : params(p) {} // reading parameter values, never writing them

        void prepare(juce::dsp::ProcessSpec& spec) noexcept
        {
            constexpr double duration = 0.02;
            const int steps = static_cast<int>(spec.sampleRate * duration);
            auto resetAll = [steps](auto& smootherArray)
            {
                for (auto& smoother : smootherArray)
                    smoother.reset(steps);
            };

            resetAll(inputSmoother);
            resetAll(regenSmoother);
            resetAll(deRezSmoother);
            resetAll(predelaySmoother);
            resetAll(wetLvlSmoother);

            // After 200 ms (0.2f), the 1-pole filter will have approached the target value to within 63.2%.
            // The formula below describes the charge time of an analog capacitor.
            // It is common to use these representations in DSP code, as a great deal of DSP is based on EE.
            // Using a 1-pole here for an exponential curve to avoid zipper noise since it fits better in context.

            //coeff = 1.0f - std::exp(-1.0f / (0.2f * static_cast<float>(spec.sampleRate)));
        }

        void reset() noexcept
        {
            input = 0.0f;
            const auto gainVal = juce::Decibels::decibelsToGain(params.input->get());
            for (auto& smoother : inputSmoother)
                smoother.setCurrentAndTargetValue(gainVal);

            regen = 0.0f;
            for (auto& smoother : regenSmoother)
                smoother.setCurrentAndTargetValue(params.regen->get() * 0.01f);

            deRez = 0.0f;
            for (auto& smoother : deRezSmoother)
                smoother.setCurrentAndTargetValue(params.deRez->get()); // milliseconds

            predelay = 1.0f;
            for (auto& smoother : predelaySmoother)
                smoother.setCurrentAndTargetValue(params.predelay->get() * 0.01f);

            wetLvl = 0.0f;
            for (auto& smoother : wetLvlSmoother)
                smoother.setCurrentAndTargetValue(params.wetLvl->get() * 0.01f);

        }

        // Update the smoother's target when the parameter changes; the actual
        // gain value will be read via smoothen() per-sample to avoid zipper noise.
        void update() noexcept
        {
            const float gainDB = params.input->get();
            const float newGainDB = juce::Decibels::decibelsToGain(gainDB);
            for (auto& smoother : inputSmoother)
                smoother.setTargetValue(newGainDB);

            const float newRegenPercent = params.regen->get() * 0.01f; // 0..1
            for (auto& smoother : regenSmoother)
                smoother.setTargetValue(newRegenPercent);

            const float newDeRez = params.deRez->get();
            for (auto& smoother : deRezSmoother)
                smoother.setTargetValue(newDeRez);

            const float newPreDelay = params.predelay->get() * 0.01f; // -1..1
            for (auto& smoother : predelaySmoother)
                smoother.setTargetValue(newPreDelay);

            const float newWetLevel = params.wetLvl->get() * 0.01f; // -1..1
            for (auto& smoother : wetLvlSmoother)
                smoother.setTargetValue(newWetLevel);

            bypassed = params.bypass->get();
            oversample = params.oversample->getIndex();
            // delayNote = params.delayNoteChoice->getIndex();
            // tempoSync = params.tempoSyncParam->get();

            // targetDelayTime = params.delayTimeParam->get();

            // if (delayTime == 0.0f)
            // {
            //     delayTime = targetDelayTime;
            // }
        }

        // static void EqualPowerPan(const float panLR, float& L, float& R)
        // {
        //     const float x = 0.7853981633974483f * (panLR + 1.0f);
        //     L = std::cos(x);
        //     R = std::sin(x);
        // }

        void smoothen() noexcept
        {
            auto smoothen = [](auto& smootherArray)
            {
                for (auto& smoother : smootherArray)
                    smoother.getNextValue();
            };

            smoothen(inputSmoother);
            smoothen(regenSmoother);
            smoothen(deRezSmoother);
            smoothen(predelaySmoother);
            smoothen(wetLvlSmoother);

            // Compute equal-power pan from the smoothed stereo parameter (0..1 -> -1..1).
            // We only need channel[0] for pan, so we'll call .getNextValue() directly.
            // We'll save a few cycles this way, since panning is inherently mono.

            // const float pan = stereoPanSmoother[0].getNextValue();
            // EqualPowerPan(pan, panL, panR);
            // delayTime = targetDelayTime;
        }

        std::vector<std::array<juce::LinearSmoothedValue<float>, 2>*> getSmoothers() noexcept
        {
            return { &inputSmoother,
                     &regenSmoother,
                     &deRezSmoother,
                     &predelaySmoother,
                     &wetLvlSmoother };
        }

        enum class SmootherUpdateMode
        {
            initialize,
            liveInRealTime
        };

        void setSmoothers(int numSamplesToSkip, SmootherUpdateMode init) noexcept
        {
            juce::ignoreUnused(init);

            auto skipArray = [numSamplesToSkip](auto& smootherArray)
            {
                for (auto& s : smootherArray)
                    s.skip(numSamplesToSkip);
            };

            skipArray(inputSmoother);
            skipArray(regenSmoother);
            skipArray(deRezSmoother);
            skipArray(predelaySmoother);
            skipArray(wetLvlSmoother);
        }

        // Lightweight getters for consumers that need per-sample smoothed values
        // without calling smoothen() for the whole set at once.
        // These return the next smoothed value for the requested channel (0 = L, 1 = R).

        float getNextInput    (size_t channel = 0) noexcept { return inputSmoother[channel].getNextValue(); }
        float getNextRegen    (size_t channel = 0) noexcept { return regenSmoother[channel].getNextValue(); }
        float getNextDeRez    (size_t channel = 0) noexcept { return deRezSmoother[channel].getNextValue(); }
        float getNextPreDelay (size_t channel = 0) noexcept { return predelaySmoother[channel].getNextValue(); }
        float getNextWetLevel (size_t channel = 0) noexcept { return wetLvlSmoother[channel].getNextValue(); }

    private:

        // we don't need to copy, just reference
        const Parameters& params;

        float input    { 0.0f };
        float regen    { 0.0f };
        float deRez    { 0.0f };
        float predelay { 0.0f };
        float wetLvl   { 0.0f };
        int oversample { 0 };
        bool bypassed  { false };

        std::array<juce::LinearSmoothedValue<float>, 2>

        inputSmoother,
        regenSmoother,
        deRezSmoother,
        predelaySmoother,
        wetLvlSmoother;
    };
}
