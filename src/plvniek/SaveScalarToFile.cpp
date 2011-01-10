
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

#include <QDebug>

#include "SaveScalarToFile.h"
#include <opencv/cv.h>

#include <string>
#include <iomanip>
#include <iostream>
#include <fstream>
using std::ofstream;

using namespace plv;
using namespace plvopencv;

ofstream outFile;
long prcsn;

//--- Constructor & Destructor ------------------------------------------------
SaveScalarToFile::SaveScalarToFile():
        m_doSave(false),
        m_filename("dump"),
        m_directory("c:/"),
        m_counter(0)
{
    m_inputPin = createInputPin<cv::Scalar>( "scalar_input", this, IInputPin::CONNECTION_REQUIRED );

    m_fileFormat.add("Comma Separated Values - *.csv");
    m_fileFormat.add("Plain Text - *.txt");
    m_fileFormat.add("Binary Dump - *.dat");

    m_fileExt.clear();
    m_fileExt = ".csv";
    prcsn = outFile.precision();
}

SaveScalarToFile::~SaveScalarToFile(){}

//--- Mandatory functions -----------------------------------------------------
void SaveScalarToFile::init(){}
void SaveScalarToFile::deinit() throw ()
{
    if(!m_data.isEmpty())
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file!"
                    << " Left over data is discarded.";
        }
    }
    m_data.clear();
    m_counter = 0;
}
void SaveScalarToFile::start(){}

void SaveScalarToFile::stop()
{
    if(!m_data.isEmpty())
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file!"
                    << " Left over data is discarded.";
        }
    }
    m_data.clear();
    m_counter = 0;
}

void SaveScalarToFile::process()
{
    cv::Scalar scale = m_inputPin->get();

    if(m_doSave)
    {
        m_data.append(scale);
        ++m_counter;

        if(m_data.size() >= 100)
        {
            if(!dumpData())
            {
                qDebug() << "Something went wrong with dumping the data to the old file!"
                        << " Left over data is discarded.";
                m_counter -= m_data.size();
                m_data.clear();
            }
        }
    }

}

//--- Class funtions ----------------------------------------------------------

bool SaveScalarToFile::getDoSave()
{
    QMutexLocker lock( m_propertyMutex );
    return m_doSave;
}

QString SaveScalarToFile::getFilename()
{
    QMutexLocker lock( m_propertyMutex );
    return m_filename;
}

QString SaveScalarToFile::getDirectory()
{
    QMutexLocker lock( m_propertyMutex );
    return m_directory;
}

plv::Enum SaveScalarToFile::getFileFormat() const
{
    QMutexLocker lock( m_propertyMutex );
    return m_fileFormat;
}

void SaveScalarToFile::setDoSave(bool b)
{
    QMutexLocker lock( m_propertyMutex );
    m_doSave = b;
}

void SaveScalarToFile::setFilename(QString s)
{
    QMutexLocker lock( m_propertyMutex );
    //first check if there is still some data to be saved in the previous file
    if(!m_data.isEmpty())
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file!"
                    << " Left over data is discarded.";
            m_counter -= m_data.size();
            m_data.clear();
        }
    }

    if(s.contains('\\') || s.contains('/'))
    {
        emit(filenameChanged(m_filename));
    }
    else
    {
        m_filename = s;
        m_counter = 0;
    }
}

void SaveScalarToFile::setDirectory(QString s)
{
    QMutexLocker lock( m_propertyMutex );
    //first check if there is still some data to be saved in the previous file
    if(!m_data.isEmpty())
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file! Left over data is discarded.";
            m_counter -= m_data.size();
            m_data.clear();
        }
    }

    m_counter = 0;
    m_directory.clear();
    if(s.contains('\\'))
    {
        m_directory = s.replace('\\','/');
        emit(directoryChanged(m_directory));
    }
    else
    {
        m_directory = s;
    }
}

void SaveScalarToFile::setFileFormat(plv::Enum e)
{
    QMutexLocker lock( m_propertyMutex );
    //first check if there is still some data to be saved in the previous file
    if(!m_data.isEmpty())
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file! Left over data is discarded.";
            m_counter -= m_data.size();
            m_data.clear();
        }
    }

    if(m_fileFormat.getSelectedValue() != e.getSelectedValue()){
        m_fileFormat = e;
        m_counter = 0;
        m_fileExt.clear();
        switch(e.getSelectedIndex())
        {
        case 0: //CSV
            m_fileExt = ".csv";
            break;
        case 1: //Plain text
            m_fileExt = ".txt";
            break;
        default: //Binary dump
            m_fileExt = ".dat";
            break;
        }
    }
}

bool SaveScalarToFile::dumpData()
{
    //build path
    QString l_path = m_directory;
    if(!l_path.endsWith('/'))
        l_path.append('/');
    l_path.append(m_filename);
    l_path.append(m_fileExt);

    //call correct function
    bool success = true;
    switch(m_fileFormat.getSelectedIndex())
    {
    case 0: //CSV
        success = writeCSV(l_path);
        break;
    case 1: //Plain text
        success = writeText(l_path);
        break;
    default: //Binary dump
        success = writeBinary(l_path);
        break;
    }

    //clear all data if a success
    if(success)
    {
        m_data.clear();
    }

    //return the succes
    return success;
}

bool SaveScalarToFile::writeText(QString path)
{
    //open output file
    std::string l_path = path.toStdString();

    outFile.open(l_path.c_str(), std::ios::app);
    if(!outFile)
    {
        qDebug() << "Problems occured during the opening of the file: "
                << path;
        return false;
    }

    outFile << std::fixed << std::setprecision(3);
    for(int i = 0; i < m_data.size(); i++)
    {
        cv::Scalar scale = m_data.at(i);
        outFile << m_counter - m_data.size() + i << " = [ "
                    << scale[0] << " , "
                    << scale[1] << " , "
                    << scale[2] << " , "
                    << scale[3] << " ]\n";

        if(!outFile)
            return false;
    }

    outFile.close();
    return true;
}

bool SaveScalarToFile::writeCSV(QString path)
{
    std::string l_path = path.toStdString();

    outFile.open(l_path.c_str(), std::ios::app);
    if(!outFile)
    {
        qDebug() << "Problems occured during the opening of the file: "
                << path;
        return false;
    }

    outFile << std::fixed << std::setprecision(prcsn);
    for(int i = 0; i < m_data.size(); i++)
    {
        cv::Scalar scale = m_data.at(i);
        outFile << m_counter - m_data.size() + i << ";"
                    << scale[0] << ";"
                    << scale[1] << ";"
                    << scale[2] << ";"
                    << scale[3] << ";\n";

        if(!outFile)
            return false;
    }

    outFile.close();
    return true;
}

bool SaveScalarToFile::writeBinary(QString path)
{
    std::string l_path = path.toStdString();

    outFile.open(l_path.c_str(), std::ios::app | std::ios::binary);
    if(!outFile)
    {
        qDebug() << "Problems occured during the opening of the file: "
                << path;
        return false;
    }

    for(int i = 0; i < m_data.size(); i++)
    {
        cv::Scalar scale = m_data.at(i);
        outFile.write( reinterpret_cast< const char * >(&scale),
                       sizeof(scale));

        if(!outFile)
            return false;
    }

    outFile.close();
    return true;
}
