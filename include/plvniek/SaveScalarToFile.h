/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvniek module of ParleVision.
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
#ifndef SAVESCALARTOFILE_H
#define SAVESCALARTOFILE_H

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>
#include <plvcore/Enum.h>
#include <opencv/cv.h>

Q_DECLARE_METATYPE( cv::Scalar );

namespace plvopencv
{
    /**
      * Take a bitwise ScalarToFile of two images .
      */
    class SaveScalarToFile : public plv::PipelineProcessor
    {
        Q_OBJECT
        Q_DISABLE_COPY( SaveScalarToFile )
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
        SaveScalarToFile();
        virtual ~SaveScalarToFile();

        /** property methods */
        bool getDoSave();
        QString getFilename();
        QString getDirectory();
        plv::Enum getFileFormat() const;

    signals:
        void doSaveChanged (bool newValue);
        void filenameChanged(QString newValue);
        void directoryChanged(QString newValue);
        void fileFormatChanged(plv::Enum newValue);

    public slots: void setDoSave(bool b);

        void setFilename(QString s);
        void setDirectory(QString s);
        void setFileFormat(plv::Enum e);

    private:
        plv::InputPin<cv::Scalar>* m_inputPin;
        //Moet toch nog iets verzinnen op het bewaren van channel info.
        //Uit Scalar zelf kan je niet zien of de waarde van alle elementen
        //ook op iets nuttigs is gebasseerd. Is de waarde > 0.0 dan is het
        //nuttig. Maar is het == 0.0 dan weet je het niet zeker, omdat 0.0
        //de initializatie waarde van een cv::Scalar is. Maar ook de som
        //van alle elementen in een kanaal als het kanaal gebruikt wordt.

        bool        m_doSave;
        QString     m_filename;
        QString     m_directory;
        plv::Enum   m_fileFormat;

        /* Additional properties */
        QString             m_fileExt;
        QVector<cv::Scalar> m_data;
        int         m_counter;
        //int         m_insert;
        //OpenCVScalar* m_data[100];

        bool dumpData();
        bool writeText(QString path);
        bool writeCSV(QString path);
        bool writeBinary(QString path);

    };//class SaveScalarToFile
}//namespace plvopencv


#endif // SAVESCALARTOFILE_H
