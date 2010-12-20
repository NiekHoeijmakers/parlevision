/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvopencv module of ParleVision.
  *
  * ParleVision is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * ParleVision is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * A copy of the GNU General Public License can be found in the root
  * of this software package directory in the file LICENSE.LGPL.
  * If not, see <http://www.gnu.org/licenses/>.
  */

#include <QDebug>

#include "Trigger.h"
#include <plvcore/Pin.h>
#include "OpenCVImage.h"
#include "ActivationTrigger.h"
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

/**
 * Constructor.
 */
ActivationTrigger::ActivationTrigger() :
        m_activate(false),
        m_continuous(false)
{
    m_inputPin = createInputPin<OpenCVImage>("image_input", this, IInputPin::INPUT_REQUIRED );
    m_outputPin = createOutputPin<Trigger>( "trigger_output", this );
}

/**
 * Destructor.
 */
ActivationTrigger::~ActivationTrigger(){}

/** Mandatory methods. Has nothing to do here. Yet? */
void ActivationTrigger::init() throw (PipelineException){}
void ActivationTrigger::deinit() throw(){}
void ActivationTrigger::start() throw (PipelineException){}
void ActivationTrigger::stop() throw (PipelineException){}


/**
 * Check if the image isn't NULL. Get a new activation trigger data, with
 * the same value as the activation property. If continuous isn't true then
 * the activation property is automatically set to false.
 */
void ActivationTrigger::process()
{
    assert(m_inputPin != 0);
    RefPtr<OpenCVImage> img = m_inputPin ->get();

    if(!img->isNull())
    {
        //send a trigger to the output with the value of activate
        RefPtr<Trigger> trig = new Trigger( m_activate );
        m_outputPin->put( trig.getPtr() );

        //check if it is continuous or not
        if(m_activate && !m_continuous) setActivate(false);
    }
}
