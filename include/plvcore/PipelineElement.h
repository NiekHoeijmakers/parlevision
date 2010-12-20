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

#ifndef PIPELINEELEMENT_H
#define PIPELINEELEMENT_H

#include <map>
#include <stdexcept>
#include <assert.h>

#include <QString>
#include <QObject>
#include <QMetaType>

#include "plvglobal.h"
#include "RefPtr.h"
#include "PlvExceptions.h"
#include "PipelineElementFactory.h"

/** Utility macro for implemented pure abstract methods in sub classes */
#define PLV_PIPELINE_ELEMENT \
public: \
    virtual void init(); \
    virtual void deinit() throw (); \
    virtual void start(); \
    virtual void stop(); \
    virtual void process();

namespace plv
{
    class IInputPin;
    class IOutputPin;
    class Pipeline;
    class ScheduleInfo;

    class PLVCORE_EXPORT PipelineElement : public QObject, public RefCounted
    {
        Q_OBJECT

    private:
        /** copy constructor and assignment operator are disabled
            since we are a QObject */
        Q_DISABLE_COPY(PipelineElement)

    public:
        /** typedefs to make code more readable */
        typedef std::map< QString, RefPtr< IInputPin > > InputPinMap;
        typedef std::map< QString, RefPtr< IOutputPin > > OutputPinMap;

    protected:
        /** processor id */
        int m_id;

        /** true if this element has been successfuly initialized */
        bool m_initialized;

        /** map which contains the input pins identified and indexed by their name */
        InputPinMap  m_inputPins;

        /** map which contains the output pins identified and indexed by their name */
        OutputPinMap m_outputPins;

        mutable QMutex m_pleMutex;

        /** mutex used for properties. Properties need a recursive mutex
          * sice the emit() they do to update their own value can return the
          * call to the set function resulting in a deadlock if we use a normal
          * mutex */
        mutable QMutex* m_propertyMutex;

        /** if an error has occured, this string holds an error message.
            Errors are also emitted using signal errorOccurred */
        QString m_errorString;

    public:
        friend class Pipeline;
        friend class ScheduleInfo;

        /*************** BEGIN PUBLIC API ******************/
        PipelineElement();
        virtual ~PipelineElement();

        /** Initialise the element so it is ready to receive process() calls.
          * This will only be called once by the pipeline and allows for late
          * initialization. It however can be called again after a call to deinit()
          * Note: when this method throws an exception, deinit() is called right after.
          */
        virtual void init() = 0;

        /** Deinitalizes an element and frees resources. This method is not
          * allowed to throw an exception. It is called on PipelineElement
          * destruction and directly after init() throws an exception.
          */
        virtual void deinit() throw() = 0;

        /** Start() and stop() are called when the pipeline is started and stopped
          * by the user. This is useful for opening required input devices,
          * starting threads etc. A start() call may occur again after every stop().
          */
        virtual void start() = 0;
        virtual void stop() = 0;

        /** @returns true when this PipelineElement is ready for procesing,
          * which is when the process method is allowed to be called by the scheduler.
          * This method is used by the scheduler to schedule processors
          * and necessary to support processors which do not require input to be
          * available on all defined pins and hence makes it relatively easy to support
          * asynchronous events using normal pipeline connections. Also, processors could
          * be implemented as state machines, using pipeline connections as change of
          * state signals. For instance, one could design a processor which does A when
          * the light is on, and B when the light is not on, where the light state is
          * connected by a normal processor connection.
          */
        virtual bool isReadyForProcessing() const = 0;

        /** @returns true when input pins which are required by this processor to
          * be connected are connected. */
        bool requiredPinsConnected() const;

        /** @returns the set of pipeline elements which are connected to
          * this element via the output pins.
          */
        QSet<PipelineElement*> getConnectedElementsToOutputs() const;

        /** @returns the set of pipeline elements which are connected to
          * this element via the input pins.
          */
        QSet<PipelineElement*> getConnectedElementsToInputs() const;

        /** @returns true if input pins which are connected and synchronous
          *  have data available
          */
        bool dataAvailableOnInputPins() const;

        /** This function does the actual work of this PipelineElement and
          * is called by the PipelineScheduler when inputs of this processor
          * are ready i.e. when isReadyForProcessing returns a positive integer.
          */
        virtual void process() = 0;

        /** Get the name that describes this element, in human readable form. This name
            should be defined as a class property in the processors implementation, e.g
            Q_CLASSINFO("name", "Example"). If no name is defined, the unmangled C++
            class name is returned, e.g. plv::ExampleProcessor. */
        QString getName() const;

        /*************** END OF API ******************/

        /** Adds the input pin to this processing element.
          * @throws IllegalArgumentException if an input pin with
          * the same name already exists
          */
        void addInputPin( IInputPin* pin ) throw (IllegalArgumentException);

        /** Adds the output pin to this processing element.
          * @throws IllegalArgumentException if an input pin with
          * the same name already exists
          */
        void addOutputPin( IOutputPin* pin ) throw (IllegalArgumentException);

        /** @returns the input pin with that name, or null if none exists */
        IInputPin* getInputPin( const QString& name ) const;

        /** @returns the ouput pin with that name, or null if none exists */
        IOutputPin* getOutputPin( const QString& name ) const;

        inline void setId( int id ) { assert(m_id == -1); m_id = id; }
        inline int getId() const { return m_id; }

        /** Get a list of properties defined on this element */
        QStringList getConfigurablePropertyNames();

        /** @returns the summed total of all connections in all input pins */
        int inputPinsConnectionCount() const;

        /** @returns the summed total of all connections in all output pins */
        int outputPinsConnectionCount() const;

        /** @returns the summed total of all connections in all input and output pins */
        int pinsConnectionCount() const;

        /** @returns a list of names of input pins added to this PipelineElement */
        QStringList getInputPinNames() const;

        /** returns a copy of the contents of the input pin map
          * This function is thread safe.
          */
        InputPinMap getInputPins() const;

        /** returns a copy of the contents of the output pin map.
          * This function is thread safe.
          */
        OutputPinMap getOutputPins() const;

        /** @returns a list of names of output pins added to this PipelineElement */
        QStringList getOutputPinNames() const;

        /** returns true if there is at least one Pin with a connection */
        bool hasPinConnections() const;

        /** Get a list of all known PipelineElement Type names */
        //static QStringList types();

        /** Register the given type as a PipelineElement Type.
          * The type needs to be known to Qt's MetaType system,
          * so you will likely rarely call this yourself.
          * Use one of the plvRegisterPipelineElement macros instead.
          * @require typeName was not registered to PipelineElement before.
          * @require typeName is a type registered to the Qt MetaType system
          *     e.g. QMetaType::type(typeName) returns a valid ID
          */
        //static int registerType(QString typeName, QString humanName);

        /** Get a human readable name for the given type
          * @require typeName is a registered type
          * @return a human readable name for the type
          */
        //static QString nameForType(QString typeName);

        /** Returns the largest queue size of the connections connected
          * to the input pins. Returns 0 when there are no input pins with
          * a connection.
          */
        int maxInputQueueSize() const;

        QString getClassProperty(const char* name) const;

        /** Overridden from QObject::setProperty()
          * @emits propertyChanged(QString name)
          */
        void setProperty(const char *name, const QVariant &value);

        inline void setProcessingSerial( unsigned int serial ) { m_serial = serial; }
        inline unsigned int getProcessingSerial() const { return m_serial; }

        void error( ErrorType type, QString msg );

    signals:
        void propertyChanged(QString);
        void errorOccured( ErrorType type, QString message );

    protected:
        /** list to keep track of registered types */
        //static QStringList s_types;
        //static std::map<QString,QString> s_names;

        /** serial number of current processing run. */
        unsigned int m_serial;

        // TODO should be called by the scheduler
        virtual void __init() = 0;

        /** check if required pins are connected and if data is available
          * on required pins. Calls isReadyForProcessing function of super
          * class if this is indeed the case.
          * @returns true when all conditions have been met and isReadyForProcessing()
          * of super also returns true.
          */
        virtual bool __isReadyForProcessing() const = 0;

        /**
          * private process function which handles scoping of input and output pins
          * and calls the process() function of the super class.
          */
        virtual void __process() = 0;
    };
}

// FIXME: [DR] can we, instead of using the 2nd name arguments,
// get this information within plvRegisterPipelineElement from Q_CLASSINFO"name"?
// And use actual classname if this Q_CLASSINFO was not set?
template<typename PET>
int plvRegisterPipelineElement()
{
    plv::PipelineElementConstructorHelper<PET>* plec = new plv::PipelineElementConstructorHelper<PET>();
    int id = plv::PipelineElementFactory::registerElement( plec );
    return id;
}

#endif // PIPELINEELEMENT_H
