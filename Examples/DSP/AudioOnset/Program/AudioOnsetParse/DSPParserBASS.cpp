
#include <vector>

#include "DSPConfig.h"

#include "DSPParserBASS.h"

bool DSPParserBASS::DoDSPParse(const char* filename)
{
	// first load the file
	m_BASSStream = BASS_StreamCreateFile(false, filename, 0, 0, BASS_STREAM_DECODE|BASS_STREAM_PRESCAN);
	if (!m_BASSStream)
	{
		printf("Error to load audio file : %s", filename);
		return false;
	}

	// get the channel info
	BASS_CHANNELINFO info;
	BOOL ret = BASS_ChannelGetInfo(m_BASSStream, &info);
	if (!ret)
	{
		printf("Error to load audio info : %s", filename);
		return false;
	}
	
	// get audio info
	m_audioInfo.channel = info.chans;
	m_audioInfo.samplerate = info.freq;

	m_curFrameCount = 0;

	// dispatch begin event
    dispatchSpectrumEvent(EDE_SpectrumBegin, 0, NULL);

	// allocate buffer
	const size_t sizeCount = DSP_AUDIO_FRAME_PER_BLOCK / 2;
	float* spectrum = new float[sizeCount];
	
    // use BASS get spectrum data, more info see BASS doc
	DWORD fftFlag = getBassFFTFlag(DSP_AUDIO_FRAME_PER_BLOCK);
	DWORD spectrumRet = BASS_ChannelGetData(m_BASSStream, spectrum, fftFlag);

	while (spectrumRet != -1)
	{
		int spectrumCount = spectrumRet / sizeof(float);
		m_curFrameCount += spectrumCount;
		// dispatch parsing event here
		dispatchSpectrumEvent(EDE_Spectrum, spectrumCount, spectrum);
		spectrumRet = BASS_ChannelGetData(m_BASSStream, spectrum, fftFlag);
	}

	// dispatch end event
	dispatchSpectrumEvent(EDE_SpectrumEnd, 0, NULL);

	// release the bass stream here
	BASS_StreamFree(m_BASSStream);
	m_BASSStream = NULL;

	// release buffer
	delete[] spectrum;

	return true;

}

// get bass FFT flag
DWORD DSPParserBASS::getBassFFTFlag(size_t windowSize) const
{
	DWORD fftFlag = BASS_DATA_FFT4096; // default value
	switch (windowSize)
	{
	case 256:
		fftFlag = BASS_DATA_FFT256;
		break;
	case 512:
		fftFlag = BASS_DATA_FFT512;
		break;
	case 1024:
		fftFlag = BASS_DATA_FFT1024;
		break;
	case 2048:
		fftFlag = BASS_DATA_FFT2048;
		break;
	case 4096:
		fftFlag = BASS_DATA_FFT4096;
		break;
	case 8192:
		fftFlag = BASS_DATA_FFT8192;
		break;
	case 16384:
		fftFlag = BASS_DATA_FFT16384;
		break;
	}
	// other flags here, like BASS_DATA_FFT_INDIVIDUAL,BASS_DATA_FFT_NOWINDOW,BASS_DATA_FFT_REMOVEDC etc.
	return fftFlag;
}

// dispatch spectrum event
void DSPParserBASS::dispatchSpectrumEvent(EDSPEvent event, size_t spectrumCount, const float* spectrum)
{
	SDSPEventData spectrumEvent(event, m_audioInfo, m_curFrameCount, m_curFrameCount * 1.0f / m_audioInfo.samplerate, spectrumCount, spectrum);
	if (m_dspDelegate) m_dspDelegate->OnEvent(spectrumEvent);
}