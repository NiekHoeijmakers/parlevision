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

#include "ImageFlip.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include <opencv/cv.h>

using namespace plv;

#define INPUT_PIN_NAME "input"
#define OUTPUT_PIN_NAME "output"

ImageFlip::ImageFlip() :
        m_flipX(true),
        m_flipY(true),
        m_method(-1)
{
    m_inputPin = createInputPin<OpenCVImage>( INPUT_PIN_NAME, this );
    m_outputPin = createOutputPin<OpenCVImage>( OUTPUT_PIN_NAME, this );
}

ImageFlip::~ImageFlip()
{
}

void ImageFlip::init() throw (PipelineException)
{
}

void ImageFlip::start() throw (PipelineException)
{
}

void ImageFlip::stop() throw (PipelineException)
{
}

bool ImageFlip::isReadyForProcessing() const
{
    return m_inputPin->hasData();
}

void ImageFlip::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    RefPtr<OpenCVImage> imgIn = m_inputPin->get();

    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            imgIn->getWidth(), imgIn->getHeight(), imgIn->getDepth(), imgIn->getNumChannels() );


    // open for reading
    const IplImage* iplImgIn = imgIn->getImage();

    // open image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();

    // do a flip of the image
    if (m_flipX || m_flipY) cvFlip( iplImgIn, iplImgOut, m_method);
    else cvCopy( iplImgIn, iplImgOut);

    // publish the new image
    m_outputPin->put( imgOut.getPtr() );

}



void ImageFlip::setFlipX(bool b)
{
    if (b)
    {
        if (m_flipY) m_method = -1;
        else m_method=0;
    }
    else
    {
        if (m_flipY) m_method = 1;
        else m_method=0;
    }


    m_flipX = b;
    emit(flipXChanged(m_flipX));
}

void ImageFlip::setFlipY(bool b)
{
    if (b)
    {
        if (m_flipX) m_method = -1;
        else m_method=1;
    }
    else
    {
        if (m_flipX) m_method = 0;
        else m_method=1;
    }


    m_flipY = b;
    emit(flipYChanged(m_flipY));
}
