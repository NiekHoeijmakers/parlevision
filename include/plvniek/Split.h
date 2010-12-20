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
#ifndef SPLIT_H
#define SPLIT_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVImage;

    /**
      * Take a bitwise XOR of two images .
      */
    class Split : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Split")
        Q_CLASSINFO("description", "A processor that splits a mutlichannel image into several single channel images."
                        " The number it will be split in is dependent on the number of channels in the image.");

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        Split();
        virtual ~Split();

    private:
        plv::InputPin<OpenCVImage>* m_inputPin; // multi channel image input
        plv::OutputPin<OpenCVImage>* m_outputPin0; // single channel image output 0
        plv::OutputPin<OpenCVImage>* m_outputPin1; // single channel image output 1
        plv::OutputPin<OpenCVImage>* m_outputPin2; // single channel image output 2
        plv::OutputPin<OpenCVImage>* m_outputPin3; // single channel image output 3
    };//class Split
}//namespace plvopencv

#endif // SPLIT_H
