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

#ifndef SAVEIMAGETOFILE_H
#define SAVEIMAGETOFILE_H

#include <QVariant>

#include <plvcore/PipelineProcessor.h>
#include <plvcore/Pin.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataInputPin;
}

namespace plvopencv {

    /**
     * This proccessor allows a single camera image to be saved to the hard-
     * drive. The user is capable of providing the directory where the file has
     * to be saved. The filename can be provided as well but it will always be
     * followed by a suffix number. Multiple of these processors can be used in
     * the pipeline, but make sure that either the directory or filename is
     * different or run the risk of corrupting or overwriting your image files.
     *
     * OpenCV provides 6 different file formats to save the image to:
     *  - BMP
     *  - JPG
     *  - PNG
     *  - PBM
     *  - SR
     *  - TIFF
     * default is set to BMP.
     */
    class SaveImageToFile : public plv::PipelineProcessor
    {
        Q_OBJECT

        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Save image to file")
        Q_CLASSINFO("description", "A processor that takes an input and saves it when"
                        " it is told to by either a propperty or a trigger input.");

        Q_PROPERTY( bool doSave READ getDoSave WRITE setDoSave NOTIFY doSaveChanged  )
        Q_PROPERTY( QString filename READ getFilename WRITE setFilename NOTIFY filenameChanged )
        Q_PROPERTY( QString directory READ getDirectory WRITE setDirectory NOTIFY directoryChanged )
        Q_PROPERTY( plv::Enum fileFormat READ getFileFormat WRITE setFileFormat NOTIFY fileFormatChanged )
        Q_PROPERTY( int suffixNr READ getSuffixNr WRITE setSuffixNr NOTIFY suffixNrChanged  )
        Q_PROPERTY( bool autoIncSuf READ getAutoIncSuf WRITE setAutoIncSuf NOTIFY autoIncSufChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        /** Constructor/Destructor */
        SaveImageToFile();
        virtual ~SaveImageToFile();

        /** property methods */
        bool getDoSave();
        void updateDoSave(bool b){ setDoSave(b); doSaveChanged(b); }

        QString getFilename();
        void updateFilename(QString s){ setFilename(s); filenameChanged(s); }

        QString getDirectory();
        void updateDirectory(QString s){ setDirectory(s); directoryChanged(s); }

        plv::Enum getFileFormat() const;
        void updateFileFormat(plv::Enum e){ setFileFormat(e); fileFormatChanged(e); }

        int getSuffixNr();
        void updateSuffixNr(int i){ setSuffixNr(i); suffixNrChanged(i); }

        bool getAutoIncSuf();
        void updateAutoIncSuf(bool b){ setAutoIncSuf(b); autoIncSufChanged(b); }

    signals:
        void doSaveChanged (bool newValue);
        void filenameChanged(QString newValue);
        void directoryChanged(QString newValue);
        void fileFormatChanged(plv::Enum newValue);
        void suffixNrChanged(int newValue);
        void autoIncSufChanged (bool newValue);

    public slots:
        void setDoSave(bool b);
        void setFilename(QString s);
        void setDirectory(QString s);
        void setFileFormat(plv::Enum e);
        void setSuffixNr(int i);
        void setAutoIncSuf(bool b);

    private:
        plv::CvMatDataInputPin* m_inputImage;
        plv::InputPin<bool>* m_inputTrigger;

        bool        m_doSave;    //Determines if the input has to be saved. Mostly false.
        QString     m_filename;  //The filename to save the image to.
        QString     m_directory; //Directory to store the image.
        plv::Enum   m_fileFormat;//The file extention and thus the manner in which the data is stored.
        int         m_suffixNr;  //The suffix number that is appended at the end of the filename.
        bool        m_autoIncSuf;//Tells whether the suffix number should be increased after each save.

        /* Additional properties */
        QString     m_fileExt;   //The filename extension selected through m_fileFormat.

    };//class SaveImageToFile

}//namespace plvopencv


#endif // SAVEIMAGETOFILE_H
