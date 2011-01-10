
/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvniek module of ParleVision.
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

#include "MedianSmooth.h"

#include <plvcore/CvMatData.h>
#include <plvcore/CvMatDataPin.h>
#include <plvcore/Util.h>

using namespace plv;
using namespace plvopencv;

MedianSmooth::MedianSmooth() :
        m_apertureSize(1)
{
    m_inputPin  = createCvMatDataInputPin( "image_input", this );
    m_outputPin = createCvMatDataOutputPin( "image_output", this );

    //Have to add the channel acceptance of only 8U 16U and 32F
}

MedianSmooth::~MedianSmooth(){}
void MedianSmooth::init(){}
void MedianSmooth::deinit() throw (){}
void MedianSmooth::start(){}
void MedianSmooth::stop(){}

void MedianSmooth::process()
{
    CvMatData srcPtr = m_inputPin->get();
    CvMatData dstPtr = CvMatData::create( srcPtr.properties() );

    //check for the correct depth
    if(m_apertureSize >= 7 && srcPtr.depth() != CV_8U){
        qDebug() << "With an aperture of 7 and higher an image depth of 8U is required.";
        return;
    }

    // open for reading
    const cv::Mat& src = srcPtr;

    // open image for writing
    cv::Mat& dst = dstPtr;

    // perform smooth operation on the image
    // Smoothes image using a median filter
    cv::medianBlur( src, dst, m_apertureSize );

    // publish the new image
    m_outputPin->put( dstPtr );
}

void MedianSmooth::setApertureSize(int ksize)
{
    QMutexLocker lock( m_propertyMutex );

    //aperture size must be odd and positive
    if (ksize < 1)
        ksize = 1;
    else if( Util::isEven(ksize) )
    {   //even: determine appropriate new odd value
        //we were increasing -- increase to next odd value
        if (ksize > m_apertureSize)
            ksize++;
        //we were decreasing -- decrease to next odd value
        else
            ksize--;
    }
    m_apertureSize = ksize;
    emit(apertureSizeChanged((m_apertureSize)));
}

int MedianSmooth::getApertureSize()
{
    QMutexLocker lock(m_propertyMutex);
    return m_apertureSize;
}
