/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SecondOrderHighPass::SecondOrderHighPass()
{
}

void SecondOrderHighPass::init(int sampleRate)
{
	m_sampleRate = sampleRate;
}

void SecondOrderHighPass::setCoef(float frequency, float Q)
{
	if (m_sampleRate == 0)
	{
		return;
	}

	const float omega = frequency * (2.0f * 3.141593f / m_sampleRate);
	const float alpha = sinf(omega) / (2.0f * Q);
	const float cn = cosf(omega);

	m_a0 = 1.0f + alpha;
	m_a1 = -2.0f * cn;
	m_a2 = 1.0f - alpha;

	m_b0 = (1 + cn) / 2.0f;
	m_b1 = -1.0f * (1.0f + cn);
	m_b2 = m_b0;

	m_a1 /= m_a0;
	m_a2 /= m_a0;

	m_b0 /= m_a0;
	m_b1 /= m_a0;
	m_b2 /= m_a0;
}

float SecondOrderHighPass::process(float in)
{
	float y = m_b0 * in + m_b1 * m_x1 + m_b2 * m_x2 - m_a1 * m_y1 - m_a2 * m_y2;

	m_y2 = m_y1;
	m_y1 = y;
	m_x2 = m_x1;
	m_x1 = in;

	return y;
}
//==============================================================================

const std::string HarmonicsAudioProcessor::paramsNames[] = { "Gain", "Mix", "Volume" };

//==============================================================================
HarmonicsAudioProcessor::HarmonicsAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
	gainParameter   = apvts.getRawParameterValue(paramsNames[0]);
	mixParameter    = apvts.getRawParameterValue(paramsNames[1]);
	volumeParameter = apvts.getRawParameterValue(paramsNames[2]);

	button2Parameter = static_cast<juce::AudioParameterBool*>(apvts.getParameter("Button2"));
	button4Parameter = static_cast<juce::AudioParameterBool*>(apvts.getParameter("Button4"));
	button6Parameter = static_cast<juce::AudioParameterBool*>(apvts.getParameter("Button6"));
	button8Parameter = static_cast<juce::AudioParameterBool*>(apvts.getParameter("Button8"));
}

HarmonicsAudioProcessor::~HarmonicsAudioProcessor()
{
}

//==============================================================================
const juce::String HarmonicsAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HarmonicsAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HarmonicsAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HarmonicsAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HarmonicsAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HarmonicsAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HarmonicsAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HarmonicsAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HarmonicsAudioProcessor::getProgramName (int index)
{
    return {};
}

void HarmonicsAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HarmonicsAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	m_secondOrderHighPass2[0].init((int)sampleRate);
	m_secondOrderHighPass2[1].init((int)sampleRate);
	m_secondOrderHighPass4[0].init((int)sampleRate);
	m_secondOrderHighPass4[1].init((int)sampleRate);
	m_secondOrderHighPass6[0].init((int)sampleRate);
	m_secondOrderHighPass6[1].init((int)sampleRate);
	m_secondOrderHighPass8[0].init((int)sampleRate);
	m_secondOrderHighPass8[1].init((int)sampleRate);

	const float frequency = 20.0f;
	const float Q = 0.7f;

	m_secondOrderHighPass2[0].setCoef(frequency, Q);
	m_secondOrderHighPass2[1].setCoef(frequency, Q);
	m_secondOrderHighPass4[0].setCoef(frequency, Q);
	m_secondOrderHighPass4[1].setCoef(frequency, Q);
	m_secondOrderHighPass6[0].setCoef(frequency, Q);
	m_secondOrderHighPass6[1].setCoef(frequency, Q);
	m_secondOrderHighPass8[0].setCoef(frequency, Q);
	m_secondOrderHighPass8[1].setCoef(frequency, Q);
}

void HarmonicsAudioProcessor::releaseResources()
{
	
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HarmonicsAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void HarmonicsAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	// Get params
	const auto gain = juce::Decibels::decibelsToGain(gainParameter->load());
	const auto mix = mixParameter->load();
	const auto volume = juce::Decibels::decibelsToGain(volumeParameter->load());

	// Buttons
	const auto button2 = button2Parameter->get();
	const auto button4 = button4Parameter->get();
	const auto button6 = button6Parameter->get();
	const auto button8 = button8Parameter->get();

	// Gains
	const float gain2 = 2.0f;
	const float gain4 = 8.0f;
	const float gain6 = 16.0f;
	const float gain8 = 24.0f;

	// Mics constants
	const float mixInverse = 1.0f - mix;
	const int channels = getTotalNumOutputChannels();
	const int samples = buffer.getNumSamples();		

	for (int channel = 0; channel < channels; ++channel)
	{
		// Channel pointer
		auto* channelBuffer = buffer.getWritePointer(channel);

		auto& secondOrderHighPass2 = m_secondOrderHighPass2[channel];
		auto& secondOrderHighPass4 = m_secondOrderHighPass4[channel];
		auto& secondOrderHighPass6 = m_secondOrderHighPass6[channel];
		auto& secondOrderHighPass8 = m_secondOrderHighPass8[channel];

		for (int sample = 0; sample < samples; ++sample)
		{
			// Get input
			const float in = channelBuffer[sample] * gain;

			const float inDist2 = in * in * gain2;
			const float inFilter2 = secondOrderHighPass2.process(inDist2);

			const float inDist4 = inFilter2 * inFilter2 * gain4;
			const float inFilter4 = secondOrderHighPass4.process(inDist4);

			const float inDist6 = inFilter2 * inFilter4 * gain6;
			const float inFilter6 = secondOrderHighPass6.process(inDist6);

			const float inDist8 = inFilter4 * inFilter4 * gain8;
			const float inFilter8 = secondOrderHighPass8.process(inDist8);

			const float inDist = button2 * inFilter2 + button4 * inFilter4 + button6 * inFilter6 + button8 * inFilter8;
			const float inLimit = fmaxf(-1.0f, fminf(1.0f, inDist));

			// Apply volume, mix and send to output
			channelBuffer[sample] = volume * (mix * inLimit + mixInverse * in);
		}
	}
}

//==============================================================================
bool HarmonicsAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HarmonicsAudioProcessor::createEditor()
{
    return new HarmonicsAudioProcessorEditor (*this, apvts);
}

//==============================================================================
void HarmonicsAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{	
	auto state = apvts.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void HarmonicsAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(apvts.state.getType()))
			apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout HarmonicsAudioProcessor::createParameterLayout()
{
	APVTS::ParameterLayout layout;

	using namespace juce;

	layout.add(std::make_unique<juce::AudioParameterFloat>(paramsNames[0], paramsNames[0], NormalisableRange<float>(-24.0f, 24.0f,  0.1f, 1.0f), 0.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(paramsNames[1], paramsNames[1], NormalisableRange<float>(  0.0f,  1.0f, 0.05f, 1.0f), 1.0f));
	layout.add(std::make_unique<juce::AudioParameterFloat>(paramsNames[2], paramsNames[2], NormalisableRange<float>(-24.0f, 24.0f,  0.1f, 1.0f), 0.0f));

	layout.add(std::make_unique<juce::AudioParameterBool>("Button2", "Button2", true));
	layout.add(std::make_unique<juce::AudioParameterBool>("Button4", "Button4", false));
	layout.add(std::make_unique<juce::AudioParameterBool>("Button6", "Button6", false));
	layout.add(std::make_unique<juce::AudioParameterBool>("Button8", "Button8", false));

	return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HarmonicsAudioProcessor();
}
