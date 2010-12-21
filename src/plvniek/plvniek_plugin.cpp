#include "plvniek_plugin.h"

#include <QtPlugin>
#include <QtDebug>

#include "Xor.h"
#include "Multiply.h"
#include "Split.h"
#include "Trigger.h"
#include "ImageTypeScaleConversion.h"

#include "SaveImageToFile.h"

#include "ImageLoader.h"
#include "ImageSequenceLoader.h"

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
    //Producers
    plvRegisterPipelineElement<plvopencv::ImageLoader>();
    plvRegisterPipelineElement<plvopencv::ImageSequenceLoader>();
    //Processors
    plvRegisterPipelineElement<plvopencv::Xor>();
    plvRegisterPipelineElement<plvopencv::Multiply>();
    plvRegisterPipelineElement<plvopencv::Split>();
    plvRegisterPipelineElement<plvopencv::Trigger>();
    plvRegisterPipelineElement<plvopencv::ImageTypeScaleConversion>();
    //Consumers
    plvRegisterPipelineElement<plvopencv::SaveImageToFile>();
}

Q_EXPORT_PLUGIN2(plv_niek_plugin, PlvNiekPlugin)
