#include "Trigger.h"
#include <plvcore/RefPtr.h>

#include <opencv/cv.h>

using namespace plv;
using namespace plvopencv;

Trigger::Trigger( bool value ) : Data()
{
    m_value = value;
}

Trigger::~Trigger(){}

Trigger* Trigger::deepCopy() const
{
    Trigger* trig = new Trigger( m_value );
    assert(trig->getValue() == m_value);
    return trig;
}
