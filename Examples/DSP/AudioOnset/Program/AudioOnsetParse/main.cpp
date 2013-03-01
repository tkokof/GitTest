/*!
    \brief main program
*/

#include <iostream>

#include "bass.h"

#include "DSPParserBASS.h"

#include "AudioAnalysisOnsetSpectralFlux.h"

int main()
{
	// first we init BASS library
	if (!BASS_Init(-1, 44100, 0, 0, NULL))
	{
		printf("can not init BASS library ...");
		return -1;
	}

	// set up dsp parsing
	DSPParserBASS dspParser;
    AudioAnalysisOnsetSpectralFlux onsetParser;
    
	// do dsp parsing
	dspParser.SetDSPEventDelegate(&onsetParser);
	dspParser.DoDSPParse("test.ogg");

	// output onset info
	size_t onsetCount = onsetParser.GetOnsetCount();
	std::cout << "Onset Count : " << onsetCount << std::endl;
	for (size_t i = 0; i < onsetCount; ++i)
	{
		std::cout << "Time : " << onsetParser.GetOnsetByIndex(i) << std::endl;
	}

	std::cout << "finish ...\n";

	// just do a simple console pause
	system("pause");

	return 0;
}