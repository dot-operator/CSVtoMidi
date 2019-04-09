/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class CsvtoMidiAudioProcessorEditor  : public AudioProcessorEditor, public Button::Listener
{
public:
    CsvtoMidiAudioProcessorEditor (CsvtoMidiAudioProcessor&);
    ~CsvtoMidiAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

	void buttonClicked(Button* button);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    CsvtoMidiAudioProcessor& processor;

	// GUI
	TextButton btnLoadCSV;
	TextButton btnPlay;
	TextButton btnClear;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CsvtoMidiAudioProcessorEditor)
};
