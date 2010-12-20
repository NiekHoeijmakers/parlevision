#include "plvniek_plugin.h"

#include <QtPlugin>
#include <QtDebug>

#include "Xor.h"

using namespace plv;

PlvNiekPlugin::PlvNiekPlugin()
{
}

PlvNiekPlugin::~PlvNiekPlugin()
{
}

void PlvNiekPlugin::onLoad()
{
    qDebug() << "PlvNiekPlugin onLoad";
    plvRegisterPipelineElement<plvopencv::Xor>();
}

Q_EXPORT_PLUGIN2(plv_niek_plugin, PlvNiekPlugin)
