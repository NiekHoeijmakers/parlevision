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

#ifndef TYPES_H
#define TYPES_H

#include <QMap>
#include <QStringList>
#include <QMetaType>

#include "RefPtr.h"
#include "assert.h"

namespace plv 
{
    /** Base class for data resources.
      *
      */
    class Data : public RefCounted
    {
    protected:
        /** mutable flag used for determining if data resource can be
          * written to
          */
        bool m_mutable;

    public:
        Data() : m_mutable( true ) {}

        /** Copy constructor needs to be implemented by super classes
          * to allow the copying of a data resources when the Pin
          * connection type is set to copy which can be faster with
          * simple types.
          */
        Data( const Data& other );

        /** makes this data unit mutable again. Internal framework method.
          * Should normally not be called by client code
          */
        inline void makeMutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = true;
        }

        /** makes this data element immutable. This is called by the framework when this element
          * is read only shared in multiple threads
          */
        inline void makeImmutable()
        {
            QMutexLocker( &this->m_refMutex );
            m_mutable = false;
        }

        inline bool isMutable() const
        {
            QMutexLocker( &this->m_refMutex );
            return m_mutable;
        }

    protected:
        /** protected destructor, data resources are not allowed to be
          * deleted by individual processor since they can be in use
          * by other processors in other threads. Deletion in one processor
          * could cause a crash. Reference counting is used to let data
          * resources delete themselves.
          */
        virtual ~Data() {}
    };

    /** private class used to store enum information in Enum class */
    class EnumPair
    {
    public:
        EnumPair( const QString& s="", int v=-1 ) :
                m_name(s), m_value(v) {}

        inline QString name() const { return m_name; }
        inline int value() const { return m_value; }

    protected:
        QString m_name;
        int m_value;
    };

    /** Class for configurable enum properties with introspection support. Useful in GUI code */
    class Enum
    {
    public:
        Enum( int selected = 0 );

        ~Enum();

        Enum( const Enum& other );

        void setSelectedIndex( int i );

        void setSelected( const QString& selected );

        void add( const QString& str );

        void add( const QString& str, int value );

        int getSelectedIndex() const;

        int getSelectedValue() const;

        QString getItemName( int i ) const;

        QString getSelectedItemName() const;

        int getItemValue( int i ) const;

        QStringList getItemNames() const;

        QString toString() const;

    protected:
        int m_selectedIndex;
        QMap<int, EnumPair> m_items;
    };
}
/** Declare as Qt Metatype so we can pass RefPtr<Data> along with signals and slots */
Q_DECLARE_METATYPE( plv::RefPtr<plv::Data> )
Q_DECLARE_METATYPE( plv::Enum )

#endif // TYPES_H
