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

#include "Split.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>
#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Split::Split()
{
    m_inputPin = createInputPin<OpenCVImage>( "multi_ch_image", this, IInputPin::INPUT_REQUIRED );
    m_outputPin0 = createOutputPin<OpenCVImage>( "single_ch_image_0", this );
    m_outputPin1 = createOutputPin<OpenCVImage>( "single_ch_image_1", this );
    m_outputPin2 = createOutputPin<OpenCVImage>( "single_ch_image_2", this );
    m_outputPin3 = createOutputPin<OpenCVImage>( "single_ch_image_3", this );
}

Split::~Split(){}
void Split::init() throw (PipelineException){}
void Split::deinit() throw (){}
void Split::start() throw (PipelineException){}
void Split::stop() throw (PipelineException){}

void Split::process()
{
    assert(m_inputPin != 0);
    assert(m_outputPin0 != 0);
    assert(m_outputPin1 != 0);
    assert(m_outputPin2 != 0);
    assert(m_outputPin3 != 0);

    RefPtr<OpenCVImage> img = m_inputPin->get();


    // open input images for reading
    const IplImage* iplImgIn = img->getImage();

    if(iplImgIn->nChannels < 2 || iplImgIn->depth != IPL_DEPTH_8U){
        throw std::runtime_error("The input image is not a mutli channel image or doesn't have a 8 bit unsigned channels");
    }


    //First create image null-pointers. Because if the image doesn't have 4 channels
    //the rest of the channels will have to be null pointers.
    //plv
    RefPtr<OpenCVImage> imgOut0 = NULL;
    RefPtr<OpenCVImage> imgOut1 = NULL;
    RefPtr<OpenCVImage> imgOut2 = NULL;
    RefPtr<OpenCVImage> imgOut3 = NULL;
    //opencv
    IplImage* iplImgOut0 = NULL;
    IplImage* iplImgOut1 = NULL;
    IplImage* iplImgOut2 = NULL;
    IplImage* iplImgOut3 = NULL;

    // get new output images of same depth and size as input image and
    // open output images for writing dependent on the number of channels in the input
    switch(iplImgIn->nChannels){
    case 4:
        imgOut3 = OpenCVImageFactory::instance()->get(
                    img->getWidth(), img->getHeight(), img->getDepth(), 1);
        iplImgOut3 = imgOut3->getImageForWriting();
    case 3:
        imgOut2 = OpenCVImageFactory::instance()->get(
                    img->getWidth(), img->getHeight(), img->getDepth(), 1);
        iplImgOut2 = imgOut2->getImageForWriting();
    case 2:
        imgOut1 = OpenCVImageFactory::instance()->get(
                    img->getWidth(), img->getHeight(), img->getDepth(), 1);
        iplImgOut1 = imgOut1->getImageForWriting();
    default:
        imgOut0 = OpenCVImageFactory::instance()->get(
                    img->getWidth(), img->getHeight(), img->getDepth(), 1);
        iplImgOut0 = imgOut0->getImageForWriting();
    }

    //Split the image in multiple images
    cvSplit(iplImgIn, iplImgOut0, iplImgOut1, iplImgOut2, iplImgOut3);

    // publish the new image based on the number of channels in the input image
    switch(iplImgIn->nChannels){
    case 4:
        m_outputPin3->put( imgOut3.getPtr() );
    case 3:
        m_outputPin2->put( imgOut2.getPtr() );
    case 2:
        m_outputPin1->put( imgOut1.getPtr() );
    default:
        m_outputPin0->put( imgOut0.getPtr() );
    }

}
