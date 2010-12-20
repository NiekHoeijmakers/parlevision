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

#ifndef TRIGGER_H
#define TRIGGER_H

#include <opencv/cv.h>

#include <plvcore/Types.h>
#include <plvcore/PlvExceptions.h>

#include "plvopencv_global.h"

namespace plvopencv
{
    class PLVOPENCV_EXPORT Trigger : public plv::Data
    {

    protected:
        bool m_value;

    public:
        Trigger( bool value );
        Trigger( const Trigger& other );
        virtual ~Trigger();

        inline bool getValue() const { return m_value; }

        /** @returns a deep copy of this Trigger.
          */
        Trigger* deepCopy() const;

    };//class Trigger

} //namespace plvopencv

#endif // TRIGGER_H
