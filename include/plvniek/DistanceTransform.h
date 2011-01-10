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
#ifndef DISTANCETRANSFORM_H
#define DISTANCETRANSFORM_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * This processor takes one image and then calculates the distance to the
      * closest zero pixel for all non-zero pixels of the input image. The cal-
      * culated distance is then output to the output pin.
      */
    class DistanceTransform : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( DistanceTransform )
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Distance Transformation")
        Q_CLASSINFO("description", "A processor that calculates the distance of a non-zero pixel to the closest zero pixel.<br> "
                    "<b>requirement:</b> The input image is an 8-bit single channel image (preferably binary).<br> "
                    "See OpenCV reference for meaning of parameters. "
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/miscellaneous_image_transformations.html#cv-distancetransform'>"
                    "http://opencv.willowgarage.com/documentation/cpp/miscellaneous_image_transformations.html#cv-distancetransform"
                    "</a>");

        Q_PROPERTY( plv::Enum distanceType READ getDistanceType WRITE setDistanceType NOTIFY distanceTypeChanged  )
        Q_PROPERTY( plv::Enum maskSize READ getMaskSize WRITE setMaskSize NOTIFY maskSizeChanged  )
        //Q_PROPERTY( double uMaskValue1 READ getUMaskValue1 WRITE setUMaskValue1 NOTIFY uMaskValue1Changed  )
        //Q_PROPERTY( double uMaskValue2 READ getUMaskValue2 WRITE setUMaskValue2 NOTIFY uMaskValue2Changed  )
        //Q_PROPERTY( double uMaskValue3 READ getUMaskValue3 WRITE setUMaskValue3 NOTIFY uMaskValue3Changed  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT


    public:
        DistanceTransform();
        virtual ~DistanceTransform();

        /** property methods */
        plv::Enum getDistanceType();
        plv::Enum getMaskSize();
        //double getUMaskValue1() { return m_uMaskValue1; }
        //double getUMaskValue2() { return m_uMaskValue2; }
        //double getUMaskValue3() { return m_uMaskValue3; }

    signals:
        void distanceTypeChanged(plv::Enum newValue);
        void maskSizeChanged(plv::Enum newValue);
        //void uMaskValue1Changed(double newValue);
        //void uMaskValue2Changed(double newValue);
        //void uMaskValue3Changed(double newValue);

    public slots:
        void setDistanceType(plv::Enum e);
        void setMaskSize(plv::Enum e);
        //void setUMaskValue1(double f) { m_uMaskValue1 = f; emit(uMaskValue1Changed(m_uMaskValue1)); }
        //void setUMaskValue2(double f) { m_uMaskValue2 = f; emit(uMaskValue2Changed(m_uMaskValue2)); }
        //void setUMaskValue3(double f) { m_uMaskValue3 = f; emit(uMaskValue3Changed(m_uMaskValue3)); }


    private:
        plv::CvMatDataInputPin* m_inputPin; // image input
        plv::CvMatDataOutputPin* m_outputPin; // image output (32-bit floating-point single-channel)

        /** property variables */
        plv::Enum m_distanceType;
        plv::Enum m_maskSize;
        //double m_uMaskValue1;
        //double m_uMaskValue2;
        //double m_uMaskValue3;

    };//class DistanceTransform
}//namespace plvopencv


#endif // DISTANCETRANSFORM_H
