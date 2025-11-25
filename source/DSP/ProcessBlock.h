#pragma once

#include "140/Plate140.h"
#include "../Parameters.h"

namespace MarsDSP::DSP {

    class ProcessBlock {

        public:

        ProcessBlock() = default;
        ~ProcessBlock() = default;

        void prepare (const double sample_Rate, int samples_Per_Block, int num_Channels, int factor, const MarsDSP::Parameters& params)
        {
            juce::dsp::ProcessSpec spec {};
            spec.sampleRate = sample_Rate * (double)(1 << factor);
            spec.maximumBlockSize = static_cast<juce::uint32> (samples_Per_Block * (1 << factor));
            spec.numChannels = static_cast<juce::uint32> (num_Channels);

            m_spec = spec;

            m_smoothers = std::make_unique<MarsDSP::Smoothers>(params);
            m_smoothers->prepare(m_spec);
            m_smoothers->reset();

            plate.prepare(m_spec);
            m_oversample = std::make_unique<juce::dsp::Oversampling<float>>(
                static_cast<int>(m_spec.numChannels),
                factor,
                juce::dsp::Oversampling<float>::FilterType::filterHalfBandPolyphaseIIR,
                true);

            m_oversample->initProcessing(static_cast<size_t>(m_spec.maximumBlockSize));
        }

        void process(juce::AudioBuffer<float>& buffer, const int num_samples)
        {
            if (m_oversample == nullptr)
                return;

            juce::dsp::AudioBlock<float> block (buffer);

            const int osFactor = static_cast<int>(m_oversample->getOversamplingFactor());
            const int numOversampled = num_samples * osFactor;

            // Upsample into internal buffers managed by the oversampler
            auto up = m_oversample->processSamplesUp(block);

            // Ensure we have at least one channel.
            // default is stereo, so use channel 0 for both if mono
            const int upNumChannels = static_cast<int>(up.getNumChannels());
            float* outL = up.getChannelPointer(0);
            float* outR = upNumChannels > 1 ? up.getChannelPointer(1) : up.getChannelPointer(0);
            const float* inL = outL;
            const float* inR = outR;

            // Process the oversampled stereo block
            if (m_smoothers)
                plate.process(inL, inR, outL, outR, numOversampled, *m_smoothers);

            // Downsample back into the original buffer
            m_oversample->processSamplesDown(block);
        }

        // Returns the current audio sample rate
        [[nodiscard]] double getSampleRate() const noexcept
        {
            return static_cast<double>(m_spec.sampleRate);
        }

        void updateParams(const MarsDSP::Parameters& /*parameters*/)
        {
            if (m_smoothers)
                m_smoothers->update();
        }

    private:

        Plate140 plate;
        std::unique_ptr<juce::dsp::Oversampling<float>> m_oversample;
        std::unique_ptr<MarsDSP::Smoothers> m_smoothers;
        juce::dsp::ProcessSpec m_spec {};
    };
}
