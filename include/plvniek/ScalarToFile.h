/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
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
#ifndef SCALARTOFILE_H
#define SCALARTOFILE_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>

namespace plvopencv
{
    class OpenCVScalar;

    /**
      * Take a bitwise ScalarToFile of two images .
      */
    class ScalarToFile : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Save Scalar To File")
        Q_CLASSINFO("description", "A processor that saves the data of an incomming scalar to a file. It does not overwrite a file, but instead appends to the file."
                    " Supported file types are: Plain text, CSV, Binary Dump");

        Q_PROPERTY( bool doSave READ getDoSave WRITE setDoSave NOTIFY doSaveChanged  )
        Q_PROPERTY( QString filename READ getFilename WRITE setFilename NOTIFY filenameChanged )
        Q_PROPERTY( QString directory READ getDirectory WRITE setDirectory NOTIFY directoryChanged )
        Q_PROPERTY( plv::Enum fileFormat READ getFileFormat WRITE setFileFormat NOTIFY fileFormatChanged )


        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ScalarToFile();
        virtual ~ScalarToFile();

        /** property methods */
        bool getDoSave(){ return m_doSave; }
        QString getFilename() { return m_filename; }
        QString getDirectory() {return m_directory; }
        plv::Enum getFileFormat() const { return m_fileFormat; }

    signals:
        void doSaveChanged (bool newValue);
        void filenameChanged(QString newValue);
        void directoryChanged(QString newValue);
        void fileFormatChanged(plv::Enum newValue);

    public slots:
        void setDoSave(bool b) {m_doSave = b; emit(doSaveChanged(m_doSave));}
        void setFilename(QString s);
        void setDirectory(QString s);
        void setFileFormat(plv::Enum e);

    private:
        plv::InputPin<OpenCVScalar>* m_inputPin;

        bool        m_doSave;
        QString     m_filename;
        QString     m_directory;
        plv::Enum   m_fileFormat;

        /* Additional properties */
        QString     m_fileExt;
        int         m_counter;
        int         m_insert;
        OpenCVScalar* m_data[100];

        bool dumpData();
        bool writeText(QString path);
        bool writeCSV(QString path);
        bool writeBinary(QString path);

    };//class ScalarToFile
}//namespace plvopencv


#endif // SCALARTOFILE_H
