/*!
    \file AudioAnalysis_Onset_SpectralFlux.h
	\brief simple implementation of spectral flux onset detection
*/

#ifndef __AA_ONSET_SPECTRAL_FLUX_H__
#define __AA_ONSET_SPECTRAL_FLUX_H__

#include <vector>

#include "IDSPEventDelegate.h"

class AudioAnalysisOnsetSpectralFlux: public IDSPEventDelegate
{
public:
	AudioAnalysisOnsetSpectralFlux():m_thresholdTime(0.16f),m_multiplier(1.5f) {};
	//! reset method
	void Reset();
	//! dsp event handler
	virtual void OnEvent(const SDSPEventData& event);
    //! get the onset count
	size_t GetOnsetCount() const { return m_onset.size(); }
	//! get the onset time which is detected
	float GetOnsetByIndex(size_t index) const
	{ 
		if (index >= m_onset.size()) return -1;
		return m_onset[index];
	}

private:
	// get spectrum data
	void getSpectrumData(const SDSPEventData& event);
	// do spectrum analysis
	void doSpectrumAnalysis();

private:
	std::vector<float> m_lastSpectrum; //< last spectrum buffer
	std::vector<float> m_spectralFlux; //< spectral flux TODO: more detail, like bin spectral flux
	std::vector<float> m_threshold; //< threshold flux buffer
	std::vector<float> m_prunnedSpectralFlux; //< prunne spectral flux
	std::vector<float> m_onset; //< onset storage
private:
	float m_thresholdTime; //< threshold time
	float m_multiplier; //< onset detect multiplier
private:
    AudioInfo m_audioInfo; //< audio info buffer
};

#endif // __AA_ONSET_SPECTRAL_FLUX_H__