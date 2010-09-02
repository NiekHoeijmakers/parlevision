#include "ActivationTrigger.h"
#include "RefPtr.h"

#include <opencv/cv.h>
#include <QMutexLocker>

using namespace plv;

ATriggerFactory* ATriggerFactory::m_instance = 0;

ATriggerFactory::ATriggerFactory( int maxObjectPoolSize ) :
    m_maxObjectPoolSize( maxObjectPoolSize ), m_objectPoolSize( 0 )
{
}

ATriggerFactory::~ATriggerFactory()
{
    purgeAll();
}

void ATriggerFactory::purge()
{
    for( std::list<ActivationTrigger*>::iterator itr = m_objectPool.begin();
            itr != m_objectPool.end(); ++itr )
    {
        ActivationTrigger* trig = *itr;
        if( trig->getRefCount() == 1 )
        {
            // this will auto delete the image
            trig->dec();
            m_objectPool.erase( itr );
        }
    }
}

void ATriggerFactory::purgeAll()
{
    for( std::list<ActivationTrigger*>::iterator itr = m_objectPool.begin();
            itr != m_objectPool.end(); ++itr )
    {
        ActivationTrigger* trig = *itr;
        //m_objectPoolSize -= img->size();
        if( trig->getRefCount() > 1 )
        {
            qDebug() << "ATriggerFactory::purgeAll() called when object " <<
                    trig << " has reference count of " << trig->getRefCount();
            qDebug() << "WARNING. Forcing delete.";
            delete trig;
        }
        else
        {
            // this will auto delete the image
            trig->dec();
        }
        m_objectPool.erase( itr );
    }
}

int ATriggerFactory::objectPoolSize()
{
    return m_objectPoolSize;
}

int ATriggerFactory::maxObjectPoolSize()
{
    return m_maxObjectPoolSize;
}

int ATriggerFactory::numObjects()
{
    return m_objectPool.size();
}

int ATriggerFactory::numObjectsInUse()
{
    int count = 0;
    for( std::list<ActivationTrigger*>::iterator itr = m_objectPool.begin();
            itr != m_objectPool.end(); ++itr )
    {
        ActivationTrigger* trig = *itr;
        if( trig->getRefCount() > 1 )
        {
            ++count;
        }
    }
    return count;
}

ActivationTrigger* ATriggerFactory::get( bool value )
{
    QMutexLocker lock( &m_factoryMutex );
    return getOrCreate( value );
}

ActivationTrigger* ATriggerFactory::getOrCreate( bool value )
{
    // this function is private and should only be called when the mutex
    // has already been acquired
    assert( value == true || value == false );

    ActivationTrigger* trigger = 0;
    for( std::list<ActivationTrigger*>::iterator itr = m_objectPool.begin();
            trigger == 0 && itr != m_objectPool.end(); ++itr )
    {
        ActivationTrigger* current = *itr;
        if( current->getRefCount() == 1 )
        {
            trigger = current;

            // reset mutable state so this image can be freely written to
            trigger->makeMutable();
        }
    }

    if( trigger == 0 )
    {
        trigger = new ActivationTrigger( value );

#if ATRIGGER_USE_POOL
        // up the ref count by one
        trigger->inc();
        m_objectPoolSize += trigger->size();
        m_objectPool.push_back( trigger );
#endif
    }

    assert( trigger != 0 );
    assert( trigger->isMutable() );

    return trigger;
}

/*******************************************************************/

ActivationTrigger::ActivationTrigger( bool value ) : Data()
{
    m_value = value;
}

ActivationTrigger::~ActivationTrigger(){}

bool ActivationTrigger::isConsistent() const
{
    return (m_value == true) || (m_value == false);
}

ActivationTrigger* ActivationTrigger::deepCopy() const
{
    ActivationTrigger* trig = ATriggerFactory::instance()->get( m_value );
    assert(trig->getValue() == m_value);
    return trig;
}
