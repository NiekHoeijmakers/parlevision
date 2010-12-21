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

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>
using namespace plv;
using namespace plvopencv;

#include <vector>
using std::vector;

Split::Split()
{
    m_inputPin = createCvMatDataInputPin( "multi_ch_image", this, IInputPin::CONNECTION_REQUIRED );
    m_outputPin0 = createCvMatDataOutputPin( "single_ch_image_0", this );
    m_outputPin1 = createCvMatDataOutputPin( "single_ch_image_1", this );
    m_outputPin2 = createCvMatDataOutputPin( "single_ch_image_2", this );
    m_outputPin3 = createCvMatDataOutputPin( "single_ch_image_3", this );
}

Split::~Split(){}
void Split::init(){}
void Split::deinit() throw (){}
void Split::start(){}
void Split::stop(){}

void Split::process()
{
    int i, chnls;

    plv::CvMatData inImg = m_inputPin->get();
    chnls = inImg.channels();

    if(chnls < 2)
    {
        qDebug() << "Input in the Split processor does not get a multi channel image";
        throw std::runtime_error("The input image is not a mutli channel image");
    }

    // open input images for reading
    const cv::Mat& in = inImg;

    //Create the vector with output data
    vector<CvMatData> outImgs;
    for(i = 0; i < chnls; ++i )
        outImgs.push_back(CvMatData::create(inImg.width(),
                                            inImg.height(),
                                            inImg.depth(),
                                            1));

    //open output images for writing
    vector<cv::Mat> out;
    for(i = 0; i < chnls; ++i )
        out.push_back( outImgs.at(i) );

    //Split the image in multiple images
    cv::split(in, out);

    // publish the new image based on the number of channels in the input image
    switch(chnls){
    case 4:
        m_outputPin3->put( outImgs.at(3) );
    case 3:
        m_outputPin2->put( outImgs.at(2) );
    case 2:
        m_outputPin1->put( outImgs.at(1) );
    default:
        m_outputPin0->put( outImgs.at(0) );
    }

}
