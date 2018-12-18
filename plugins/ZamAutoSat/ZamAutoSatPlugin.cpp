/*
 * ZamAutoSat Automatic saturator
 * Copyright (C) 2014  Damien Zammit <damien@zamaudio.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#include "ZamAutoSatPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------

ZamAutoSatPlugin::ZamAutoSatPlugin()
    : Plugin(paramCount, 0, 0) // 0 programs, 0 states
{
    // reset
    deactivate();
}

ZamAutoSatPlugin::~ZamAutoSatPlugin()
{
}

// -----------------------------------------------------------------------
// Init

void ZamAutoSatPlugin::initParameter(uint32_t, Parameter&)
{
}

// -----------------------------------------------------------------------
// Internal data

float ZamAutoSatPlugin::getParameterValue(uint32_t) const
{
	return 0.0f;
}

void ZamAutoSatPlugin::setParameterValue(uint32_t, float)
{
}

// -----------------------------------------------------------------------
// Process

void ZamAutoSatPlugin::activate()
{
}

void ZamAutoSatPlugin::deactivate()
{
}

void ZamAutoSatPlugin::run(const float** inputs, float** outputs, uint32_t frames)
{
	uint32_t i;
	for (i = 0; i < frames; i++) {
		float x = inputs[0][i];
		outputs[0][i] = x;
		outputs[0][i] = 2.0*x*(1.0 - fabsf(x)*0.5);
	}
}

// -----------------------------------------------------------------------

Plugin* createPlugin()
{
    return new ZamAutoSatPlugin();
}

// -----------------------------------------------------------------------

END_NAMESPACE_DISTRHO
