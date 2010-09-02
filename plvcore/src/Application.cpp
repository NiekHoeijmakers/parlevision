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

#include "Application.h"

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

#include "Types.h"
#include "Plugin.h"
#include "PipelineElement.h"
#include "RefPtr.h"
#include "Pipeline.h"
#include "PipelineLoader.h"

#include "CameraProducer.h"

#include "Add.h"
#include "Sub.h"
#include "Diff.h"
#include "DelayImage.h"
#include "DummyProcessor.h"
#include "EdgeDetectorLaplace.h"
#include "EdgeDetectorCanny.h"
#include "EdgeDetectorSobel.h"
#include "ImageColorConvert.h"
#include "ImageCornerHarris.h"
#include "ImageFlip.h"
#include "ImageSmooth.h"
#include "Snapshot.h"
#include "ToImageFile.h"
#include "Trigger.h"
#include "ViolaJonesFaceDetector.h"

using namespace plv;

Application::Application(QCoreApplication* app)
{
    this->app = app;
    app->setApplicationName("ParleVision");
    app->setOrganizationName("University of Twente");
}

void Application::init()
{
    loadBuiltins();
    loadPlugins();
}

void Application::loadBuiltins()
{
    //FIXME: [DR] can we, instead of using the 2nd name arguments, get this information within plvRegisterPipelineElement from Q_CLASSINFO"name"? And use actual classname if this Q_CLASSINFO was not set?

    // register classes with Qt so they can be used in signals and slots
    qRegisterMetaType< RefPtr<Data> >("RefPtr<Data>");
    qRegisterMetaType< plv::Enum >( "plv::Enum" );

    //producers
    plvRegisterPipelineElement<plv::CameraProducer>("plv::CameraProducer", "Camera");

    //processors
    plvRegisterPipelineElement<plv::Add>("plv::Add", "A add B");
    plvRegisterPipelineElement<plv::Sub>("plv::Sub", "A sub B");
    plvRegisterPipelineElement<plv::Diff>("plv::Diff", "AbsDiff(A, B)");
    plvRegisterPipelineElement<plv::DelayImage>("plv::DelayImage", "Delay");
    plvRegisterPipelineElement<plv::DummyProcessor>("plv::DummyProcessor", "Dummy");
    plvRegisterPipelineElement<plv::EdgeDetectorCanny>("plv::EdgeDetectorCanny", "Edge Canny");
    plvRegisterPipelineElement<plv::EdgeDetectorLaplace>("plv::EdgeDetectorLaplace", "Edge Laplace");
    plvRegisterPipelineElement<plv::EdgeDetectorSobel>("plv::EdgeDetectorSobel", "Edge Sobel");
    plvRegisterPipelineElement<plv::ImageColorConvert>("plv::ImageColorConvert", "Color Conversion");
    plvRegisterPipelineElement<plv::ImageCornerHarris>("plv::ImageCornerHarris", "Harris Corner Detection");
    plvRegisterPipelineElement<plv::ImageFlip>("plv::ImageFlip", "Flip Image");
    plvRegisterPipelineElement<plv::ImageSmooth>("plv::ImageSmooth", "Smooth");
    plvRegisterPipelineElement<plv::Snapshot>("plv::Snapshot", "Snapshot");
    plvRegisterPipelineElement<plv::ToImageFile>("plv::ToImageFile", "ToImageFile");
    plvRegisterPipelineElement<plv::Trigger>("plv::Trigger", "Trigger");
    plvRegisterPipelineElement<plv::ViolaJonesFaceDetector>("plv::ViolaJonesFaceDetector", "Face Detect V.-J.");

    //consumers
}

void Application::loadPlugins()
{
    QDir pluginsDir(app->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS")
    {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        qDebug() << "base dir: " << pluginsDir;
    }
#endif
    bool inPluginsDir = pluginsDir.cd("plugins");
    qDebug() << "looking in " << pluginsDir;
    if(!inPluginsDir)
    {
        qWarning() << "Plugins directory not found. Skipping loading of plugins";
        return;
    }

    foreach (QString fileName, pluginsDir.entryList(QDir::Files))
    {
        qDebug() << "Trying " << fileName;
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        pluginLoader.load();
        QObject *plugin = pluginLoader.instance();
        if (plugin)
        {
            Plugin* thePlugin = qobject_cast<Plugin*>(plugin);
            if (thePlugin)
            {
                thePlugin->onLoad();
            }
            else
            {
                qWarning() << "Plugin library " << fileName
                        << " is not a valid plugin: " << pluginLoader.errorString();
            }
        }
        else
        {
            qWarning() << "Failed to load plugin " << fileName;
            qWarning() << "Reason: " << pluginLoader.errorString();
        }
    }
}
