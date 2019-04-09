/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CsvtoMidiAudioProcessor::CsvtoMidiAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	addParameter(pSpeed = new AudioParameterFloat("speed", "Playback Rate", 16.f, 1.f, 5.f));
	addParameter(pLegato = new AudioParameterFloat("legato", "Note Length", 0.01f, 2.f, 0.1f));
	addParameter(pBinMin = new AudioParameterFloat("minval", "Minimum Value", -64.f, 127.f, 0.f));
	addParameter(pBinMax = new AudioParameterFloat("maxval", "Maximum Value", -64.f, 128.f, 2.f));
	addParameter(pSquelchZero = new AudioParameterInt("squelchzero", "Squelch Zeroes", 0, 1, 1));
	addParameter(pSustain = new AudioParameterInt("sustain", "Sustain on Equal Value", 0, 1, 0, "Sustain note on Equal value"));

	addParameter(pPlaying = new AudioParameterBool("play", "Play", false));
	addParameter(pResetBins = new AudioParameterBool("resetbins", "Remove Bins", false));
	
	addParameter(pSync = new AudioParameterChoice("sync", "Sync", { "Off", "1/2", "1/3", "1/4", "1/6", "1/8", "1/12", "1/16", "1/24", "1/32" }, 0));
}

CsvtoMidiAudioProcessor::~CsvtoMidiAudioProcessor()
{
}

//==============================================================================
const String CsvtoMidiAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CsvtoMidiAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CsvtoMidiAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CsvtoMidiAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CsvtoMidiAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CsvtoMidiAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int CsvtoMidiAudioProcessor::getCurrentProgram()
{
    return 0;
}

void CsvtoMidiAudioProcessor::setCurrentProgram (int index)
{
}

const String CsvtoMidiAudioProcessor::getProgramName (int index)
{
    return {};
}

void CsvtoMidiAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void CsvtoMidiAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
	fTimeElapsed = 0.f;
	fSampleRate = static_cast<float>(sampleRate);
	
	*pPlaying = false;
	bArmed = false;

	iLastMidiNote = 0;
}

void CsvtoMidiAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CsvtoMidiAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CsvtoMidiAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	if (*pResetBins) {
		resetBins();
		*pResetBins = false;
	}

	// Set Zero Squelching
	csv.setSquelchZeroes(pSquelchZero->get() == 1);
	
	// Set anything relevant to transport synchronization.
	float rate;
	if (pSync->getCurrentChoiceName() != "Off") {
		// We're synced to the DAW, so start/stop with it.
		AudioPlayHead::CurrentPositionInfo info;
		getPlayHead()->getCurrentPosition(info);
		*(pPlaying) = info.isPlaying;
		if (info.isPlaying) {
			if (bWasStopped) {
				bWasStopped = false;
				// determine the number of notes that we should have played so far
				float mult = rateFromSync(pSync->getCurrentChoiceName(), info.timeSigDenominator);
				unsigned numNotes = info.ppqPosition * mult;
				csv.setCursor(numNotes);
			}
			
		}
		else bWasStopped = true;

		rate = 1.f / (((float)info.bpm / 60.f) * rateFromSync(pSync->getCurrentChoiceName(), info.timeSigDenominator));
	}
	else {
		// Or prepare the alternative.
		rate = 1.f / pSpeed->get();
	}

	int ignore;
	MidiMessage msg;
	for (MidiBuffer::Iterator it(midiMessages); it.getNextEvent(msg, ignore);)
	{
		if (msg.isNoteOn()) {
			csv.addNoteBin(msg.getNoteNumber());
		}
		//else if (msg.isNoteOff()) notes.removeValue(msg.getNoteNumber());
	}
	midiMessages.clear();

	// Send a note off signal if we stop playing just in case.
	if (!*pPlaying) {
		if (iLastMidiNote != 0) {
			midiMessages.addEvent(MidiMessage::noteOff(1, iLastMidiNote, 0.5f), 0);
		}
		return;
	}

	// We should not have audio.
	jassert(buffer.getNumChannels() == 0);
	// Shouldn't be computing over zero time.
	jassert(buffer.getNumChannels() != 0);

	csv.setBinMinMax(*pBinMin, *pBinMax);

	// Use sample size in buffer to determine time progression.
	float fTimeThisFrame = ((float)buffer.getNumSamples() / fSampleRate);

	fTimeElapsed += fTimeThisFrame;
	
	if (fTimeElapsed > rate) {
		int val = csv.getNextValue();

		if (val == -1) {
			*pPlaying = false;
			midiMessages.addEvent(MidiMessage::noteOff(1, val, uint8(64)), 
				(fTimeElapsed - rate) + fSampleRate * pLegato->get());
			csv.resetCursor();
			return;
		}

		if ((val == -2) || (val == iLastMidiNote && (*pSustain == 1))) {
			// Value was squelched or we're sustaining the last note.
			fTimeElapsed = fmodf(fTimeElapsed, rate);
			return;
		}
		
		if (iLastMidiNote != 0) {
			midiMessages.addEvent(MidiMessage::noteOff(1, iLastMidiNote, 0.5f), fTimeElapsed - rate - 10.f);
		}
		midiMessages.addEvent(MidiMessage::noteOn(1, val, 0.5f), fTimeElapsed - rate);
		iLastMidiNote = val;
		
		fTimeElapsed = fmodf(fTimeElapsed, /*pSpeed->get()*/rate); // Only emit one note.
	}
}

//==============================================================================
bool CsvtoMidiAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CsvtoMidiAudioProcessor::createEditor()
{
    return new CsvtoMidiAudioProcessorEditor (*this);
}

//==============================================================================
void CsvtoMidiAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void CsvtoMidiAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

bool CsvtoMidiAudioProcessor::loadCSV(const String & path)
{
	if (csv.loadCSV(path.toStdString())) {
		*pBinMax = csv.getMax();
		*pBinMin = csv.getMin();
		bArmed = true;
		return true;
	}
	return false;
	return true;
}

bool CsvtoMidiAudioProcessor::togglePlay()
{
	if(!bArmed)
		return false;

	*pPlaying = !*pPlaying;
	if (!*pPlaying) {

	}
	return pPlaying;
}

void CsvtoMidiAudioProcessor::resetBins()
{
	csv.clearNoteBins();
}

float CsvtoMidiAudioProcessor::rateFromSync(const juce::String & selection, int denominator)
{
	// We expect the "selection" to be a string in the format 1/n
	// Denominator is the beat denominator
	// so if we get 1/4 and 4, we output 1; 1/16 and 4: 0.25
	std::string selString = selection.toStdString();
	if (selString.size() < 3 || (selString.find("1/") == selString.npos)) {
		// Should probably assert here or something.
		return 1.f;
	}
	
	int n = std::stoi(selString.substr(2));
	return ((float)n / (float)denominator);
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CsvtoMidiAudioProcessor();
}