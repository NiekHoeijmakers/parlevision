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

#ifndef MEDIANSMOOTH_H
#define MEDIANSMOOTH_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /*
    Smoothes image using a Median filter
    Parameters:

        * src – The source image
        * dst – The destination image; will have the same size and the same type as src
        * ksize – The aperture linear size. It must be odd and more than 1, i.e. 3, 5, 7 ...
    */
    class MedianSmooth : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( MedianSmooth )
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Median Smooth")
        Q_CLASSINFO("description", "Smooths and image using Median smoothing"
                    "See OpenCV reference for meaning of parameters."
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/imgproc_image_filtering.html#cv-medianblur'>"
                    "http://opencv.willowgarage.com/documentation/cpp/imgproc_image_filtering.html#cv-medianblur"
                    "</a>")

        Q_PROPERTY( int apertureSize READ getApertureSize WRITE setApertureSize NOTIFY apertureSizeChanged )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        MedianSmooth();
        virtual ~MedianSmooth();

        /** propery methods */
        int getApertureSize();

    signals:
        void apertureSizeChanged(int ksize);

    public slots:
        void setApertureSize(int ksize);

    private:
        plv::CvMatDataInputPin* m_inputPin;
        plv::CvMatDataOutputPin* m_outputPin;

        int m_apertureSize;

    };
}
#endif // MEDIANSMOOTH_H
