
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

#include <QDebug>

#include "ScalarToFile.h"
#include "OpenCVScalar.h"

#include <plvcore/Pin.h>
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
ScalarToFile::ScalarToFile():
        m_doSave(false),
        m_filename("dump"),
        m_directory("c:/"),
        m_counter(0),
        m_insert(0)
{
    m_inputPin = createInputPin<OpenCVScalar>( "scalar_input", this, IInputPin::INPUT_REQUIRED );

    m_fileFormat.add("Comma Separated Values - *.csv");
    m_fileFormat.add("Plain Text - *.txt");
    m_fileFormat.add("Binary Dump - *.dat");

    m_fileExt.clear();
    m_fileExt = ".csv";
    prcsn = outFile.precision();
}

ScalarToFile::~ScalarToFile(){}

//--- Mandatory functions -----------------------------------------------------
void ScalarToFile::init() throw (PipelineException){}
void ScalarToFile::deinit() throw ()
{
    if(m_insert > 0)
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file!"
                    << " Left over data is discarded.";
            m_insert = 0;
        }
    }
    m_counter = 0;
}
void ScalarToFile::start() throw (PipelineException){}

void ScalarToFile::stop() throw (PipelineException)
{
    if(m_insert > 0)
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file!"
                    << " Left over data is discarded.";
            m_insert = 0;
        }
    }
    m_counter = 0;
}

void ScalarToFile::process()
{
    assert(m_inputPin != 0);
    RefPtr<OpenCVScalar> scale = m_inputPin->get();

    if(m_doSave)
    {
        m_data[m_insert] = scale->deepCopy();

        m_counter++;
        if(++m_insert >= 100)
        {
            if(!dumpData())
            {
                qDebug() << "Something went wrong with dumping the data to the old file!"
                        << " Left over data is discarded.";
                m_insert = 0;
            }
        }
    }

}

//--- Class funtions ----------------------------------------------------------

void ScalarToFile::setFilename(QString s)
{
    //first check if there is still some data to be saved in the previous file
    if(m_insert > 0)
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file!"
                    << " Left over data is discarded.";
            m_insert = 0;
        }
    }

    m_filename = s;
    m_counter = 0;
    emit(filenameChanged(m_filename));
}

void ScalarToFile::setDirectory(QString s)
{
    //first check if there is still some data to be saved in the previous file
    if(m_insert > 0)
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file! Left over data is discarded.";
            m_insert = 0;
        }
    }

    m_directory.clear();
    m_directory = s.replace('\\','/');
    m_counter = 0;
    emit(directoryChanged(m_directory));
}

void ScalarToFile::setFileFormat(plv::Enum e)
{
    //first check if there is still some data to be saved in the previous file
    if(m_insert > 0)
    {
        if(!dumpData())
        {
            qDebug() << "Something went wrong with dumping the data to the old file! Left over data is discarded.";
            m_insert = 0;
        }
    }
    m_counter = 0;
    m_fileFormat = e;
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

    emit(fileFormatChanged(m_fileFormat));
}

bool ScalarToFile::dumpData()
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

    //reset the index pointer and delete all the Scalar data
    if(success)
    {
        for(int i = 0; i < m_insert; i++)
        {
            OpenCVScalar* sp = m_data[i];
            m_data[i] = NULL;
            delete sp;
        }
        m_insert = 0;
    }

    //return the succes
    return success;
}

bool ScalarToFile::writeText(QString path)
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
    for(int i = 0; i < m_insert; i++)
    {
        OpenCVScalar* sptr = m_data[i];
        outFile << m_counter - m_insert + i << " = [ " << sptr->getValueAtIndex(0) << " , "
                    << sptr->getValueAtIndex(1) << " , "
                    << sptr->getValueAtIndex(2) << " , "
                    << sptr->getValueAtIndex(3) << " ]  channels = "
                    << sptr->getNumChannels() << "\n";

        if(!outFile)
            return false;
    }

    outFile.close();
    return true;
}

bool ScalarToFile::writeCSV(QString path)
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
    for(int i = 0; i < m_insert; i++)
    {
        OpenCVScalar* sptr = m_data[i];
        outFile << m_counter - m_insert + i << ";" << sptr->getValueAtIndex(0) << ";"
                    << sptr->getValueAtIndex(1) << ";"
                    << sptr->getValueAtIndex(2) << ";"
                    << sptr->getValueAtIndex(3) << ";"
                    << sptr->getNumChannels() << ";\n";

        if(!outFile)
            return false;
    }

    outFile.close();
    return true;
}

bool ScalarToFile::writeBinary(QString path)
{
    std::string l_path = path.toStdString();

    outFile.open(l_path.c_str(), std::ios::app |std::ios::binary);
    if(!outFile)
    {
        qDebug() << "Problems occured during the opening of the file: "
                << path;
        return false;
    }

    for(int i = 0; i < m_insert; i++)
    {
        OpenCVScalar* sptr = m_data[i];
        outFile.write( reinterpret_cast< const char * >(sptr), sizeof(*sptr));

        if(!outFile)
            return false;
    }

    outFile.close();
    return true;
}
