/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
class HarmonicsAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    HarmonicsAudioProcessorEditor (HarmonicsAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~HarmonicsAudioProcessorEditor() override;

	// GUI setup
	static const int N_SLIDERS_COUNT = 3;
	static const int SCALE = 70;
	static const int LABEL_OFFSET = 25;
	static const int SLIDER_WIDTH = 200;
	static const int BOTTOM_MENU_HEIGHT = 50;
	static const int HUE = 75;

	static const int TYPE_BUTTON_GROUP = 1;

	// Colours
	static const juce::Colour veryLight;
	static const juce::Colour light;
	static const juce::Colour medium;
	static const juce::Colour dark;

    //==============================================================================
	void paint (juce::Graphics&) override;
    void resized() override;

	typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    HarmonicsAudioProcessor& audioProcessor;

	juce::AudioProcessorValueTreeState& valueTreeState;

	juce::Label m_labels[N_SLIDERS_COUNT] = {};
	juce::Slider m_sliders[N_SLIDERS_COUNT] = {};
	std::unique_ptr<SliderAttachment> m_sliderAttachment[N_SLIDERS_COUNT] = {};

	juce::Label automationTLabel;
	juce::Label smoothingTypeLabel;
	juce::Label detectionTypeLabel;

	juce::TextButton harmonics2Button{ "2" };
	juce::TextButton harmonics4Button{ "4" };
	juce::TextButton harmonics6Button{ "6" };
	juce::TextButton harmonics8Button{ "8" };

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> button2Attachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> button4Attachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> button6Attachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> button8Attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicsAudioProcessorEditor)
};
