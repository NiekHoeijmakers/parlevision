/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvcore module of ParleVision.
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

#ifndef SUB_H
#define SUB_H

#include <plvcore/PipelineProcessor.h>

namespace plv
{
    class OpenCVImage;
    class CvMatDataInputPin;
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    /**
      * Sub of two images.
      */
    class Sub : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( Sub )
        Q_CLASSINFO("author", "Ported from old version by Dennis")
        Q_CLASSINFO("name", "A sub B")
        Q_CLASSINFO("description", "A simple processor to subtract two images. See "
                    "<a href='http://opencv.willowgarage.com/documentation/cpp/"
                    "core_operations_on_arrays.html#subtract'>here</a> for details");

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        Sub();
        virtual ~Sub();

    private:
        plv::CvMatDataInputPin* m_inputPin1;
        plv::CvMatDataInputPin* m_inputPin2;
        plv::CvMatDataInputPin* m_inputPin3;
        plv::CvMatDataOutputPin* m_outputPin;
    };
}
#endif // SUB_H
