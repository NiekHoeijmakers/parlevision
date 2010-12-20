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
#ifndef SUM_H
#define SUM_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVImage;
    class OpenCVScalar;

    /**
      * Take a element-wise Sum of an images .
      */
    class Sum : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Sum A")
        Q_CLASSINFO("description", "A processor that calculates the sum of all the elements for each channel in the image.");

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        Sum();
        virtual ~Sum();

    private:
        plv::InputPin<OpenCVImage>* m_inputPin; // image input A
        plv::OutputPin<OpenCVScalar>* m_outputPin; // image output

        //void additionalProcess(OpenCVScalar&);
    };//class Sum
}//namespace plvopencv


#endif // SUM_H
