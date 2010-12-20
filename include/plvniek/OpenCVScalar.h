/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * Author: Niek Hoeijmakers
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

#ifndef OPENCVSCALAR_H
#define OPENCVSCALAR_H
#include <opencv/cv.h>

#include <plvcore/Types.h>
#include <plvcore/PlvExceptions.h>
#include "plvopencv_global.h"

namespace plvopencv
{
    class PLVOPENCV_EXPORT OpenCVScalar : public plv::Data
    {

    protected:
        CvScalar m_scalar;
        int m_nChannels;

    public:
        OpenCVScalar( CvScalar scalar, int nChannels );
        OpenCVScalar( const OpenCVScalar& other );
        virtual ~OpenCVScalar();

        /**
         * Returns the value of the CvScalar at the given index.
         * Note that a scalar only has 4 elements thus only index
         * values 0, 1, 2 and 3 are accepted.
         */
        double getValueAtIndex(int index) const throw( plv::IllegalArgumentException );

        CvScalar getScalar() const;
        int getNumChannels() const;

        /** @returns a deep copy of this OpenCVScalar.
          */
        OpenCVScalar* deepCopy() const;

    };//class OpenCVScalar

} //namespace plvopencv

#endif // OPENCVSCALAR_H
