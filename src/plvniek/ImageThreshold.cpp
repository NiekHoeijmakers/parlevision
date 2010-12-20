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

#include "ImageThreshold.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

/** Constructor **/
ImageThreshold::ImageThreshold() :
        m_threshold( 0.0 ),
        m_maxValue( 255.0 )//,
        //m_useOtsu( false )
{
    m_inputPin = createInputPin<OpenCVImage>( "image_input", this,  IInputPin::INPUT_REQUIRED );
    m_outputPin = createOutputPin<OpenCVImage>( "image_output", this );

    PLV_ENUM_ADD( m_method, CV_THRESH_BINARY );
    PLV_ENUM_ADD( m_method, CV_THRESH_BINARY_INV );
    PLV_ENUM_ADD( m_method, CV_THRESH_TRUNC );
    PLV_ENUM_ADD( m_method, CV_THRESH_TOZERO );
    PLV_ENUM_ADD( m_method, CV_THRESH_TOZERO_INV );

}

/** Destructor **/
ImageThreshold::~ImageThreshold(){}

/** Mandatory methods with nothing in them. **/
void ImageThreshold::init() throw (PipelineException){}
void ImageThreshold::deinit() throw (){}
void ImageThreshold::start() throw (PipelineException){}
void ImageThreshold::stop() throw (PipelineException){}

void ImageThreshold::process()
{
    //double l_method = 0.0;

    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    //Retrieve the image
    RefPtr<OpenCVImage> img = m_inputPin->get();
    //
    RefPtr<OpenCVImage> img2 = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );

    // open received image for reading
    const IplImage* imgScr = img->getImage();

    // open created image for writing
    IplImage* imgDst = img2->getImageForWriting();

    //The OTSU parameter does not work, it only crashes.
    /*if(m_useOtsu && img->getDepth() == IPL_DEPTH_8U){
        l_method = m_method.getSelectedValue() | CV_THRESH_OTSU;
    }
    else{
        l_method = m_method.getSelectedValue();
    }*/
    // perform threshold operation on the image
    cvThreshold( imgScr, imgDst, m_threshold, m_maxValue, m_method.getSelectedValue() );
    //cvThreshold( imgScr, imgDst, m_threshold, m_maxValue, l_method );

    // publish the new image
    m_outputPin->put( img2.getPtr() );
}
