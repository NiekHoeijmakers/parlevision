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

#include "Pin.h"
using namespace plv;

IInputPin::~IInputPin()
{
}

/** Connects this pin through the given connection.
  * @require !this->isConnected()
  * @ensure this->isConnected();
  */
void IInputPin::setConnection(PinConnection* connection)
{
    assert(!this->isConnected());
    assert(connection != 0);

    this->m_connection = connection;
}

void IInputPin::removeConnection()
{
    assert( m_connection.isNotNull() );
    m_connection.set( 0 );
}

PinConnection* IInputPin::getConnection() const
{
    assert( m_connection.isNotNull() );
    return m_connection.getPtr();
}

bool IInputPin::isConnected() const
{
    return this->m_connection.isNotNull();
}

/** @returns true if there is a connection and that connection has data
  * available
  */
bool IInputPin::hasData() const
{
    if( m_connection.isNotNull() )
    {
        return this->m_connection->hasData();
    }
    return false;
}

bool IInputPin::fastforward( unsigned int target )
{
    return m_connection->fastforward( target );
}

void IInputPin::flushConnection()
{
    m_connection->flush();
}

unsigned int IInputPin::getNextSerial() const
{
    Data d = m_connection->peek();
    return d.getSerial();
}

void IInputPin::pre()
{
    m_called = false;
}

void IInputPin::post()
{
    // check if mandatory get() method has been called and
    // data has been removed from this pin
    if( this->isConnected() && this->isSynchronous() )
    {
        if( !m_called )
        {
            QString processorName = this->getOwner()->getName();
            QString msg = "Processor " % processorName %
                          " did not call mandatory get() "
                          " on input Pin " %
                          this->getName();
            throw PlvRuntimeException(msg, __FILE__, __LINE__);
        }
    }
}

void IInputPin::getVariant( QVariant& v )
{
    // check if get is not called twice during one process call
    if( m_called )
    {
        QString processorName = this->m_owner->getName();
        QString msg = "Illegal: method get() called twice "
                      "during process() on InputPin. Pin name is \"" %
                      this->m_name %
                      "\" of processor \"" % processorName % "\"";
        throw PlvRuntimeException(msg,__FILE__, __LINE__ );
    }
    m_called = true;
    if( !(this->m_connection.isNotNull() &&
          this->m_connection->hasData() ))
    {

        QString processorName = this->m_owner->getName();
        QString msg = "Illegal: method get() called on InputPin "
                      "which has no data available. Pin name is " %
                      this->m_name %
                      " of processor " % processorName;
        throw PlvRuntimeException(msg, __FILE__, __LINE__);
    }
    Data d = m_connection->get();
    v.setValue(d.getPayload());
}

IOutputPin::~IOutputPin()
{
    removeConnections();
}

/** Adds a connection to the set of connections this pin outputs to
  * @ensure this->isConnected();
  */
void IOutputPin::addConnection( PinConnection* connection )
{
    assert(connection != 0);

    this->m_connections.push_back(connection);
}

/** Removes the connection from this pin.
  * Assumes it has been disconnect()'ed
  */
void IOutputPin::removeConnection( PinConnection* connection )
{
    assert( connection != 0 );

    for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        RefPtr<PinConnection> current = *itr;
        if( current.getPtr() == connection )
        {
            m_connections.erase( itr );
            return;
        }
    }
    // we should never be here
    assert( false );
}

void IOutputPin::removeConnections()
{
    for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        m_connections.erase( itr );
    }
}

std::list< RefPtr<PinConnection > > IOutputPin::getConnections()
{
    // makes a copy
    return m_connections;
}

int IOutputPin::connectionCount() const
{
    return m_connections.size();
}

int IOutputPin::maxDataOnConnection() const
{
    int max = 0;

    for(std::list< RefPtr<PinConnection> >::const_iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        int size = (*itr)->size();
        if( size > max ) max = size;
    }

    return max;
}



void IOutputPin::pre()
{
    m_called = false;
}

void IOutputPin::post()
{
    // publish NULL data to all listeners
    // this is to keep everything synchronized
    if( this->isConnected() )
    {
        if( !m_called )
        {
            Data nullData;

            // publish to all pin connections
            for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
                    itr != m_connections.end(); ++itr)
            {
                PinConnection* connection = (*itr).getPtr();
                connection->put( nullData );
            }
        }
    }
}

void IOutputPin::putVariant( const QVariant& v )
{
    // check if get is not called twice during one process call
    if( m_called )
    {
        QString processorName = this->m_owner->getName();
        QString msg = "Illegal: method put() called twice "
                      "during process() on OutputPin. Pin name is \"" %
                      this->m_name %
                      "\" of processor \"" % processorName % "\"";
        throw PlvRuntimeException( msg,__FILE__, __LINE__ );
    }
    m_called = true;

    // propagate the serial number
    unsigned int serial = m_owner->getProcessingSerial();
    Data data( serial, v );

    // data should never be NULL
    if( serial == 0  )
    {
        QString processorName = this->m_owner->getName();
        QString msg = "Illegal: Error in processor " % processorName
                      %"NULL data published during process() on OutputPin \"" %
                      this->m_name % "\"";
        throw PlvRuntimeException( msg, __FILE__, __LINE__ );
    }

    // publish data to viewers
    emit( newData( v ) );

    // publish to all pin connections
    for(std::list< RefPtr<PinConnection> >::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr)
    {
        PinConnection* connection = (*itr).getPtr();
        connection->put( data );
    }
}

