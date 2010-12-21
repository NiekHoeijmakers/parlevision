#include "plvniek_plugin.h"

#include <QtPlugin>
#include <QtDebug>

#include "Xor.h"
#include "Multiply.h"
#include "Split.h"

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
    plvRegisterPipelineElement<plvopencv::Multiply>();
    plvRegisterPipelineElement<plvopencv::Split>();
}

Q_EXPORT_PLUGIN2(plv_niek_plugin, PlvNiekPlugin)
