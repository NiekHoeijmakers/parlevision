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

#ifndef ACTIVATIONTRIGGER_H
#define ACTIVATIONTRIGGER_H

#include <list>
#include <QMutex>
//#include <QReadWriteLock>
#include <opencv/cv.h>

#include "Types.h"
#include "PlvExceptions.h"

#define ATRIGGER_MAX_OBJECT_POOL_SIZE 10000
#define ATRIGGER_USE_POOL 0

namespace plv
{
    class ActivationTrigger;

    class ATriggerFactory
    {
    public:

        /** Will return an ActivationTrigger with the appropriate value. Uses object
          * pooling to recycle triggers which are no longer in use by the pipeline.
          * @returns an ActivationTrigger with appropriate value          */
        ActivationTrigger* get( bool value );

        /** Purges all objects which are not in used from the object pool */
        void purge();

        /** Purges all objects, even those in use (refcount > 1). Dangerous! */
        void purgeAll();

        /** @returns the number of object in the object pool */
        int numObjects();

        /** @returns the number of objects in the object pool which are in use */
        int numObjectsInUse();

        /** @returns the size of the object pool in bytes */
        int objectPoolSize();

        /** @returns the maximum size of the object pool in bytes */
        int maxObjectPoolSize();

        /** Sets the maximum size of the object pool in bytes.
          */
        void setObjectPoolSize( int bytes );

        inline static ATriggerFactory* instance()
        {
            if( m_instance == 0 )
                m_instance = new ATriggerFactory();
            return m_instance;
        }

    private:
        ATriggerFactory( int maxObjectPoolSize = ATRIGGER_MAX_OBJECT_POOL_SIZE );
        ~ATriggerFactory();

        ActivationTrigger* getOrCreate( bool value );

    private:
        static ATriggerFactory* m_instance;  /** singleton class instance */
        std::list<ActivationTrigger*> m_objectPool;
        int m_maxObjectPoolSize;    /** max object pool size in bytes */
        int m_objectPoolSize;       /** object pool size in bytes */
        QMutex m_factoryMutex;
    }; //class ATriggerFactory

/*******************************************************************/

    class ActivationTrigger : public Data
    {
        friend class ATriggerFactory;
        //friend class OpenCVImageWriter;

    protected:
        ActivationTrigger( bool value );
        ActivationTrigger( const ActivationTrigger& other );
        ~ActivationTrigger();

        bool m_value;
        //mutable QMutex m_imgLock;

    public:

        inline bool getValue() const { return m_value; }
        bool isConsistent() const;

        /** @returns a deep copy of this ActivationTrigger.
          */
        ActivationTrigger* deepCopy() const;

    };//class ActivationTrigger

} //namespace plv

#endif // ACTIVATIONTRIGGER_H
