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
#ifndef IMAGETYPESCALECONVERSION_H
#define IMAGETYPESCALECONVERSION_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class ImageTypeScaleConversion : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Type conversion")
        Q_CLASSINFO("description", "A processor that can convert the type of an image. "
                    "Scaling and/or shifting the image colors is optional. "
                    "This processor uses cv::Mat::convertTo, see "
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/core_basic_structures.html#Mat::convertTo'>"
                    "OpenCV reference"
                    "</a> for meaning of parameters.");

        Q_PROPERTY( plv::Enum type READ getType WRITE setType NOTIFY typeChanged  )
        Q_PROPERTY( double scale READ getScale WRITE setScale NOTIFY scaleChanged  )
        Q_PROPERTY( double shift READ getShift WRITE setShift NOTIFY shiftChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageTypeScaleConversion();
        virtual ~ImageTypeScaleConversion();

        /** property methods */
        plv::Enum getType();
        double getScale();
        double getShift();

    signals:
        void typeChanged(plv::Enum newValue);
        void scaleChanged(double newValue);
        void shiftChanged(double newValue);

    public slots:
        void setType(plv::Enum e);
        void setScale(double f);
        void setShift(double f);

    private:
        plv::CvMatDataInputPin* m_inputPin; // image input
        plv::CvMatDataOutputPin* m_outputPin; // image output

        plv::Enum m_type;
        double m_scale;
        double m_shift;

    };//class ImageTypeScaleConversion
}//namespace plvopencv



#endif // IMAGETYPESCALECONVERSION_H
