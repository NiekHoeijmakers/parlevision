
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

#define NEGATIVE    -1

#include <QDebug>

#include "ImageTypeScaleConversion.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>

using namespace plv;
using namespace plvopencv;

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

ImageTypeScaleConversion::ImageTypeScaleConversion() :
        m_scale(1.0),
        m_shift(0.0)
{
    m_inputPin = createCvMatDataInputPin( "image_input", this, IInputPin::CONNECTION_REQUIRED );
    m_outputPin = createCvMatDataOutputPin( "image_output", this );

    PLV_ENUM_ADD( m_type, NEGATIVE );
    PLV_ENUM_ADD( m_type, IPL_DEPTH_8U );
    PLV_ENUM_ADD( m_type, IPL_DEPTH_8S );
    PLV_ENUM_ADD( m_type, IPL_DEPTH_16U );
    PLV_ENUM_ADD( m_type, IPL_DEPTH_16S );
    PLV_ENUM_ADD( m_type, IPL_DEPTH_32S );
    PLV_ENUM_ADD( m_type, IPL_DEPTH_32F );
    PLV_ENUM_ADD( m_type, IPL_DEPTH_64F );
}

ImageTypeScaleConversion::~ImageTypeScaleConversion(){}
void ImageTypeScaleConversion::init(){}
void ImageTypeScaleConversion::deinit() throw (){}
void ImageTypeScaleConversion::start(){}
void ImageTypeScaleConversion::stop(){}

void ImageTypeScaleConversion::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin != 0);

    CvMatData img = m_inputPin->get();

        // open input images for reading
    const cv::Mat& src = img;


    //get a new output image of same depth and size as input image
    CvMatData out = CvMatData::create(img.width(), img.height(),
                                      m_type.getSelectedValue(), img.channels());

    // open output image for writing
    cv::Mat& dst = out;

    //Convert the image.
    src.convertTo(dst, m_type.getSelectedValue(), m_scale, m_shift);
    //cvConvertScale(iplImgIn, iplImgOut, m_scale, m_shift);

    // publish the new image
    m_outputPin->put( out );
}

plv::Enum ImageTypeScaleConversion::getType()
{
    QMutexLocker lock( m_propertyMutex );
    return m_type;
}

double ImageTypeScaleConversion::getScale()
{
    QMutexLocker lock( m_propertyMutex );
    return m_scale;
}

double ImageTypeScaleConversion::getShift()
{
    QMutexLocker lock( m_propertyMutex );
    return m_shift;
}

void ImageTypeScaleConversion::setType(plv::Enum e)
{
    QMutexLocker lock( m_propertyMutex );
    m_type = e;
}

void ImageTypeScaleConversion::setScale(double f)
{
    QMutexLocker lock( m_propertyMutex );
    m_scale = f;
}

void ImageTypeScaleConversion::setShift(double f)
{
    QMutexLocker lock( m_propertyMutex );
    m_shift = f;
}
