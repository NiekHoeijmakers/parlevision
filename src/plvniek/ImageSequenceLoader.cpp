
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

#include <QMutexLocker>
#include <QDebug>

#include "ImageSequenceLoader.h"
#include "OpenCVImage.h"

#include <plvcore/Pin.h>

#include <opencv/highgui.h>
#include <opencv/cv.h>
#include <string>

using namespace plv;
using namespace plvopencv;

ImageSequenceLoader::ImageSequenceLoader() :
        m_startLoading(false),
        m_reset(false),
        m_filename("img"),
        m_directory("c:/"),
        m_startNr(1001),
        m_endNr(1002)
{
    m_outputPin = new OutputPin<OpenCVImage>("image_output", this );
    addOutputPin( m_outputPin.getPtr() );

    m_fileFormat.add("Windows Bitmap - *.bmp");
    m_fileFormat.add("JPEG Files - *.jpg");
    m_fileFormat.add("Portable Network Graphics - *.png");
    m_fileFormat.add("Portable Image Format - *.pbm");
    m_fileFormat.add("Sun Rasters - *.sr");
    m_fileFormat.add("TIFF Files - *.tiff");

    m_fileExt.clear();
    m_fileExt = ".bmp";

    m_counter = m_startNr;
}

ImageSequenceLoader::~ImageSequenceLoader(){}

ImageSequenceLoader::ImageSequenceLoader(const ImageSequenceLoader& other) :
        PipelineProducer( other ),
        m_startLoading(other.m_startLoading),
        m_reset(other.m_reset),
        m_filename(other.m_filename),
        m_directory(other.m_directory),
        m_startNr(other.m_startNr),
        m_endNr(other.m_endNr),
        m_counter(other.m_counter)
{
    //create the output pin
    m_outputPin = new OutputPin<OpenCVImage>("image_output", this );
    addOutputPin( m_outputPin.getPtr() );

    m_fileFormat.add("Windows Bitmap - *.bmp");
    m_fileFormat.add("JPEG Files - *.jpg");
    m_fileFormat.add("Portable Network Graphics - *.png");
    m_fileFormat.add("Portable Image Format - *.pbm");
    m_fileFormat.add("Sun Rasters - *.sr");
    m_fileFormat.add("TIFF Files - *.tiff");

    m_fileExt.clear();
    m_fileExt = ".bmp";

    setFileFormat(other.m_fileFormat);
}

/**
 * Deals with the change of the directory property.
 */
void ImageSequenceLoader::setDirectory(QString s){
    //Clear the string before assigning a new value to it.
    m_directory.clear();
    //replace all '\' characters with '/' characters
    m_directory = s.replace('\\','/');

    //qDebug() << "New directory selected:" << m_directory;

    emit( directoryChanged(m_directory));
}

/**
 * Deals with the change of the fileFomat property.
 */
void ImageSequenceLoader::setFileFormat(plv::Enum e){
    m_fileFormat = e;

    //clear the additional properties
    m_fileExt.clear();
    switch(e.getSelectedIndex()){
    case 1: //JPEG Files
        m_fileExt = ".jpg";
        break;
    case 2: //Portable Network Graphics
        m_fileExt = ".png";
        break;
    case 3: //Portable Image Format
        m_fileExt = ".pbm";
        break;
    case 4: //Sun Rasters
        m_fileExt = ".sr";
        break;
    case 5: //TIFF Files
        m_fileExt = ".tiff";
        break;
    default: //Defaulting to Windows bitmap
        m_fileExt = ".bmp";
        break;
    }

    //qDebug() << "File format has changed to: " << m_fileExt;

    emit( fileFormatChanged(m_fileFormat) );
}

void ImageSequenceLoader::setStartNr(int i) {
    m_startNr = i;
    m_counter = i;
    emit(startNrChanged(m_startNr));
}

/** Mandatory methods. Has nothing to do here. Yet? */
void ImageSequenceLoader::init() throw (PipelineException){}
void ImageSequenceLoader::deinit() throw (){}
void ImageSequenceLoader::start() throw (PipelineException){}
void ImageSequenceLoader::stop() throw (PipelineException){}

bool ImageSequenceLoader::isReadyForProcessing() const
{
    return m_startLoading;
}

void ImageSequenceLoader::process()
{
    if(!m_startLoading) return;

    assert(m_outputPin != 0);

    if(m_reset){
        m_counter = m_startNr;
        setReset(false);
    }

    QString l_filename = m_filename + QString::number(m_counter) + m_fileExt;

    if(ObtainImage(m_directory, l_filename)){
        m_outputPin->put( m_loadedImage.getPtr() );
    }

    if(++m_counter >= m_endNr){
        setStartLoading(false);
        m_counter = m_startNr;
    }
}

bool ImageSequenceLoader::ObtainImage(QString directory, QString filename){
    QMutexLocker lock(&m_processMutex);

    //build the path
    QString path = directory;
    if(!path.endsWith('/')) path.append('/');
    path.append(filename);
    std::string c_path = path.toStdString();

    m_loadedImage = 0;

    //load the image
    const IplImage* image = cvLoadImage(c_path.c_str(), CV_LOAD_IMAGE_UNCHANGED );
    if(image != 0){
        m_loadedImage = OpenCVImageFactory::instance()->getFromBuffer( image );

        //check if the newly loaded image is null or not.
        if(!m_loadedImage->isNull()){
            qDebug() << "New image is loaded from path: " << path;
            return true;
        }
    }

    qDebug() << "New image could not be loaded from path: " << path;
    return false;
}
