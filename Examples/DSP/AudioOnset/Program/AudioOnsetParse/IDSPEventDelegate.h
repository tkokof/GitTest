/*!
    \file IDSPEventDelegate.h
	\brief here we defines some basic struct during dsp parsing
*/

#ifndef __IDSP_EVENT_DELEGATE_H__
#define __IDSP_EVENT_DELEGATE_H__

//! dsp stream event
enum EDSPEvent
{
	EDE_SpectrumBegin = 0, //< dsp spectrum begin event
	EDE_Spectrum, //< dsp spectrum event
	EDE_SpectrumEnd //< dsp spectrum event end
};

//! audio info struct
struct AudioInfo
{
	int samplerate; //< audio samplerate
	int channel; //< audio channel
	
	AudioInfo():samplerate(0),channel(0) {};
	AudioInfo(const AudioInfo& audioInfo):samplerate(audioInfo.samplerate),channel(audioInfo.channel) {};
	AudioInfo& operator = (const AudioInfo& audioInfo)
	{
		samplerate = audioInfo.samplerate;
		channel = audioInfo.channel;
		return *this;
	}
};

//! dsp event struct
struct SDSPEventData
{
	EDSPEvent event; //< event type
	AudioInfo audioInfo; //< audio info
	size_t curFrame; //< current audio frame
	float curTime; //< current audio frame time
	
	size_t spectrumCount; //< spectrum count
	const float* spectrumData; //< event extra data

	SDSPEventData(EDSPEvent event_, const AudioInfo& audioInfo_ = AudioInfo(), size_t curFrame_ = 0, float curTime_ = 0, size_t spectrumCount_ = 0, const float* spectrumData_ = NULL):
	event(event_),audioInfo(audioInfo_),curFrame(curFrame_),curTime(curTime_),spectrumCount(spectrumCount_),spectrumData(spectrumData_) {};
	SDSPEventData(const SDSPEventData& data):event(data.event),audioInfo(data.audioInfo),curFrame(data.curFrame),curTime(data.curTime),spectrumCount(data.spectrumCount),spectrumData(data.spectrumData) {};
	SDSPEventData& operator = (const SDSPEventData& data)
	{
		event = data.event;
		audioInfo = data.audioInfo;
		curFrame = data.curFrame;
		curTime = data.curTime;
		spectrumCount = data.spectrumCount;
		spectrumData = data.spectrumData;
		return *this;
	}
};

//! dsp event listener
class IDSPEventDelegate
{
public:
	virtual ~IDSPEventDelegate() {};
	//! dsp event handler
	virtual void OnEvent(const SDSPEventData& event) = 0;
};

#endif // __IDSP_EVENT_DELEGATE_H__