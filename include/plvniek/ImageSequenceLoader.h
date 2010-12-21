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

#ifndef IMAGESEQUENCELOADER_H
#define IMAGESEQUENCELOADER_H

#include <QMutex>
#include <plvcore/PipelineProducer.h>
#include <plvcore/Pin.h>
#include <plvcore/CvMatData.h>
#include <plvcore/Enum.h>

namespace plv
{
    class CvMatDataOutputPin;
}

namespace plvopencv
{
    class ImageSequenceLoader : public plv::PipelineProducer
    {
        Q_OBJECT
        Q_CLASSINFO("author", "Niek Hoeijmakers")
        Q_CLASSINFO("name", "Image Sequence Loader")
        Q_CLASSINFO("description", "A processor that loads a series of images one by one and sends them to the pipe.");

        Q_PROPERTY( bool startLoading READ getStartLoading WRITE setStartLoading NOTIFY startLoadingChanged  )
        Q_PROPERTY( bool reset READ getReset WRITE setReset NOTIFY resetChanged  )
        Q_PROPERTY( QString filename READ getFilename WRITE setFilename NOTIFY filenameChanged )
        Q_PROPERTY( QString directory READ getDirectory WRITE setDirectory NOTIFY directoryChanged )
        Q_PROPERTY( plv::Enum fileFormat READ getFileFormat WRITE setFileFormat NOTIFY fileFormatChanged )
        Q_PROPERTY( int startNr READ getStartNr WRITE setStartNr NOTIFY startNrChanged  )
        Q_PROPERTY( int EndNr READ getEndNr WRITE setEndNr NOTIFY endNrChanged  )

        /** required standard method declaration for plv::PipelineElement */
        PLV_PIPELINE_ELEMENT

    public:
        ImageSequenceLoader();
        virtual ~ImageSequenceLoader();

        /** @returns true if a new frame is available */
        bool isReadyForProcessing() const;


        /** property methods */
        bool getStartLoading();
        void updateStartLoading(bool b){ setStartLoading(b); startLoadingChanged(b); }

        bool getReset();
        void updateReset(bool b){ setReset(b); resetChanged(b); }

        QString getFilename();
        void updateFilename(QString s){ setFilename(s); filenameChanged(s); }

        QString getDirectory();
        void updateDirectory(QString s){ setDirectory(s); directoryChanged(s); }

        plv::Enum getFileFormat() const;
        void updateFileFormat(plv::Enum e){ setFileFormat(e); fileFormatChanged(e); }

        int getStartNr();
        void updateStartNr(int i){ setStartNr(i); startNrChanged(i); }

        int getEndNr();
        void updateEndNr(int i){ setEndNr(i); endNrChanged(i); }

    signals:
        void startLoadingChanged (bool newValue);
        void resetChanged (bool newValue);
        void filenameChanged(QString newValue);
        void directoryChanged(QString newValue);
        void fileFormatChanged(plv::Enum newValue);
        void startNrChanged(int newValue);
        void endNrChanged(int newValue);

    public slots:
        void setStartLoading(bool b);
        void setReset(bool b);
        void setFilename(QString s);
        void setDirectory(QString s);
        void setFileFormat(plv::Enum e);
        void setStartNr(int i);
        void setEndNr(int i);

    protected:
        plv::CvMatData m_loadedImage;
        plv::CvMatDataOutputPin* m_outputPin;

        QMutex m_processMutex;

    private:
        bool        m_startLoading;
        bool        m_reset;
        QString     m_filename;
        QString     m_directory;
        plv::Enum   m_fileFormat;
        int         m_startNr;
        int         m_endNr;

        /* Additional properties */
        QString     m_fileExt;
        int         m_counter;

        /**
          * This method attemps to load a new image with directory and filename. It
          * returns true if it succesfully loaded a new image and false if the load-
          * ed image is NULL.
          */
        bool ObtainImage(QString directory, QString filename);

    };//class ImageSequenceLoader
}//namespace plvopencv


#endif // IMAGESEQUENCELOADER_H
