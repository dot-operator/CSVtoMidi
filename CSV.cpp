#include "CSV.h"
#include <fstream>
#include <algorithm>

bool CSV::loadCSV(const std::string& path)
{
	std::ifstream file(path);
	jassert(file.is_open());
	if (!file.is_open())
		return false;

	values.clear();

	std::string line, temp;
	while (std::getline(file, line))
	{
		// We only care about the last value in a line right now.
		if (line.find_last_of(',') != line.npos) {
			temp = line.substr(line.find_last_of(',') + 1);
			if (isdigit(temp[0])) {
				values.push_back(std::stof(temp));
			}
		}
	}
	file.close();
	cursor = values.begin();

	setBinMinMax(*std::min_element(std::begin(values), std::end(values)), 
		*std::max_element(std::begin(values), std::end(values)));

	return true;
}

CSV::CSV()
{
	// temporarily add a default set of notes
	bins.push_back(60); // 1
	bins.push_back(62); // 3
	bins.push_back(63); // 4
	bins.push_back(65); // 6
	bins.push_back(67); // 8
	bins.push_back(68); // 9
	bins.push_back(70); // 11

	for (int i = 0; i < 256; ++i) {
		values.push_back(0.f);
	}
	setBinMinMax(0.f, 2.f);
	bLoaded = true;
}

CSV::~CSV()
{
}

int CSV::getNextValue()
{
	constexpr float EPSILON = 0.0001f;

	if (cursor != values.end() && !bins.empty()) {
		float val = *cursor++;

		// Normalize the data
		val = std::max(val, fMin);
		val = std::min(val, fMax);

		if (val == fMin) {
			return -2;
		}

		val = ((val - fMin) / fMaxNorm) * (bins.size() + 1);
		//val -= EPSILON; // Biggest value should fit inside the bins still

		int binNumber = (int)val % bins.size();

		if (binNumber >= bins.size()) // shouldn't need this
			binNumber = bins.size() - 1;

		return bins[binNumber];
	}
	return -1;
}

void CSV::clearNoteBins()
{
	bins.clear();
}

void CSV::addNoteBin(int note)
{
	bins.push_back(note);
}

void CSV::setBinMinMax(float min, float max)
{
	// idiot proof setter
	fMin = std::min(min, max);
	fMax = std::max(min, max);
	fMaxNorm = fMax - fMin;
}

void CSV::resetCursor()
{
	cursor = values.begin();
}
