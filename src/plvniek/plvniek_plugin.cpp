#include "plvniek_plugin.h"

#include <QtPlugin>
#include <QtDebug>

#include "Xor.h"
#include "Multiply.h"
#include "Split.h"
#include "Trigger.h"
#include "ImageTypeScaleConversion.h"
#include "MedianSmooth.h"
#include "DistanceTransform.h"

#include "SaveImageToFile.h"
#include "SaveScalarToFile.h"

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
    plvRegisterPipelineElement<plvopencv::MedianSmooth>();
    plvRegisterPipelineElement<plvopencv::DistanceTransform>();
    //Consumers
    plvRegisterPipelineElement<plvopencv::SaveImageToFile>();
    plvRegisterPipelineElement<plvopencv::SaveScalarToFile>();
}

Q_EXPORT_PLUGIN2(plv_niek_plugin, PlvNiekPlugin)
