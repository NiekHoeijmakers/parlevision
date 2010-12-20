#ifndef PLVNIEKPLUGIN_H
#define PLVNIEKPLUGIN_H

#include "plvniek_global.h"
#include <QObject>
#include <plvcore/Plugin.h>


class PLV_NIEK_EXPORT PlvNiekPlugin : public QObject, public plv::Plugin
{
    Q_OBJECT
    Q_INTERFACES(plv::Plugin)

public:
    PlvNiekPlugin();
    virtual ~PlvNiekPlugin();
    void onLoad();
};

#endif // PLVNIEKPLUGIN_H
