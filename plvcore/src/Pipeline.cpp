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

#include "Pipeline.h"

#include <QDebug>
#include <QStringBuilder>
#include <list>
#include <QtConcurrentRun>
#include <QTime>

#include "PipelineElement.h"
#include "PipelineProcessor.h"
#include "PipelineProducer.h"
#include "Pin.h"
#include "PinConnection.h"
#include "Scheduler.h"

#include "PlvExceptions.h"

using namespace plv;

Pipeline::Pipeline() :
        m_stopRequested( false ),
        m_running( false ),
        m_scheduler( new Scheduler( this ) )
{
    // for error reporting to GUI
    connect(m_scheduler, SIGNAL(errorOccurred(QString)),
            this, SIGNAL(errorOccurred(QString)));
}

Pipeline::~Pipeline()
{
    assert( !m_running );
    delete m_scheduler;
}

int Pipeline::addElement( PipelineElement* child ) throw (IllegalArgumentException)
{
    if( !canAddElement( child) )
    {
        QString msg = "Tried to add PipelineElement " % child->getName()
                      % " to pipeline with an id which is already in use.";
        throw IllegalArgumentException( msg );
    }

    RefPtr<PipelineElement> element = child;
    int id = element->getId();
    if( id == - 1 )
    {
        id = getNewPipelineElementId();
        element->setId( id );
    }
    element->setPipeline(this);

    try
    {
        element->init();
    }
    catch( PipelineException& e )
    {
        QString msg = element->getName() + ": " + e.what();
        errorOccurred( msg );
    }

    QMutexLocker lock( &m_pipelineMutex );
    m_children.insert( id, element );
    emit( elementAdded(element) );
    return id;
}

bool Pipeline::canAddElement( PipelineElement* child )
{
    QMutexLocker lock( &m_pipelineMutex );

    RefPtr<PipelineElement> element = child;
    int id = element->getId();
    if( id != -1 )
    {
        return( !m_children.contains( id ) );
    }
    return true;
}

int Pipeline::getNewPipelineElementId()
{
    QMutexLocker lock( &m_pipelineMutex );

    int id = 0;

    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    bool done = false;
    while( itr.hasNext() && !done )
    {
        itr.next();
        if( itr.key() > id )
            done = true;
        else
            ++id;
    }
    return id;
}

PipelineElement* Pipeline::getElement( int id )
{
    QMutexLocker lock( &m_pipelineMutex );

    if( m_children.contains( id ) )
    {
        return m_children.value( id );
    }
    return 0;
}

void Pipeline::removeElement( int id )
{
    QMutexLocker lock( &m_pipelineMutex );

    if( m_children.contains( id ) )
    {
        // preserve the element so we can send it over the signal later
        RefPtr<PipelineElement> element = m_children.value( id );
        removeConnectionsForElement( element );
        m_children.remove( id );
        emit( elementRemoved(element) );
    }
}

void Pipeline::removeAllElements()
{
    QMutexLocker lock( &m_pipelineMutex );

    foreach( RefPtr<PipelineElement> element, m_children )
    {
        removeConnectionsForElement( element );
        emit( elementRemoved(element) );
    }
    m_children.clear();
}

void Pipeline::removeConnectionsForElement( PipelineElement* element )
{
    RefPtr<PipelineElement> ple( element );

    // get a copy of the input pin map
    PipelineElement::InputPinMap inputPins = ple->getInputPins();
    for( PipelineElement::InputPinMap::const_iterator itr = inputPins.begin()
        ; itr!=inputPins.end(); ++itr)
    {
        RefPtr<IInputPin> ipp = itr->second;
        if( ipp->isConnected() )
        {
            RefPtr<PinConnection> connection = ipp->getConnection();
            disconnect( connection.getPtr() );
        }
    }

    // get a copy of the output pin map
    PipelineElement::OutputPinMap outputPins = ple->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator outputPinItr = outputPins.begin()
        ; outputPinItr!=outputPins.end(); ++outputPinItr)
    {
        RefPtr<IOutputPin> opp = outputPinItr->second;

        std::list< RefPtr<PinConnection> > connections = opp->getConnections();
        for( std::list< RefPtr<PinConnection> >::const_iterator connItr = connections.begin();
             connItr!= connections.end(); ++connItr )
        {
            RefPtr<PinConnection> connection = *connItr;
            disconnect( connection.getPtr() );
        }
    }
}

const Pipeline::PipelineElementMap& Pipeline::getChildren() const
{
    return m_children;
}

const Pipeline::PipelineConnectionsList& Pipeline::getConnections() const
{
    return m_connections;
}

void Pipeline::connectPins( IOutputPin* outputPin, IInputPin* inputPin)
        throw (IncompatibleTypeException, DuplicateConnectionException)
{
    QMutexLocker lock( &m_pipelineMutex );

    RefPtr<PinConnection> connection = new PinConnection(outputPin, inputPin);
    m_connections.push_back(connection);
    emit( connectionAdded(connection) );
}

void Pipeline::removeAllConnections()
{
    QMutexLocker lock( &m_pipelineMutex );

    assert( !m_running );

    foreach( RefPtr<PinConnection> connection, m_connections )
    {
        connection->disconnect();
        emit( connectionRemoved( connection ) );
    }
    m_connections.clear();
}

void Pipeline::disconnect( PinConnection* connection )
{
    if( connection == 0 )
    {
        qWarning() << "Ignoring disconnect of null connection";
        return;
    }
    RefPtr<PinConnection> con( connection );
    con->disconnect();

    QMutexLocker lock( &m_pipelineMutex );
    for( PipelineConnectionsList::iterator itr = m_connections.begin();
            itr != m_connections.end(); ++itr )
    {
        RefPtr<PinConnection> con2 = *itr;
        if( con.getPtr() == con2.getPtr() )
        {
            m_connections.erase( itr );
            emit( connectionRemoved( con ) );
            break;
        }
    }
}

//bool Pipeline::init()
//{
//    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );

//    QList<QString> errors;
//    if( itr.hasNext() )
//    {
//        itr.next();
//        RefPtr<PipelineElement> element = itr.value();
//        if( !m_initialized.contains( element->getId() ) )
//        {
//            try
//            {
//                element->init();
//                m_initialized.insert(element->getId());
//            }
//            catch( PipelineException& e )
//            {
//                errors.append( element->getName() + ": " + e.what() );
//            }
//        }
//    }

//    if( !errors.empty() )
//    {
//        foreach( const QString& str, errors )
//        {
//            errorOccurred( str );
//        }
//        return false;
//    }
//    return true;
//}

void Pipeline::clear()
{
    QMutexLocker lock( &m_pipelineMutex );

    assert( !m_running );

    // we need to explicitly remove the connections
    // and the children because they hold a ref pointer
    // to Pipeline and will prevent us from deleting ourselves
    removeAllConnections();
    removeAllElements();
}

void Pipeline::start()
{
    m_pipelineMutex.lock();

    bool error = false;

    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    while( itr.hasNext() )
    {
        itr.next();
        RefPtr<PipelineElement> element = itr.value();
        try
        {
            element->start();
        }
        catch( PipelineException& e )
        {
            QString msg = element->getName() % ": " % e.what();
            errorOccurred(msg);
            error = true;
        }
    }

    // TODO make thread safe
    // and ensure that pipeline cannot be started twice
    m_stopRequested = false;

    if( !error )
    {
        // start scheduler
        m_scheduler->start();

        m_pipelineMutex.unlock();

        // calls run method
        QThread::start();
    }
}

void Pipeline::stop()
{
    m_scheduler->stop();

    //TODO more elegant solution here with QTimer
    m_stopRequested = true;
    while( m_running )
    {
        usleep(100);
    }

    // TODO formalize this procedure (s of pipeline) more!
    QMapIterator<int, RefPtr<PipelineElement> > itr( m_children );
    while( itr.hasNext() )
    {
        itr.next();
        itr.value()->stop();
    }

    foreach(RefPtr<PinConnection> conn, m_connections)
    {
        qDebug() << "flushing " << conn;
        conn->flush();
        assert(!conn->hasData());
    }

    emit(stopped());
}

void Pipeline::run()
{
    m_running = true;
    emit(started());

    while( !m_stopRequested )
    {
        if( !m_scheduler->schedule() )
        {
            m_running = false;
            stop();
        }
    }

    m_running = false;
}
