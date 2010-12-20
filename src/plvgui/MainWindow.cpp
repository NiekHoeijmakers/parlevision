/**
  * Copyright (C)2010 by Michel Jansen and Richard Loos
  * All rights reserved.
  *
  * This file is part of the plvgui module of ParleVision.
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

#include "ui_mainwindow.h"
#include "ui_welcome.h"

#include <plvgui/MainWindow.h>
#include <plvgui/LibraryWidget.h>
#include <plvgui/InspectorWidget.h>
#include <plvgui/ViewerWidget.h>
#include <plvgui/DataRenderer.h>
#include <plvgui/RendererFactory.h>
#include <plvgui/PinClickedEvent.h>
#include <plvgui/PinWidget.h>
#include <plvgui/PipelineElementWidget.h>
#include <plvgui/PipelineScene.h>

#include <plvcore/Pipeline.h>
#include <plvcore/PipelineElement.h>
#include <plvcore/Pin.h>
#include <plvcore/PinConnection.h>
#include <plvcore/PipelineLoader.h>

#include <QDebug>
#include <QSettings>
#include <QtGui>

#include <list>

using namespace plvgui;
using namespace plv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_documentChanged(false),
    m_fileName("")
{
    setAttribute(Qt::WA_DeleteOnClose);
    initGUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initGUI()
{
    // Load design from Form mainwindow.ui
    ui->setupUi(this);
    setUnifiedTitleAndToolBarOnMac(true);

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

    this->setWindowTitle("ParleVision - no pipeline");

    ui->view->setAcceptDrops(true);
    ui->view->setEnabled(false);
    ui->view->hide();
    ui->actionSave->setEnabled(false);
    ui->actionSaveAs->setEnabled(false);
    ui->actionStart->setEnabled(false);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);

    QShortcut* shortcut = new QShortcut(QKeySequence(tr("Backspace")),this);
    connect(shortcut, SIGNAL(activated()), ui->actionDelete, SLOT(trigger()));

    QShortcut* closeShortcut = new QShortcut(QKeySequence(tr("Ctrl+W")),this);
    connect(closeShortcut, SIGNAL(activated()), this, SLOT(close()));

    createRecentFileActs();
    // the welcome widget needs the recent file actions
    createWelcomeWidget();
    createLibraryWidget();
    createInspectorWidget();

    // Restore window geometry and state
    loadSettings();

}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    case QEvent::ActivationChange:
        if(this->isActiveWindow() || this->m_libraryWidget->isActiveWindow())
        {
//            qDebug() << this << " activated";
        }
        else
        {
//            qDebug() << this << " went to background";
        }
    default:
        break;
    }
}

bool MainWindow::event(QEvent* event)
{
//    qDebug()<< "MainWindow got event " << event
//            << " ut=" << PinDoubleClickedEvent::user_type()
//            << " ut=" << PinClickedEvent::user_type();

    if(event->type() == PinDoubleClickedEvent::user_type())
    {
        PinDoubleClickedEvent* pce = static_cast<PinDoubleClickedEvent*>(event);
        qDebug() << pce->getSource()->getPin()->getName();
        RefPtr<IOutputPin> pin = ref_ptr_dynamic_cast<IOutputPin>(pce->getSource()->getPin());
        if(pin.isNotNull())
        {
            // only show viewer for output pins
            showViewerForPin(pin);
        }
    }

    return QMainWindow::event(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(this->m_documentChanged)
    {
        offerToSave();
    }

    if( m_pipeline.isNotNull() )
    {
        qDebug() << "Stopping pipeline...";
        m_pipeline->stop();
        m_pipeline->clear();
    }
    QSettings settings;
    qDebug() << "Saving geometry info to " << settings.fileName();
    settings.setValue("MainWindow/geometry", saveGeometry());
    settings.setValue("MainWindow/windowState", saveState());

    QMainWindow::closeEvent(event);
}

void MainWindow::loadSettings()
{
    QSettings settings;
    qDebug() << "Reading settings from " << settings.fileName();
    try
    {
        restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
        restoreState(settings.value("MainWindow/windowState").toByteArray());
    }
    catch(...)
    {
        qWarning() << "Settings corrupt. Clearing.";
        settings.clear();
    }
}

void MainWindow::setCurrentFile(QString fileName)
{
    this->m_fileName = fileName;

    if(fileName.isEmpty())
        return;

    this->updateWindowTitle();

    // Load, update and save the list of recent files
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
    {
        files.removeLast();
    }

    settings.setValue("recentFileList", files);

    // update all windows
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
        {
            mainWin->updateRecentFileActions();
        }
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(files[i]).fileName());
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
    {
        recentFileActs[j]->setVisible(false);
    }

    m_recentFilesSeparator->setVisible(numRecentFiles > 0);
}

void MainWindow::createLibraryWidget()
{
    m_libraryWidget = new LibraryWidget(this);
    this->addDockWidget(Qt::LeftDockWidgetArea, m_libraryWidget);
    m_libraryWidget->toggleViewAction()->setIcon(QIcon(":/icons/library.png"));
    m_libraryWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_L));
    ui->toolBar->addAction(m_libraryWidget->toggleViewAction());
    ui->menuView->addAction(m_libraryWidget->toggleViewAction());
    #ifdef Q_OS_MAC
    // Show LibraryWidget as floating window on Mac OS X
    m_libraryWidget->setFloating(true);
    #endif
}

void MainWindow::createInspectorWidget()
{
    m_inspectorWidget = new InspectorWidget(this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_inspectorWidget);
    m_inspectorWidget->toggleViewAction()->setIcon(QIcon(":/icons/inspector.png"));
    m_inspectorWidget->toggleViewAction()->setShortcut(QKeySequence(Qt::ControlModifier + Qt::Key_I));
    ui->toolBar->addAction(m_inspectorWidget->toggleViewAction());
    ui->menuView->addAction(m_inspectorWidget->toggleViewAction());
    #ifdef Q_OS_MAC
    // Show LibraryWidget as floating window on Mac OS X
    m_inspectorWidget->setFloating(true);
    #endif
}

void MainWindow::createRecentFileActs()
{
    m_recentFilesSeparator = ui->menuFile->addSeparator();

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        ui->menuFile->addAction(recentFileActs[i]);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));
    }

    updateRecentFileActions();
}

void MainWindow::createWelcomeWidget()
{
    Ui::WelcomeWidget* w = new Ui::WelcomeWidget();
    this->welcomeWidget = new QWidget(this);
    w->setupUi(this->welcomeWidget);

    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        QAction* recentFileAct = recentFileActs[i];
        assert(recentFileAct != 0);
        QPushButton* fileLink = new QPushButton(recentFileAct->data().toString(), this->welcomeWidget);
        fileLink->setFlat(true);
        fileLink->setCursor(Qt::PointingHandCursor);
        connect(fileLink, SIGNAL(clicked()), recentFileAct, SIGNAL(triggered()));
        w->recentFilesColumn->addWidget(fileLink);
    }

    w->recentFilesColumn->addStretch();
    w->scrollArea->horizontalScrollBar()->setValue(w->scrollArea->horizontalScrollBar()->maximum());

    connect(w->newButton, SIGNAL(clicked()), ui->actionNew, SLOT(trigger()));
    connect(w->openButton, SIGNAL(clicked()), ui->actionLoad, SLOT(trigger()));

    this->welcomeWidget->hide();

    ui->topContainer->insertWidget(0, this->welcomeWidget);
}

void MainWindow::showWelcomeScreen()
{
    this->welcomeWidget->show();
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}

void MainWindow::setPipeline(plv::Pipeline* pipeline)
{
    //TODO throw exception as well
    assert(this->m_pipeline.isNull());

    this->m_pipeline = pipeline;

    assert (ui->view != 0);
    this->m_scene = new PipelineScene(pipeline, ui->view);

    connect(m_scene, SIGNAL(selectionChanged()),
            this, SLOT(sceneSelectionChanged()));

    this->welcomeWidget->hide();

    ui->view->setScene(m_scene);
    ui->view->setEnabled(true);
    ui->view->show();
    ui->actionSave->setEnabled(true);
    ui->actionSaveAs->setEnabled(true);
    ui->actionStart->setEnabled(true);
    ui->actionPause->setEnabled(false);
    ui->actionStop->setEnabled(false);

    connect(ui->actionStop, SIGNAL(triggered()),
            pipeline, SLOT(stop()));

    connect(ui->actionStart, SIGNAL(triggered()),
            pipeline, SLOT(start()));

    connect(pipeline, SIGNAL(started()),
            this, SLOT(pipelineStarted()));

    connect(pipeline, SIGNAL(stopped()),
            this, SLOT(pipelineStopped()));

    connect(pipeline, SIGNAL(errorOccurred(QString)),
            this, SLOT(criticalError(QString)));

    connect(pipeline, SIGNAL(elementAdded(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(elementChanged(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(elementRemoved(plv::RefPtr<plv::PipelineElement>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(connectionAdded(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(connectionChanged(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(documentChanged()));
    connect(pipeline, SIGNAL(connectionRemoved(plv::RefPtr<plv::PinConnection>)),
            this, SLOT(documentChanged()));

    connect(m_scene, SIGNAL(contentsChanged()),
            this, SLOT(documentChanged()));

    connect( pipeline, SIGNAL(tick()), this, SLOT(tick()) );

    qDebug() << "setting documentChanged to false #1";
    m_documentChanged = false;
}

void MainWindow::pipelineStarted()
{
    ui->actionStart->setDisabled(true);
    ui->actionStop->setEnabled(true);
    ui->statusbar->showMessage("Running");
}

void MainWindow::pipelineStopped()
{
    ui->actionStart->setEnabled(true);
    ui->actionStop->setDisabled(true);
    ui->statusbar->showMessage("Stopped.");
}

void MainWindow::tick()
{
}

void MainWindow::loadFile(QString fileName)
{

    if(this->m_pipeline)
    {
        // already had an open pipeline, open new window
        MainWindow* other = newWindow();
        other->loadFile(fileName);
        return;
    }

    try
    {
        // this window did not yet have a pipeline loaded yet
        RefPtr<Pipeline> pl = PipelineLoader::deserialize(fileName);
//        bool state = pl->init();
        qDebug() << "Loaded pipeline";
//        assert(state);
        this->setCurrentFile(fileName);
        this->setPipeline(pl);
    }
    catch( std::runtime_error& e )
    {
        qDebug() << "Pipeline loading failed with: " << e.what();
        this->handleMessage(QtCriticalMsg, "Failed to load load pipeline from "+fileName+":\n" + QString(e.what()));
        return;
    }
    catch( ... )
    {
        qDebug() << "Caught unknown exception.";
        this->handleMessage(QtFatalMsg, "An unknown error occured while loading " + fileName + ".");
        return;
    }
}

MainWindow* MainWindow::newWindow()
{
    MainWindow *other = new MainWindow();
    other->move(x() + 40, y() + 40);
    other->show();
    return other;
}

void MainWindow::showViewersForElement(plv::RefPtr<plv::PipelineElement> element)
{
    qDebug() << "Adding renderers for " << element->getName();

    const PipelineElement::OutputPinMap& outputPins = element->getOutputPins();
    for( PipelineElement::OutputPinMap::const_iterator itr = outputPins.begin()
        ; itr!=outputPins.end(); ++itr)
    {
        showViewerForPin(itr->second);
    }
}

void MainWindow::showViewerForPin(plv::RefPtr<plv::IOutputPin> pin)
{
    assert(pin.isNotNull());
    qDebug() << "Adding renderer for Pin " << pin->getName();


    // show existing window if exists
    bool existing = false;
    int viewerCount = 0;
    foreach (QWidget *widget, QApplication::allWidgets())
    {
        ViewerWidget* viewer = qobject_cast<ViewerWidget*>(widget);

        if(viewer)
        {
            if (viewer->getPin().getPtr() == pin.getPtr())
            {
                viewer->show();
                existing = true;
            }
            viewerCount++;
        }
    }

    // or new window if not
    if(!existing)
    {
        ViewerWidget* viewer = new ViewerWidget(pin, this);
        viewer->show();
        #ifdef Q_OS_MAC
        // Show as floating window on Mac OS X
        viewer->setFloating(true);

        // move the window so it's aligned with the top right corner
        QPoint cornerPos = this->pos() + QPoint(this->geometry().width(), viewerCount*20);

        bool isAcceptable = QApplication::desktop()->geometry().contains(QRect(cornerPos.x(), cornerPos.y(), 20, 20));
        qDebug() << isAcceptable;

        if(isAcceptable)
        {
            viewer->move(cornerPos);
        }

        #else
        this->addDockWidget(Qt::BottomDockWidgetArea, viewer);
        #endif
    }
}

//void MainWindow::addCamera(plv::OpenCVCamera* camera)
//{
//    // note that signals are not yet ever disconnected.
//    // this will probably change anyway as we want the whole pipeline to stop
//    // and not just the cameras.
//    connect(ui->actionStop, SIGNAL(triggered()),
//            camera, SLOT(release()));
//    connect(this->m_stopAction, SIGNAL(triggered()),
//            camera, SLOT(release()));
//
//    connect(ui->actionStart, SIGNAL(triggered()),
//            camera, SLOT(start()));
//    connect(this->m_startAction, SIGNAL(triggered()),
//            camera, SLOT(start()));
//
//    connect(ui->actionPause, SIGNAL(triggered()),
//            camera, SLOT(pause()));
//    connect(this->m_pauseAction, SIGNAL(triggered()),
//            camera, SLOT(pause()));
//}

void plvgui::MainWindow::on_actionLoad_triggered()
{
    /*
    QString fileName = QFileDialog::getOpenFileName(this,
                            tr("Open Pipeline"),
                            "",
                            tr("ParleVision Pipeline (*.plv *.pipeline)"));
    if(filenName != "")
    {
        qDebug() << "User selected " << fileName;
        loadFile(fileName);
    }
    */

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("ParleVision Pipeline (*.plv *.pipeline)"));

    if (dialog.exec())
    {
        QString fileName = dialog.selectedFiles().first();

        qDebug() << "User selected " << fileName;
        loadFile(fileName);
    }
}

void plvgui::MainWindow::on_actionSave_triggered()
{
    if(this->m_pipeline.isNull())
        return;


    if(this->m_fileName.isEmpty())
    {
        // reroute to Save As to acquire filename
        on_actionSaveAs_triggered();
    }
    else
    {
        save();
    }
}

void plvgui::MainWindow::on_actionSaveAs_triggered()
{
    if(this->m_pipeline.isNull())
        return;

    // get filename
    QString fileName = QFileDialog::getSaveFileName(this,
                            tr("File to save pipeline to"),
                            "",
                            tr("ParleVision Pipeline (*.plv *.pipeline)"));
    if(fileName.isEmpty())
        return;

    qDebug() << "User selected " << fileName;
    m_fileName = fileName;
    save();
    // this updates the window file path as well
    setCurrentFile(fileName);
}

void plvgui::MainWindow::on_actionNew_triggered()
{
    MainWindow* win = this;
    if(this->m_pipeline.isNotNull())
    {
        win = newWindow();
    }

    RefPtr<Pipeline> pipeline = new Pipeline();
    win->setCurrentFile("");
    win->setPipeline(pipeline);
    m_documentChanged = false;
    updateWindowTitle();
}

void MainWindow::documentChanged()
{
    if(m_documentChanged)
        return;

    m_documentChanged = true;
    ui->actionSave->setEnabled(true);
    updateWindowTitle();
}

void plvgui::MainWindow::on_actionDelete_triggered()
{
    if(this->m_scene)
    {
        this->m_scene->deleteSelected();
    }
}

void plvgui::MainWindow::sceneSelectionChanged()
{
    int selectionCount = this->m_scene->selectedItems().size();

    ui->actionDelete->setEnabled(selectionCount > 0);

    if(selectionCount == 0)
    {
        m_inspectorWidget->nothingSelected();
    }
    if(selectionCount > 1)
    {
        m_inspectorWidget->multipleSelected();
    }
    else if(selectionCount == 1)
    {
        // set inspector target
        QGraphicsItem* selectedItem = this->m_scene->selectedItems().first();
        qDebug() << "selected " << selectedItem;
        PipelineElementWidget* pew = dynamic_cast<PipelineElementWidget*>(selectedItem);
        if(pew)
        {
            RefPtr<PipelineElement> element = pew->getElement();
            m_inspectorWidget->setTarget(element);
        }
        else
        {
            m_inspectorWidget->nothingSelected();
        }
    }
}

void MainWindow::save()
{
    if(this->m_pipeline.isNull() || this->m_fileName.isNull())
        return;

    qDebug() << "Saving to " << m_fileName;

    try
    {
        PipelineLoader::serialize( m_fileName, m_pipeline );
        m_documentChanged = false;
        updateWindowTitle();
    }
    catch( std::runtime_error& e )
    {
        qCritical() << "Pipeline saving failed with " << e.what();
        this->handleMessage(QtCriticalMsg, "Could not save pipeline: "
                            + QString(e.what()));
    }

}


void MainWindow::criticalError(QString msg)
{
    handleMessage(QtCriticalMsg, msg);
}

void MainWindow::handleMessage(QtMsgType type, const char* msg)
{
    handleMessage(type, QString(msg));
}

void MainWindow::handleMessage(QtMsgType type, QString msg)
{
    QMessageBox msgBox(QMessageBox::Critical,
                       QString("Error"),
                       msg,
                       QMessageBox::Ok,
                       this);
    msgBox.setWindowModality(Qt::WindowModal);

    QErrorMessage errorDialog(this);
    errorDialog.setWindowModality(Qt::WindowModal);

    switch (type) {
    case QtDebugMsg:
        break;
    case QtWarningMsg:
        errorDialog.showMessage(msg);
        errorDialog.exec();
        break;
    case QtCriticalMsg:
        errorDialog.showMessage("Error: " + QString(msg));
        errorDialog.exec();
        break;
    case QtFatalMsg:
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText("Fatal Error: " + QString(msg)
                                + "\nThe application will now close");
        msgBox.exec();

        this->offerToSave();
        QApplication::exit();
    }
}

void MainWindow::offerToSave()
{
    if(this->m_pipeline.isNull())
        return;

    QMessageBox msgBox(QMessageBox::Question,
                       QString("Save"),
                       tr("Would you like to save before closing?"),
                       QMessageBox::Save | QMessageBox::Discard,
                       this);
    msgBox.setWindowModality(Qt::WindowModal);

    int result = msgBox.exec();

    if(result == QDialog::Accepted)
    {
        on_actionSave_triggered();
    }
}


void MainWindow::updateWindowTitle()
{
    QString title;
    if(this->m_fileName.isEmpty())
    {
        title = "Untitled";
    }
    else
    {
        title = QFileInfo(m_fileName).baseName();
        setWindowFilePath(m_fileName);
    }
    if(m_documentChanged)
    {
        title += "*";
    }
    setWindowTitle(title);
}
