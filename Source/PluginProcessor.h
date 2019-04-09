/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "CSV.h"

//==============================================================================
/**
*/
class CsvtoMidiAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    CsvtoMidiAudioProcessor();
    ~CsvtoMidiAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	// CSV-specific methods
	bool loadCSV(const String& path);
	bool togglePlay();
	void resetBins();
	

private:
	// Should all be initialized in prepareToPlay.
	// Midi effect timing
	AudioParameterFloat *pSpeed;
	AudioParameterFloat *pLegato;
	AudioParameterFloat *pBinMin;
	AudioParameterFloat *pBinMax;
	AudioParameterBool *pPlaying;
	AudioParameterBool *pResetBins;
	AudioParameterInt *pSquelchZero;
	AudioParameterInt *pSustain;
	AudioParameterChoice *pSync;

	int iLastMidiNote;

	float fTimeElapsed;
	float fSampleRate;

	bool bArmed;
	bool bWasStopped = true;

	float rateFromSync(const juce::String& selection, int denominator);

	CSV csv;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsvtoMidiAudioProcessor)
};
