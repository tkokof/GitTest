/*!
    a simple test program of audio data reading
*/

#include <iostream>
#include <string>
#include <algorithm>

using namespace std;

#include "sndfile.hh"

int main()
{
	std::string filePath;
	// here we simple get the test audio file path
	cout << "Input audio file path\n";
	cin >> filePath;

	// then we try to read the audio file data
	SndfileHandle sndFile(filePath.c_str());
	// simple handle read error
	int err = sndFile.error();
	if (err != 0) 
	{
	    cout << "Error to read the audio file\n";
		return -1;
	}

	// else let's get some base audio data info
	
	// audio samplerate
	int samplerate = sndFile.samplerate();
	// audio channel
	int channel = sndFile.channels();
	// audio total frame
	int frames = (int)sndFile.frames();
	// audio format(this is just a int(enum) number in libsndfile)
	int format = sndFile.format();

	cout << "Audio Samplerate : " << samplerate << endl
		 << "Audio Channel    : " << channel << endl
		 << "Audio Frames     : " << frames << endl
		 << "Audio Format     : " << format << endl;

	// at last, we just try to read some frame data
	size_t itemCount = std::min(8, frames / channel);
	short* itemBuffer = new short[channel * itemCount];
	sndFile.read(itemBuffer, itemCount);

	cout << "Audio frames  : " << endl;
	for (size_t i = 0; i < itemCount; ++i)
	{
		for (size_t j = 0; j < size_t(channel); ++j)
		{
			cout << itemBuffer[j] << " ";
		}
		cout << endl;
	}

	// just do a simple console pause
	system("pause");
	// that's it
	return 0;
}