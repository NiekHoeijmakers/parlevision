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

#ifndef MULTIPLY_H
#define MULTIPLY_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVImage;

    /**
      * Does a per-element multiplication of two images and a scale factor.
      */
    class Multiply : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "A Mul B")
        Q_CLASSINFO("description", "A processor that does a per-element multiplication of two images and a scale factor.");

        Q_PROPERTY( double scale READ getScale WRITE setScale NOTIFY scaleChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        Multiply();
        virtual ~Multiply();

        /** propery methods */
        double getScale() { return m_scale; }

    signals:
        void scaleChanged(double newValue);

    public slots:
        void setScale(double d) {m_scale = d; emit(scaleChanged(m_scale));}

    private:
        plv::InputPin<OpenCVImage>* m_inputPin1; // image input A
        plv::InputPin<OpenCVImage>* m_inputPin2; // image input B
        plv::OutputPin<OpenCVImage>* m_outputPin; // image output

        double m_scale;
    };//class Multiply
}//namespace plvopencv



#endif // MULTIPLY_H
