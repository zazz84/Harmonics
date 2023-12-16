/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

const juce::Colour HarmonicsAudioProcessorEditor::veryLight  = juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.7f, 1.0f);
const juce::Colour HarmonicsAudioProcessorEditor::light      = juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.6f, 1.0f);
const juce::Colour HarmonicsAudioProcessorEditor::medium     = juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.5f, 1.0f);
const juce::Colour HarmonicsAudioProcessorEditor::dark       = juce::Colour::fromHSV(HUE * 0.01f, 0.5f, 0.4f, 1.0f);

//==============================================================================
HarmonicsAudioProcessorEditor::HarmonicsAudioProcessorEditor (HarmonicsAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), audioProcessor (p), valueTreeState(vts)
{
	getLookAndFeel().setColour(juce::Slider::thumbColourId, dark);
	getLookAndFeel().setColour(juce::Slider::rotarySliderFillColourId, medium);
	getLookAndFeel().setColour(juce::Slider::rotarySliderOutlineColourId, light);

	for (int i = 0; i < N_SLIDERS_COUNT; i++)
	{
		auto& label = m_labels[i];
		auto& slider = m_sliders[i];

		//Lable
		label.setText(HarmonicsAudioProcessor::paramsNames[i], juce::dontSendNotification);
		label.setFont(juce::Font(24.0f * 0.01f * SCALE, juce::Font::bold));
		label.setJustificationType(juce::Justification::centred);
		addAndMakeVisible(label);

		//Slider
		slider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
		slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
		addAndMakeVisible(slider);
		m_sliderAttachment[i].reset(new SliderAttachment(valueTreeState, HarmonicsAudioProcessor::paramsNames[i], slider));
	}

	// Buttons
	addAndMakeVisible(harmonics2Button);
	addAndMakeVisible(harmonics4Button);
	addAndMakeVisible(harmonics6Button);
	addAndMakeVisible(harmonics8Button);

	harmonics2Button.setClickingTogglesState(true);
	harmonics4Button.setClickingTogglesState(true);
	harmonics6Button.setClickingTogglesState(true);
	harmonics8Button.setClickingTogglesState(true);

	button2Attachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "Button2", harmonics2Button));
	button4Attachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "Button4", harmonics4Button));
	button6Attachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "Button6", harmonics6Button));
	button8Attachment.reset(new juce::AudioProcessorValueTreeState::ButtonAttachment(valueTreeState, "Button8", harmonics8Button));

	/*harmonics2Button.setColour(juce::TextButton::buttonColourId, light);
	harmonics4Button.setColour(juce::TextButton::buttonColourId, light);
	harmonics6Button.setColour(juce::TextButton::buttonColourId, light);
	harmonics8Button.setColour(juce::TextButton::buttonColourId, light);

	harmonics2Button.setColour(juce::TextButton::buttonOnColourId, dark);
	harmonics4Button.setColour(juce::TextButton::buttonOnColourId, dark);
	harmonics6Button.setColour(juce::TextButton::buttonOnColourId, dark);
	harmonics8Button.setColour(juce::TextButton::buttonOnColourId, dark);*/

	setSize((int)(SLIDER_WIDTH * 0.01f * SCALE * N_SLIDERS_COUNT), (int)((SLIDER_WIDTH + BOTTOM_MENU_HEIGHT) * 0.01f * SCALE));
}

HarmonicsAudioProcessorEditor::~HarmonicsAudioProcessorEditor()
{
}

void HarmonicsAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(veryLight);
}

void HarmonicsAudioProcessorEditor::resized()
{
	// Sliders + Menus
	int width = getWidth() / N_SLIDERS_COUNT;
	int height = SLIDER_WIDTH * 0.01f * SCALE;
	juce::Rectangle<int> rectangles[N_SLIDERS_COUNT];

	for (int i = 0; i < N_SLIDERS_COUNT; ++i)
	{
		rectangles[i].setSize(width, height);
		rectangles[i].setPosition(i * width, 0);
		m_sliders[i].setBounds(rectangles[i]);

		rectangles[i].removeFromBottom((int)(LABEL_OFFSET * 0.01f * SCALE));
		m_labels[i].setBounds(rectangles[i]);
	}

	// Buttons
	const int posY = height + (int)(BOTTOM_MENU_HEIGHT * 0.01f * SCALE * 0.25f);
	const int buttonHeight = (int)(BOTTOM_MENU_HEIGHT * 0.01f * SCALE * 0.5f);
	const int center = (int)(getWidth() * 0.5f);

	harmonics2Button.setBounds((int)(center - buttonHeight * 2.4f), posY, buttonHeight, buttonHeight);
	harmonics4Button.setBounds((int)(center - buttonHeight * 1.2f), posY, buttonHeight, buttonHeight);
	harmonics6Button.setBounds((int)(center + buttonHeight * 0.0f), posY, buttonHeight, buttonHeight);
	harmonics8Button.setBounds((int)(center + buttonHeight * 1.2f), posY, buttonHeight, buttonHeight);
}