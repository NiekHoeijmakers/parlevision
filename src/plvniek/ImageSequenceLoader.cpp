
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

#include <opencv/highgui.h>
#include <string>
#include <plvcore/CvMatDataPin.h>

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
    m_outputPin = createCvMatDataOutputPin("image_output", this );

    m_fileFormat.add("Windows Bitmap - *.bmp");
    m_fileFormat.add("JPEG Files - *.jpg");
    m_fileFormat.add("Portable Network Graphics - *.png");
    m_fileFormat.add("Portable Image Format - *.pbm");
    m_fileFormat.add("Sun Rasters - *.sr");
    m_fileFormat.add("TIFF Files - *.tiff");

    m_fileExt.clear();
    m_fileExt = ".bmp";

    m_counter = m_startNr;
    m_loadedImage = CvMatData();
}

ImageSequenceLoader::~ImageSequenceLoader(){}

bool ImageSequenceLoader::getStartLoading()
{
    QMutexLocker lock( m_propertyMutex );
    return m_startLoading;
}
void ImageSequenceLoader::setStartLoading(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_startLoading = b;
}

bool ImageSequenceLoader::getReset()
{
    QMutexLocker lock( m_propertyMutex );
    return m_reset;
}
void ImageSequenceLoader::setReset(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_reset = b;
}

QString ImageSequenceLoader::getFilename()
{
    QMutexLocker lock( m_propertyMutex );
    return m_filename;
}
void ImageSequenceLoader::setFilename(QString s)
{
    QMutexLocker lock( m_propertyMutex );
    m_filename = s;
}

QString ImageSequenceLoader::getDirectory()
{
    QMutexLocker lock( m_propertyMutex );
    return m_directory;
}
/**
 * Deals with the change of the directory property.
 */
void ImageSequenceLoader::setDirectory(QString s){
    QMutexLocker lock( m_propertyMutex );
    //Clear the string before assigning a new value to it.
    m_directory.clear();
    //replace all '\' characters with '/' characters
    m_directory = s.replace('\\','/');

    //qDebug() << "New directory selected:" << m_directory;
}

plv::Enum ImageSequenceLoader::getFileFormat() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_fileFormat;
}
/**
 * Deals with the change of the fileFomat property.
 */
void ImageSequenceLoader::setFileFormat(plv::Enum e)
{
    QMutexLocker lock( m_propertyMutex );

    m_fileFormat = e;

    //clear the additional properties
    m_fileExt.clear();
    switch(m_fileFormat.getSelectedIndex()){
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
}

int ImageSequenceLoader::getStartNr()
{
    QMutexLocker lock( m_propertyMutex );
    return m_startNr;
}
void ImageSequenceLoader::setStartNr(int i)
{
    QMutexLocker lock( m_propertyMutex );
    m_startNr = i;
    m_counter = i;
}

int ImageSequenceLoader::getEndNr()
{
    QMutexLocker lock( m_propertyMutex );
    return m_endNr;
}
void ImageSequenceLoader::setEndNr(int i)
{
    QMutexLocker lock( m_propertyMutex );
    m_endNr = i;
}

/** Mandatory methods. Has nothing to do here. Yet? */
void ImageSequenceLoader::init(){}
void ImageSequenceLoader::deinit() throw (){}
void ImageSequenceLoader::start(){}
void ImageSequenceLoader::stop(){}

bool ImageSequenceLoader::isReadyForProcessing() const
{
    return m_startLoading;
}

void ImageSequenceLoader::process()
{
    if(!m_startLoading) return;

    if(m_reset){
        m_counter = m_startNr;
        updateReset(false);
    }

    QString l_filename = m_filename + QString::number(m_counter) + m_fileExt;

    if(ObtainImage(m_directory, l_filename)){
        m_outputPin->put( m_loadedImage );
    }

    if(++m_counter > m_endNr){
        updateStartLoading(false);
        m_counter = m_startNr;
    }
}

bool ImageSequenceLoader::ObtainImage(QString directory, QString filename){
    QMutexLocker lock(&m_processMutex);

    //build the path
    QString path = directory;
    if(!path.endsWith('/')) path.append('/');
    path.append(filename);
    std::string load = path.toStdString();

    //load the image
    m_loadedImage = CvMatData();
    cv::Mat& image = m_loadedImage;
    image = cv::imread(load, CV_LOAD_IMAGE_UNCHANGED);

    if(m_loadedImage.isValid()){
        //qDebug() << "New image is loaded from path: " << path;
        return true;
    }

    qDebug() << "New image could not be loaded from path: " << path;
    return false;
}
