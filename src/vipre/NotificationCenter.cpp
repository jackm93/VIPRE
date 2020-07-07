//
//  NotificationCenter.cpp
//  vipre
//
//  Created by Christian Noon on 11/30/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <vipre/NotificationCenter.hpp>

using namespace vipre;

NotificationCenter::NotificationCenter()
{
    ;
}

NotificationCenter::~NotificationCenter()
{
    if (!_observers.empty())
    {
        vipreLogERROR(viprePrefix) << "ERROR: vipre::NotificationCenter has " << _observers.size() << " left!" << std::endl;
    }
}

void NotificationCenter::postNotification(const vipre::String& notificationName)
{
    std::multimap<osg::ref_ptr<AbstractFunctor>, vipre::String>::iterator iter = _observers.begin();
    while (iter != _observers.end())
    {
        if (iter->second == notificationName)
        {
            vipreLogDEBUG(viprePrefix) << "Passing: " << notificationName << "through the NotificationCenter" << std::endl;
            iter->first->notify(notificationName);
        }

        ++iter;
    }
}

void NotificationCenter::postNotificationWithObject(const vipre::String& notificationName, boost::any object)
{
    std::multimap<osg::ref_ptr<AbstractFunctor>, vipre::String>::iterator iter = _observers.begin();
    while (iter != _observers.end())
    {
        if (iter->second == notificationName)
        {
            vipreLogDEBUG(viprePrefix) << "Passing: " << notificationName << "through the NotificationCenter" << std::endl;
            iter->first->notify(object, notificationName);
        }

        ++iter;
    }
}

void NotificationCenter::removeObserver(void* observer)
{
    // First collect all the observers to remove
    std::vector<osg::ref_ptr<AbstractFunctor> > matches;
    std::multimap<osg::ref_ptr<AbstractFunctor>, vipre::String>::iterator iter = _observers.begin();
    while (iter != _observers.end())
    {
        if (iter->first->isEqual(observer))
        {
            matches.push_back(iter->first);
        }

        ++iter;
    }

    // Now remove them
    for (unsigned int i = 0; i < matches.size(); ++i)
    {
        _observers.erase(_observers.find(matches.at(i)));
    }
}
