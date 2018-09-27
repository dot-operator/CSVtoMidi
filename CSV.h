#pragma once
#include <vector>
#include <string>

#include "../JuceLibraryCode/JuceHeader.h"

// Opens CSV files and outputs MIDI notes.
class CSV
{
public:
	bool loadCSV(const std::string & path);
	CSV();
	~CSV();

	inline float operator[](int i) {
		if (!bLoaded)
			return 0.f;
		return values[i];
	}

	inline size_t size() {
		return values.size();
	}

	int getNextValue();

	// Midi controls
	void clearNoteBins();
	void addNoteBin(int note);
	void setBinMinMax(float min, float max);
	void resetCursor();

private:
	bool bLoaded = false;
	std::vector<float> values;
	std::vector<float>::iterator cursor;
	std::vector<int> bins;

	float fMin = 0.f, fMax = 0.f;
	float fMaxNorm = 0.f;
};