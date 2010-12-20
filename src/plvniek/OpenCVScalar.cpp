#include "OpenCVScalar.h"
#include <plvcore/RefPtr.h>

#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

OpenCVScalar::OpenCVScalar( CvScalar scalar, int nChannels ) :
        Data(),
        m_scalar(scalar),
        m_nChannels(nChannels)
{}

OpenCVScalar::OpenCVScalar( const OpenCVScalar& other ) :
        Data(),
        m_scalar(other.m_scalar),
        m_nChannels(other.m_nChannels)
{}

OpenCVScalar::~OpenCVScalar(){}

double OpenCVScalar::getValueAtIndex(int index) const throw( plv:: IllegalArgumentException )
{
    if(index < 0 || index > 3)
        throw plv::IllegalArgumentException("Trying to access an index that does not exist and goes out of bounds of the CvScalar.");
    return m_scalar.val[index];
}

CvScalar OpenCVScalar::getScalar() const
{
    return m_scalar;
}

int OpenCVScalar::getNumChannels() const
{
    return m_nChannels;
}

OpenCVScalar* OpenCVScalar::deepCopy() const
{
    OpenCVScalar* scale = new OpenCVScalar( *this );
    assert(scale->getValueAtIndex(0) == m_scalar.val[0]);
    assert(scale->getValueAtIndex(1) == m_scalar.val[1]);
    assert(scale->getValueAtIndex(2) == m_scalar.val[2]);
    assert(scale->getValueAtIndex(3) == m_scalar.val[3]);
    assert(scale->getNumChannels() == m_nChannels);
    return scale;
}
