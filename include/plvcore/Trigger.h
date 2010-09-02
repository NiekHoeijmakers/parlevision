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

#ifndef TRIGGER_H
#define TRIGGER_H

#include <QVariant>

#include "PipelineProcessor.h"
#include "Pin.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;
    class ActivationTrigger;

    class Trigger : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Trigger")
        Q_CLASSINFO("description", "A processor that passes on a boolean value for every received image. "
                        "The boolean value can be used by other processors that accept it as an activation trigger. "
                        "The trigger is either continuous or one time only.");

        Q_PROPERTY( bool activate READ getActivate WRITE setActivate NOTIFY activateChanged  )
        Q_PROPERTY( bool continuous READ getContinuous WRITE setContinuous NOTIFY continuousChanged  )


    public:
        /** Constructor/Destructor */
        Trigger();
        ~Trigger();

        /** Mandatory methods */
        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        bool getActivate(){ return m_activate; }
        bool getContinuous(){ return m_continuous; }

    signals:
        void activateChanged (bool newValue);
        void continuousChanged (bool newValue);

    public slots:
        void setActivate(bool b) {m_activate = b; emit(activateChanged(b));}
        void setContinuous(bool b) {m_continuous = b; emit(continuousChanged(b));}

    private:
        InputPin<OpenCVImage>* m_inputPin;
        OutputPin<ActivationTrigger>* m_outputPin;

        bool    m_activate;     //Determines if an activation (true) trigger has to be send. Mostly false.
        bool    m_continuous;   //Determines if the activation trigger automatically has to be switched off (false).

    };

}

#endif // TRIGGER_H
