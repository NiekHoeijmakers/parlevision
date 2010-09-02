/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
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
#include "Pin.h"
#include "OpenCVImage.h"
#include "ActivationTrigger.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "image_input"
#define OUTPUT_PIN_NAME "trigger_output"

/**
 * Constructor.
 */
Trigger::Trigger() :
        m_activate(false),
        m_continuous(false)
{
    m_inputPin = createInputPin<OpenCVImage>(INPUT_PIN_NAME, this, IInputPin::REQUIRED );
    m_outputPin = createOutputPin<ActivationTrigger>( OUTPUT_PIN_NAME, this );
}

/**
 * Destructor.
 */
Trigger::~Trigger(){}

/** Mandatory methods */
void Trigger::init() throw (PipelineException)
{
    /* Has nothing to do here. Yet? */
}

//virtual bool isBootstrapped() const;
bool Trigger::isReadyForProcessing() const
{
    return m_inputPin->getConnection()->hasData();
}

void Trigger::process()
{
    assert(m_inputPin != 0);
    RefPtr<OpenCVImage> img = m_inputPin ->get();

    if(!img->isNull())
    {
        //send a trigger to the output with the value of activate
        RefPtr<ActivationTrigger> trig = ATriggerFactory::instance()->get(m_activate);

        m_outputPin->put( trig.getPtr() );
        //check if it is continuous or not
        if(m_activate && !m_continuous) setActivate(false);
    }
}
