/*
 * ZamGrains Granular Delay
 * Copyright (C) 2018  Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#include "ZamGrainsPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

ZamGrainsPlugin::ZamGrainsPlugin()
    : Plugin(paramCount, 1, 0)
{
    // set default values
    loadProgram(0);
}

// -----------------------------------------------------------------------
// Init

void ZamGrainsPlugin::initParameter(uint32_t index, Parameter& parameter)
{
    switch (index)
    {
    case paramDelaytime:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Loop time";
        parameter.symbol     = "time";
        parameter.unit       = "ms";
        parameter.ranges.def = 160.0f;
        parameter.ranges.min = 5.0f;
        parameter.ranges.max = 1000.0f;
        break;
    case paramGrains:
        parameter.hints      = kParameterIsAutomable | kParameterIsInteger;
        parameter.name       = "Grains";
        parameter.symbol     = "grains";
        parameter.unit       = " ";
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 1.0f;
        parameter.ranges.max = 100.0f;
        break;
    case paramGrainspeed:
        parameter.hints      = kParameterIsAutomable | kParameterIsLogarithmic;
        parameter.name       = "Grain Speed";
        parameter.symbol     = "gs";
        parameter.unit       = " ";
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.1f;
        parameter.ranges.max = 20.f;
        break;
    case paramPlayspeed:
        parameter.hints      = kParameterIsAutomable | kParameterIsLogarithmic;
        parameter.name       = "Play Speed";
        parameter.symbol     = "ps";
        parameter.unit       = " ";
        parameter.ranges.def = 1.0f;
        parameter.ranges.min = 0.1f;
        parameter.ranges.max = 20.f;
        break;
    case paramGain:
        parameter.hints      = kParameterIsAutomable;
        parameter.name       = "Output Gain";
        parameter.symbol     = "gain";
        parameter.unit       = "dB";
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = -60.0f;
        parameter.ranges.max = 0.0f;
        break;
    }
}


void ZamGrainsPlugin::initProgramName(uint32_t index, String& programName)
{
	switch(index) {
	case 0:
		programName = "Zero";
		break;
	}
}

void ZamGrainsPlugin::loadProgram(uint32_t index)
{
	switch(index) {
	case 0:
		delaytime = 1000.f;
		grainspeed = 1.f;
		playspeed = 1.f;
		grains = 1.f;
		gain = 0.f;
		break;
	}

	activate();
}

// -----------------------------------------------------------------------
// Internal data

float ZamGrainsPlugin::getParameterValue(uint32_t index) const
{
    switch (index)
    {
    case paramDelaytime:
        return delaytime;
        break;
    case paramGrains:
        return grains;
        break;
    case paramGrainspeed:
        return grainspeed;
        break;
    case paramPlayspeed:
        return playspeed;
        break;
    case paramGain:
        return gain;
        break;
    default:
        return 0.0f;
    }
}

void ZamGrainsPlugin::setParameterValue(uint32_t index, float value)
{
    switch (index)
    {
    case paramDelaytime:
        delaytime = value;
        break;
    case paramGrains:
        grains = value;
        break;
    case paramGrainspeed:
        grainspeed = value;
        break;
    case paramPlayspeed:
        playspeed = value;
        break;
    case paramGain:
        gain = value;
        break;
    }
}

// -----------------------------------------------------------------------
// Process

void ZamGrainsPlugin::activate()
{
	int i;
	for (i = 0; i < MAX_DELAY; i++) {
		z[i] = 0.f;
	}
	posz = 0;
	posrate = 0;
	posphasor = 0;
	currgrains = 0;
	samphold = 0;
	samphold2 = 0;
	zidx = 0;
	zidx2 = 0;
	zidxold = 0;
	zidx2old = 0;
}

float ZamGrainsPlugin::sample_and_hold(int ctrl, float input, int *state) {
	if (ctrl == 0)
		*state = input;
	return *state;
}

float ZamGrainsPlugin::hanning(int pos, int windowsize)
{
	float s = sinf(M_PI * (float)pos / (float)(windowsize - 1));
	return s*s;
}

void ZamGrainsPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
	uint32_t i;
	float srate = getSampleRate();
	int delaysamples;
	float sampz, sampz2;
	float xfade;

	int windowsize;
	int outofphase;

	delaysamples = (int)(delaytime * srate) / 1000;
	windowsize = delaysamples / grains;

	for (i = 0; i < frames; i++) {
		z[posz] = inputs[0][i];
		outofphase = (posphasor + windowsize / 2) % windowsize;
		zidx = (int)(sample_and_hold(posphasor, (float)posz * playspeed, &samphold) + (float)posphasor * grainspeed);
		zidx2 = (int)(sample_and_hold(outofphase, (float)posz * playspeed, &samphold2) + (float)outofphase * grainspeed);

		if (++posphasor >= (unsigned int)windowsize) {
			posphasor = 0;
		}

		if (zidx >= delaysamples) {
			zidx %= (int)(delaysamples);
		}

		if (zidx2 >= delaysamples) {
			zidx2 %= (int)(delaysamples);
		}

		if (++posz >= (unsigned int)delaysamples) {
			posz = 0;
		}

		
		xfade = 1.0f / (float)frames;
		sampz = z[zidxold];
		sampz2 = z[zidx2old];
		sampz *= (1.-xfade);
		sampz2 *= (1.-xfade);
		sampz += z[zidx] * xfade;
		sampz2 += z[zidx2] * xfade;
		sampz = sanitize_denormal(sampz);
		sampz2 = sanitize_denormal(sampz2);

		outputs[0][i] = from_dB(gain) * (
					sampz * hanning(posphasor, windowsize) +
					sampz2 * hanning(outofphase, windowsize)
		);
		
		zidxold = zidx;
		zidx2old = zidx2;
	}
	delaytimeold = delaytime;
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new ZamGrainsPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
