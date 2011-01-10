
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

#include "DistanceTransform.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>

#include <stdexcept>

using namespace plv;
using namespace plvopencv;

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

DistanceTransform::DistanceTransform()
{
    m_inputPin = createCvMatDataInputPin( "8U_1CH_image", this, IInputPin::CONNECTION_REQUIRED );
    m_outputPin = createCvMatDataOutputPin( "32F_1CH_image", this );

    PLV_ENUM_ADD( m_distanceType, CV_DIST_L1 );
    PLV_ENUM_ADD( m_distanceType, CV_DIST_L2 );
    PLV_ENUM_ADD( m_distanceType, CV_DIST_C );

    PLV_ENUM_ADD( m_maskSize, 3 );
    PLV_ENUM_ADD( m_maskSize, 5 );
    PLV_ENUM_ADD( m_maskSize, CV_DIST_MASK_PRECISE );

    //Set channel properties acceptence.
}

DistanceTransform::~DistanceTransform(){}
void DistanceTransform::init(){}
void DistanceTransform::deinit() throw (){}
void DistanceTransform::start(){}
void DistanceTransform::stop(){}

plv::Enum DistanceTransform::getDistanceType()
{
    QMutexLocker lock( m_propertyMutex );
    return m_distanceType;
}

plv::Enum DistanceTransform::getMaskSize()
{
    QMutexLocker lock( m_propertyMutex );
    return m_maskSize;
}

void DistanceTransform::setDistanceType(plv::Enum e)
{
    QMutexLocker lock( m_propertyMutex );
    m_distanceType = e;

    if( ( m_distanceType.getSelectedValue() == CV_DIST_L1
         || m_distanceType.getSelectedValue() == CV_DIST_C )
         && m_maskSize.getSelectedValue() != 3 ){
        m_maskSize.setSelectedIndex(0);
        emit(maskSizeChanged(m_maskSize));
    }
    emit( distanceTypeChanged(m_distanceType));
}

void DistanceTransform::setMaskSize(plv::Enum e)
{
    QMutexLocker lock( m_propertyMutex );
    if( e.getSelectedValue() == 3 ||
          m_distanceType.getSelectedValue() == CV_DIST_L2){
        m_maskSize = e;
    }
    emit(maskSizeChanged(m_maskSize));
}

void DistanceTransform::process()
{
    //get the input image
    CvMatData img = m_inputPin->get();

    // open input images for reading
    const cv::Mat& src = img;

    if(img.channels() != 1)
        qDebug() << "Wrong number of channels";
    if(img.depth() != CV_8U && img.depth() != CV_8S)
        qDebug() << "Wrong depth in the image";

    //get a destination image with the distance values.
    CvMatData out = CvMatData::create( img.width(), img.height(), CV_32FC1 );
    // open output image for writing
    cv::Mat& dst = out;

    //DistanceTransform both images
    try{
        cv::distanceTransform(src, dst, m_distanceType.getSelectedValue(), m_maskSize.getSelectedValue());
    }
    catch(std::exception &e){
        //Don't know why but this try/catch block prevents exception troubles.
        qDebug() << "exception: " << e.what();
        throw;
    }

    // publish the new image
    m_outputPin->put( out );

}
