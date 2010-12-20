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
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVImage;

    /**
      * Take a bitwise ImageTypeScaleConversion of two images .
      */
    class ImageTypeScaleConversion : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Type and Scale Conversion")
        Q_CLASSINFO("description", "A processor that can convert the type of an image, scale the image and/or shift the image.");

        Q_PROPERTY( plv::Enum type READ getType WRITE setType NOTIFY typeChanged  )
        Q_PROPERTY( double scale READ getScale WRITE setScale NOTIFY scaleChanged  )
        Q_PROPERTY( double shift READ getShift WRITE setShift NOTIFY shiftChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageTypeScaleConversion();
        virtual ~ImageTypeScaleConversion();

        /** property methods */
        plv::Enum getType(){ return m_type; }
        double getScale() { return m_scale; }
        double getShift() { return m_shift; }

    signals:
        void typeChanged(plv::Enum newValue);
        void scaleChanged(double newValue);
        void shiftChanged(double newValue);

    public slots:
        void setType(plv::Enum e){ m_type = e; emit(typeChanged(m_type)); }
        void setScale(double f) { m_scale = f; emit(scaleChanged(m_scale)); }
        void setShift(double f) { m_shift = f; emit(shiftChanged(m_shift)); }

    private:
        plv::InputPin<OpenCVImage>* m_inputPin; // image input
        plv::OutputPin<OpenCVImage>* m_outputPin; // image output

        plv::Enum m_type;
        double m_scale;
        double m_shift;

    };//class ImageTypeScaleConversion
}//namespace plvopencv



#endif // IMAGETYPESCALECONVERSION_H
