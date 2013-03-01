
#include <algorithm>

#include "DSPConfig.h"

#include "AudioAnalysisOnsetSpectralFlux.h"

//! release method
void AudioAnalysisOnsetSpectralFlux::Reset()
{
	m_lastSpectrum.clear();
	m_spectralFlux.clear();
	m_threshold.clear();
	m_prunnedSpectralFlux.clear();
	m_onset.clear();
}

//! dsp event handler
void AudioAnalysisOnsetSpectralFlux::OnEvent(const SDSPEventData& event)
{
    switch (event.event)
	{
	case EDE_SpectrumBegin:
		Reset();
		break;
	case EDE_Spectrum:
		getSpectrumData(event);
		break;
	case EDE_SpectrumEnd:
		doSpectrumAnalysis();
		break;
	}
}

// get spectrum data
void AudioAnalysisOnsetSpectralFlux::getSpectrumData(const SDSPEventData& event)
{
	if (m_lastSpectrum.size() <= 0) // seems first spectrum
	{
		const float* spectrum = event.spectrumData;
		// buffer the spectrum data
		for (size_t i = 0; i < event.spectrumCount; ++i)
		{
            m_lastSpectrum.push_back(spectrum[i]);
		}
        m_audioInfo = event.audioInfo; // buffer audio info here
	}
	else
	{
		const float* spectrum = event.spectrumData;
		
		float flux = 0;
		
		// do the flux calculation
		for(size_t i = 0; i < event.spectrumCount; ++i)
		{
			float value = spectrum[i] - m_lastSpectrum[i];
			flux += value < 0 ? 0 : value;
		}
		m_spectralFlux.push_back(flux);
		
		// buffer last spectrum data
		m_lastSpectrum.clear();
		for (size_t i = 0; i < event.spectrumCount; ++i)
		{
			m_lastSpectrum.push_back(spectrum[i]);
		}
	}
}

void AudioAnalysisOnsetSpectralFlux::doSpectrumAnalysis()
{
	// set window size by sensitivity time
	//float windowTime = (float)DSP_AUDIO_FRAME_PER_BLOCK / m_audioInfo.samplerate;
    //size_t thresholdWindowSize = m_thresholdTime / windowTime;

	//////////////////////////////////////////////////////////////////////////
	size_t thresholdWindowSize = 10;
	//////////////////////////////////////////////////////////////////////////

	// do threshold
	for (size_t i = 0; i < m_spectralFlux.size(); ++i)
	{
		int start = std::max(0, int(i - thresholdWindowSize));
		int end = std::min((int)(m_spectralFlux.size() - 1), (int)(i + thresholdWindowSize));
		float mean = 0;
		for (int j = start; j <= end; ++j)
		{
			mean += m_spectralFlux[j];
		}
		mean /= (end - start);
		m_threshold.push_back(mean * m_multiplier);
	}

	// do prune
	for (size_t i = 0; i < m_threshold.size(); ++i)
	{
		if (m_threshold[i] <= m_spectralFlux[i])
		{
			m_prunnedSpectralFlux.push_back(m_spectralFlux[i] - m_threshold[i]);
		}
		else
		{
			m_prunnedSpectralFlux.push_back(0);
		}
	}

	// at last, we analysis onset
	for (int i = 0; i < int(m_prunnedSpectralFlux.size() - 1); ++i)
	{
		if (m_prunnedSpectralFlux[i] > m_prunnedSpectralFlux[i+1])
		{
			m_onset.push_back(i * DSP_AUDIO_FRAME_PER_BLOCK * 1.0f / m_audioInfo.samplerate);
		}
	}
}