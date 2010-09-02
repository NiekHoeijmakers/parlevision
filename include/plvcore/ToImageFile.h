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

#ifndef TOIMAGEFILE_H
#define TOIMAGEFILE_H

#include <QVariant>

#include "PipelineProcessor.h"
#include "Pin.h"
#include "Types.h"

namespace plv {

    class Pipeline;
    class OpenCVImage;
    class ActivationTrigger;

    class ToImageFile : public PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "ToImageFile")
        Q_CLASSINFO("description", "A processor that takes an input and saves it when"
                        " it is told to by either a propperty or a trigger input.");

        Q_PROPERTY( bool doSave READ getDoSave WRITE setDoSave NOTIFY doSaveChanged  )
        Q_PROPERTY( QString filename READ getFilename WRITE setFilename NOTIFY filenameChanged )
        Q_PROPERTY( QString directory READ getDirectory WRITE setDirectory NOTIFY directoryChanged )
        Q_PROPERTY( plv::Enum fileFormat READ getFileFormat WRITE setFileFormat NOTIFY fileFormatChanged )
        Q_PROPERTY( int suffixNr READ getSuffixNr WRITE setSuffixNr NOTIFY suffixNrChanged  )
        Q_PROPERTY( bool autoIncSuf READ getAutoIncSuf WRITE setAutoIncSuf NOTIFY autoIncSufChanged  )


    public:
        /** Constructor/Destructor */
        ToImageFile();
        ~ToImageFile();

        /** Mandatory methods */
        virtual void init() throw (PipelineException);
        //virtual bool isBootstrapped() const;
        virtual bool isReadyForProcessing() const;
        virtual void process();

        /** propery methods */
        bool getDoSave(){ return m_doSave; }
        QString getFilename() { return m_filename; }
        QString getDirectory() {return m_directory; }
        plv::Enum getFileFormat() const { return m_fileFormat; }
        int getSuffixNr() { return m_suffixNr; }
        bool getAutoIncSuf(){ return m_autoIncSuf; }

    signals:
        void doSaveChanged (bool newValue);
        void filenameChanged(QString newValue);
        void directoryChanged(QString newValue);
        void fileFormatChanged(plv::Enum newValue);
        void suffixNrChanged(int newValue);
        void autoIncSufChanged (bool newValue);

    public slots:
        void setDoSave(bool b) {m_doSave = b; emit(doSaveChanged(b));}
        void setFilename(QString s) {m_filename = s; emit(filenameChanged(s));}
        void setDirectory(QString s);
        void setFileFormat(plv::Enum e);
        void setSuffixNr(int i) {m_suffixNr = i; emit(suffixNrChanged(i));}
        void setAutoIncSuf(bool b) {m_autoIncSuf = b; emit(autoIncSufChanged(b));}

    private:
        InputPin<OpenCVImage>* m_inputImage;
        InputPin<ActivationTrigger>* m_inputTrigger;

        bool        m_doSave;    //Determines if the input has to be saved. Mostly false.
        QString     m_filename;  //The filename to save the image to.
        QString     m_directory; //Directory to store the image.
        plv::Enum   m_fileFormat;//The file extention and thus the manner in which the data is stored.
        int         m_suffixNr;  //The suffix number that is appended at the end of the filename.
        bool        m_autoIncSuf;//Tells whether the suffix number should be increased after each save.

        /* Additional properties */
        QString     m_fileExt;   //The filename extension selected through m_fileFormat.

    };

}


#endif // TOIMAGEFILE_H
