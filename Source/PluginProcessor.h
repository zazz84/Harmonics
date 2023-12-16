/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
//==============================================================================
class SecondOrderHighPass
{
public:
	SecondOrderHighPass();

	void init(int sampleRate);
	void setCoef(float frequency, float Q);
	float process(float in);

protected:
	float m_sampleRate;
	float m_a0 = 0.9964426738810956f;
	float m_a1 = -1.9928853477621913f;
	float m_a2 = 0.9964426738810956f;
	float m_b0 = 1.0f;
	float m_b1 = -1.9928691659478477f;
	float m_b2 = 0.992901529576535f;

	float m_x1 = 0.0f;
	float m_x2 = 0.0f;
	float m_y1 = 0.0f;
	float m_y2 = 0.0f;
};

//==============================================================================
class HarmonicsAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    HarmonicsAudioProcessor();
    ~HarmonicsAudioProcessor() override;

	static const std::string paramsNames[];

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	using APVTS = juce::AudioProcessorValueTreeState;
	static APVTS::ParameterLayout createParameterLayout();

	APVTS apvts{ *this, nullptr, "Parameters", createParameterLayout() };

private:	
	//==============================================================================
	//std::atomic<float>* tiltParameter = nullptr;
	std::atomic<float>* gainParameter = nullptr;
	std::atomic<float>* mixParameter = nullptr;
	std::atomic<float>* volumeParameter = nullptr;

	juce::AudioParameterBool* button2Parameter = nullptr;
	//juce::AudioParameterBool* button3Parameter = nullptr;
	juce::AudioParameterBool* button4Parameter = nullptr;
	//juce::AudioParameterBool* button5Parameter = nullptr;
	juce::AudioParameterBool* button6Parameter = nullptr;
	//juce::AudioParameterBool* button7Parameter = nullptr;
	juce::AudioParameterBool* button8Parameter = nullptr;
	//juce::AudioParameterBool* button9Parameter = nullptr;

	SecondOrderHighPass m_secondOrderHighPass2[2] = {};
	SecondOrderHighPass m_secondOrderHighPass4[2] = {};
	SecondOrderHighPass m_secondOrderHighPass6[2] = {};
	SecondOrderHighPass m_secondOrderHighPass8[2] = {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HarmonicsAudioProcessor)
};
