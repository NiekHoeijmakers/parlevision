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

#include <QDebug>

#include "SaveImageToFile.h"

#include <plvcore/CvMatDataPin.h>
#include <plvcore/CvMatData.h>
#include <opencv/highgui.h>
#include <string>


using namespace plv;
using namespace plvopencv;

/**
 * Constructor.
 */
SaveImageToFile::SaveImageToFile() :
        m_doSave(false),
        m_filename("img"),
        m_directory("c:/"),
        m_suffixNr(1001),
        m_autoIncSuf(true)
{
    m_inputImage = createCvMatDataInputPin("image_input", this, IInputPin::CONNECTION_REQUIRED );
    m_inputImage->addAllChannels();
    m_inputImage->addAllDepths();
    //m_inputTrigger = createInputPin<Trigger>("trigger_input", this, IInputPin::INPUT_OPTIONAL );

    m_fileFormat.add("Windows Bitmap - *.bmp");
    m_fileFormat.add("JPEG Files - *.jpg");
    m_fileFormat.add("Portable Network Graphics - *.png");
    m_fileFormat.add("Portable Image Format - *.pbm");
    m_fileFormat.add("Sun Rasters - *.sr");
    m_fileFormat.add("TIFF Files - *.tiff");

    m_fileExt.clear();
    m_fileExt = ".bmp";

}

/**
 * Destructor.
 */
SaveImageToFile::~SaveImageToFile(){}


bool SaveImageToFile::getDoSave()
{
    QMutexLocker lock( m_propertyMutex );
    return m_doSave;
}
void SaveImageToFile::setDoSave(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_doSave = b;
}

QString SaveImageToFile::getFilename()
{
    QMutexLocker lock( m_propertyMutex );
    return m_filename;
}
void SaveImageToFile::setFilename(QString s)
{
    QMutexLocker lock( m_propertyMutex );
    m_filename = s;
}

QString SaveImageToFile::getDirectory()
{
    QMutexLocker lock( m_propertyMutex );
    return m_directory;
}
/**
 * Deals with the change of the directory property.
 */
void SaveImageToFile::setDirectory(QString s){
    QMutexLocker lock( m_propertyMutex );
    //Clear the string before assigning a new value to it.
    m_directory.clear();
    //replace all '\' characters with '/' characters
    m_directory = s.replace('\\','/');
    //qDebug() << "New directory selected:" << m_directory;
}

plv::Enum SaveImageToFile::getFileFormat() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_fileFormat;
}
/**
 * Deals with the change of the fileFomat property.
 */
void SaveImageToFile::setFileFormat(plv::Enum e){
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

int SaveImageToFile::getSuffixNr()
{
    QMutexLocker lock( m_propertyMutex );
    return m_suffixNr;
}
void SaveImageToFile::setSuffixNr(int i)
{
    QMutexLocker lock( m_propertyMutex );
    m_suffixNr = i;
}

bool SaveImageToFile::getAutoIncSuf()
{
    QMutexLocker lock( m_propertyMutex );
    return m_autoIncSuf;
}
void SaveImageToFile::setAutoIncSuf(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_autoIncSuf = b;
}

/** Mandatory methods. Has nothing to do here. Yet? */
void SaveImageToFile::init(){}
void SaveImageToFile::deinit() throw(){}
void SaveImageToFile::start() {}
void SaveImageToFile::stop(){}


/**
 * The method in steps:
 *  - Check if input is initialized and retrieve the image data.
 *  - Check if the trigger input is active
 *     # Set doSave corresponding to the value of the trigger
 *  - Check if the image has to be saved.
 *     # Build the save path.
 *     # Save image to the correct file format
 *     # Check if the suffixNr needs to be increased and increase it.
 */
void SaveImageToFile::process()
{
    plv::CvMatData img = m_inputImage->get();

    //If the trigger has been connected check if the trigger sent an activation.
    /*if(m_inputTrigger->isConnected()){
        RefPtr<Trigger> trig = m_inputTrigger->get();
        if(trig->getValue())
            setDoSave(true);
    }*/

    if(m_doSave)
    {
        QString fn = m_directory;
        //Check if the string ends with a '/' character
        if(!fn.endsWith('/'))
            fn.append('/');
        fn.append(m_filename);
        fn.append(QString::number(m_suffixNr));
        fn.append(m_fileExt);
        const std::string save = fn.toStdString();

        //open image for reading
        const cv::Mat& saveImg = img;

        //qDebug() << "Writing to" << fn;

        if(!cv::imwrite(save, saveImg)){
            qDebug() << "Something went wrong with writing to a file: " << fn;
        }


        if(m_autoIncSuf)
            updateSuffixNr(m_suffixNr+1);

        updateDoSave(false);
    }
}
