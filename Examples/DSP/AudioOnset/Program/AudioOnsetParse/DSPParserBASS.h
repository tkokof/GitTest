/*!
    \file DSPParserBASS.h
	\brief simple dsp parser using BASS audio library
	\note more info about BASS see http://www.un4seen.com/bass.html
*/

#ifndef __DSP_PARSER_BASS_H__
#define __DSP_PARSER_BASS_H__

#include "bass.h"

#include "IDSPEventDelegate.h"

class DSPParserBASS
{
public:
	DSPParserBASS():m_BASSStream(0),m_curFrameCount(0),m_dspDelegate(NULL) {};
	//! do DSP stream operation by the given filename
	bool DoDSPParse(const char* filename);
	//! dsp event delegate
	void SetDSPEventDelegate(IDSPEventDelegate* delegate_) { m_dspDelegate = delegate_; }
	IDSPEventDelegate* GetDSPEventDelegate() { return m_dspDelegate; }

private:
	// dispatch helper function
	void dispatchSpectrumEvent(EDSPEvent event, size_t spectrumCount, const float* spectrum);
	// get bass FFT flag
    DWORD getBassFFTFlag(size_t windowSize) const;

private:
	HSTREAM m_BASSStream; //< BASS stream handler
	size_t m_curFrameCount; //< current frame count
    AudioInfo m_audioInfo; //< audio info
private:
    IDSPEventDelegate* m_dspDelegate; //< dsp event delegate
};

#endif // __DSP_PARSER_BASS_H__