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

#include "ToImageFile.h"
#include "Pin.h"
#include "OpenCVImage.h"
#include "ActivationTrigger.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>


using namespace plv;

#define INPUT_PIN_NAME "image_input"
#define INPUT_PIN_NAME2 "trigger_input"

/**
 * Constructor.
 */
ToImageFile::ToImageFile() :
        m_doSave(false),
        m_filename("img"),
        m_directory("c:/"),
        m_suffixNr(1001),
        m_autoIncSuf(true)
{
    m_inputImage = createInputPin<OpenCVImage>(INPUT_PIN_NAME, this, IInputPin::REQUIRED );
    m_inputTrigger = createInputPin<ActivationTrigger>(INPUT_PIN_NAME2, this, IInputPin::REQUIRED );

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
ToImageFile::~ToImageFile(){}

/**
 * Deals with the change of the directory property.
 */
void ToImageFile::setDirectory(QString s){
    //Clear the string before assigning a new value to it.
    m_directory.clear();
    //replace all '\' characters with '/' characters
    m_directory = s.replace('\\','/');


    qDebug() << "New directory selected:" << m_directory;

    emit( directoryChanged(m_directory));
}

/**
 * Deals with the change of the fileFomat property.
 */
void ToImageFile::setFileFormat(plv::Enum e){
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

    qDebug() << "File format has changed to: " << m_fileExt;

    emit( fileFormatChanged(m_fileFormat) );
}

/** Mandatory methods */
void ToImageFile::init() throw (PipelineException)
{
    /* Has nothing to do here. Yet? */
}

//virtual bool isBootstrapped() const;
bool ToImageFile::isReadyForProcessing() const
{
    if(m_inputTrigger->isConnected())
        return m_inputImage->getConnection()->hasData()
                && m_inputTrigger->getConnection()->hasData();
    return m_inputImage->getConnection()->hasData();
    /* TODO add a check for a trigger input as well. */
}

/**
 * The method in steps:
 *  - Check if input is initialized and retrieve the image data.
 *  - Check if the image has to be saved.
 *     # Save image to the correct file format
 *     # Check if the suffixNr needs to be increased and increase it.
 */
void ToImageFile::process()
{
    assert(m_inputImage != 0);
    RefPtr<OpenCVImage> img = m_inputImage ->get();

    //If the trigger has been connected check if the trigger sent an activation.
    if(m_inputTrigger->isConnected()){
        RefPtr<ActivationTrigger> trig = m_inputTrigger->get();
        if(trig->getValue())
            setDoSave(true);
    }

    if(m_doSave)
    {
        QString fn = m_directory;
        //Check if the string ends with a '/' character
        if(!fn.endsWith('/'))
            fn.append('/');
        fn.append(m_filename);
        fn.append(QString::number(m_suffixNr));
        fn.append(m_fileExt);
        std::string save = fn.toStdString();

        const IplImage* saveImg = img->getImage();

        qDebug() << "Writing to" << fn;

        if(!cvSaveImage(save.c_str(), saveImg)){
            qDebug() << "something went wrong with writing to a file: ";
        }


        if(m_autoIncSuf)
            setSuffixNr(m_suffixNr+1);

        setDoSave(false);
    }
}
