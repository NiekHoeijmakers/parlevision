
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

#include "DistanceTransform.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>
#include <opencv/cv.h>

#include <stdexcept>

using namespace plv;
using namespace plvopencv;

#define PLV_ENUM_ADD( plvEnum, type ) plvEnum.add( #type, type )

DistanceTransform::DistanceTransform() :
        //m_useLabels(false),
        m_uMaskValue1(1.0),
        m_uMaskValue2(1.1),
        m_uMaskValue3(1.2)
{
    m_inputPin = createInputPin<OpenCVImage>( "8-bit_single-ch", this, IInputPin::INPUT_REQUIRED );
    m_outputPin1 = createOutputPin<OpenCVImage>( "32-bit_float_single-ch", this );
    //m_outputPin2 = createOutputPin<OpenCVImage>( "8-bit_integer_single-ch", this );

    PLV_ENUM_ADD( m_distanceType, CV_DIST_L1 );
    PLV_ENUM_ADD( m_distanceType, CV_DIST_L2 );
    PLV_ENUM_ADD( m_distanceType, CV_DIST_C );
    PLV_ENUM_ADD( m_distanceType, CV_DIST_USER );

    PLV_ENUM_ADD( m_maskSize, 3 );
    PLV_ENUM_ADD( m_maskSize, 5 );
}

DistanceTransform::~DistanceTransform(){}
void DistanceTransform::init() throw (PipelineException){}
void DistanceTransform::deinit() throw (){}
void DistanceTransform::start() throw (PipelineException){}
void DistanceTransform::stop() throw (PipelineException){}

void DistanceTransform::setDistanceType(plv::Enum e){
    m_distanceType = e;

    if( ( m_distanceType.getSelectedValue() == CV_DIST_L1
         || m_distanceType.getSelectedValue() == CV_DIST_C )
         && m_maskSize.getSelectedValue() != 3 ){
        //setMaskSize(plv::Enum(0));
        m_maskSize.setSelectedIndex(0);
        emit(maskSizeChanged(m_maskSize));
    }
    emit( distanceTypeChanged(m_distanceType));
}

void DistanceTransform::setMaskSize(plv::Enum e){
    if( e.getSelectedValue() == 3 ||
          ( m_distanceType.getSelectedValue() == CV_DIST_L2
          || m_distanceType.getSelectedValue() == CV_DIST_USER)){
        m_maskSize = e;
    }
    emit(maskSizeChanged(m_maskSize));
}

void DistanceTransform::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin1 != 0);
    //assert(m_outputPin2 != 0);

    //get the input image
    RefPtr<OpenCVImage> img = m_inputPin->get();

    //check format of images?
    if( img->getNumChannels() != 1
        || (img->getDepth() != IPL_DEPTH_8U
        && static_cast< unsigned int >( img->getDepth() ) != IPL_DEPTH_8S) )
    {
        throw std::runtime_error("The format of the input image is wrong. It needs to be an 8-bit single channel image.");
    }

    // open input images for reading
    const IplImage* iplImgIn = img->getImage();



    //get a destination image with the distance values.
    RefPtr<OpenCVImage> imgOut = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), IPL_DEPTH_32F, 1 );
    // open output image for writing
    IplImage* iplImgOut = imgOut->getImageForWriting();



    //get the image with labels if it is used.
    /*RefPtr<OpenCVImage> imgLabels = NULL;
    IplImage* iplImgLabels = NULL;

    if(m_useLabels){
        imgLabels = OpenCVImageFactory::instance()->get(
            img->getWidth(), img->getHeight(), img->getDepth(), img->getNumChannels() );
        iplImgLabels = imgLabels->getImageForWriting();
    }*/



    //setup mask if needed
    float* l_mask = NULL;
    if(m_distanceType.getSelectedValue() == CV_DIST_USER){
        if(m_maskSize.getSelectedValue() == 3){
            l_mask = new float[2];
            l_mask[0] = m_uMaskValue1;
            l_mask[1] = m_uMaskValue2;
        }
        else{
            l_mask = new float[3];
            l_mask[0] = m_uMaskValue1;
            l_mask[1] = m_uMaskValue2;
            l_mask[2] = m_uMaskValue3;
        }
    }



    //DistanceTransform both images
    /*cvDistTransform(iplImgIn, iplImgOut,
                        m_distanceType.getSelectedValue(),
                        m_maskSize.getSelectedValue(),
                        l_mask,
                        iplImgLabels);*/
    cvDistTransform(iplImgIn, iplImgOut,
                        m_distanceType.getSelectedValue(),
                        m_maskSize.getSelectedValue(),
                        l_mask,
                        NULL);



    // publish the new image
    m_outputPin1->put( imgOut.getPtr() );
    //if(m_useLabels)
    //    m_outputPin2->put( imgLabels.getPtr() );

    //delete l_mask;
}
