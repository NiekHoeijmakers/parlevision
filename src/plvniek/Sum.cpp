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

#include "Sum.h"
#include "OpenCVImage.h"
#include "OpenCVScalar.h"

#include <plvcore/Pin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Sum::Sum()
{
    m_inputPin = createInputPin<OpenCVImage>( "image_input", this, IInputPin::INPUT_REQUIRED );
    m_outputPin = createOutputPin<OpenCVScalar>( "scalar_output", this );
}

Sum::~Sum(){}
void Sum::init() throw (PipelineException){}
void Sum::deinit() throw (){}
void Sum::start() throw (PipelineException){}
void Sum::stop() throw (PipelineException){}

void Sum::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    // open input images for reading
    RefPtr<OpenCVImage> img = m_inputPin->get();
    const IplImage* iplImgIn = img->getImage();

    //Sum of the image
    RefPtr<OpenCVScalar> result = new OpenCVScalar( cvSum(iplImgIn), img->getNumChannels() );

    /*qDebug() << "The result is [ " << result->getValueAtIndex(0)
            << " , " << result->getValueAtIndex(1)
            << " , " << result->getValueAtIndex(2)
            << " , " << result->getValueAtIndex(3) << " ]";*/

    // publish the result
    m_outputPin->put( result.getPtr() );
}

