//
//  NotificationCenter.hpp
//  vipre
//
//  Created by Christian Noon on 11/30/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#ifndef VIPRE_NOTIFICATION_CENTER_HPP
#define VIPRE_NOTIFICATION_CENTER_HPP

#include <iostream>
#include <map>
#include <vector>

#include <boost/any.hpp>
#include <boost/function.hpp>

#include <osg/ref_ptr>
#include <osg/Referenced>

#include <vipre/Export.hpp>
#include <vipre/Log.hpp>
#include <vipre/String.hpp>

namespace vipre {

/**
 * Functor abstract base class.
 */
class AbstractFunctor : public osg::Referenced
{
public:

    /** Pure virtual methods enforced on templated subclasses. */
    virtual void notify(const vipre::String& notificationName) = 0;
    virtual void notify(const boost::any& object, const vipre::String& notificationName) = 0;
    virtual bool isEqual(void* object) = 0;
};


/**
 * The notification functor class stores the observer object along with its function pointer to be
 * executed when a matching notification is posted. The notification functor class does not support
 * notifications with objects.
 */
template <class T>
class NotificationFunctor : public AbstractFunctor
{
public:

    /** Constructor. */
    NotificationFunctor(T* objectPointer, boost::function<void (T*)> functionPointer) :
        _objectPointer(objectPointer),
        _functionPointer(functionPointer)
    {
        ;
    }

    /** Notification callback for the functor with no object. */
    void notify(const vipre::String& notificationName)
    {
        _functionPointer(_objectPointer);
    }

    /** Notification callback for the functor with an object. */
    void notify(const boost::any& /*object*/, const vipre::String& notificationName)
    {
        vipreLogERROR(viprePrefix) << "ERROR: notification callback for \"" << notificationName <<
            "\" was sent notification object but should not have been." << std::endl;
        _functionPointer(_objectPointer);
    }

    /** Returns whether the object pointer is equal. */
    virtual bool isEqual(void* object)
    {
        return object == _objectPointer;
    }

protected:

    /** Instance member variables. */
    T* _objectPointer;
    boost::function<void (T*)> _functionPointer;
};


/**
 * The notification with object functor class stores the observer object along with its function pointer to be
 * executed when a matching notification is posted. This functor allows notifications with objects to be
 * passed through the notification center.
 */
template <class T1, class T2>
class NotificationWithObjectFunctor : public AbstractFunctor
{
public:

    /** Constructor. */
    NotificationWithObjectFunctor(T1* objectPointer, boost::function<void (T1*, T2)> functionPointer) :
        _objectPointer(objectPointer),
        _functionPointer(functionPointer)
    {
        ;
    }

    /** Notification callback for the functor with no object. */
    void notify(const vipre::String& notificationName)
    {
        vipreLogERROR(viprePrefix) << "ERROR: notification callback for \"" << notificationName <<
            "\" takes an object but was not sent one." << std::endl;
    }

    /** Notification callback for the functor with an object. */
    void notify(const boost::any& object, const vipre::String& notificationName)
    {
        try
        {
            const T2& castObject = boost::any_cast<T2>(object);
            _functionPointer(_objectPointer, castObject);
        }
        catch (const boost::bad_any_cast& /*e*/)
        {
            vipreLogERROR(viprePrefix) << "ERROR: notification object for \"" << notificationName <<
                "\" has invalid type for bound callback." << std::endl;
        }
    }

    /** Returns whether the object pointer is equal. */
    virtual bool isEqual(void* object)
    {
        return object == _objectPointer;
    }

protected:

    /** Instance member variables. */
    T1* _objectPointer;
    boost::function<void (T1*, T2)> _functionPointer;
};


/**
 * The notification with pointer functor class stores the observer object along with its function pointer to be
 * executed when a matching notification is posted. This functor allows notifications with pointers to be
 * passed through the notification center.
 */
template <class T1, class T2>
class NotificationWithPointerFunctor : public AbstractFunctor
{
public:

    /** Constructor. */
    NotificationWithPointerFunctor(T1* objectPointer, boost::function<void (T1*, T2*)> functionPointer) :
        _objectPointer(objectPointer),
        _functionPointer(functionPointer)
    {
        ;
    }

    /** Notification callback for the functor with no object. */
    void notify(const vipre::String& notificationName)
    {
        vipreLogERROR(viprePrefix) << "ERROR: notification callback for \"" << notificationName <<
            "\" takes an object but was not sent one." << std::endl;
    }

    /** Notification callback for the functor with a pointer. */
    void notify(const boost::any& object, const vipre::String& notificationName)
    {
        try
        {
            T2* castObject = boost::any_cast<T2*>(object);
            _functionPointer(_objectPointer, castObject);
        }
        catch (const boost::bad_any_cast& /*e*/)
        {
            vipreLogERROR(viprePrefix) << "ERROR: notification object for \"" << notificationName <<
                "\" has invalid type for bound callback." << std::endl;
        }
    }

    /** Returns whether the object pointer is equal. */
    virtual bool isEqual(void* object)
    {
        return object == _objectPointer;
    }

protected:

    /** Instance member variables. */
    T1* _objectPointer;
    boost::function<void (T1*, T2*)> _functionPointer;
};


/**
 * The NotificationCenter is the central messaging system for VIPRE. It allows observation
 * methods to register for notifications when they occur. This uses the observer design pattern
 * to emit messages abstractly.
 */
/**
 * Central messaging system for passing abstract messages with objects through VIPRE.
 *
 * The NotificationCenter is a notification system that allows you to send notifications abstractly
 * when events occur in your application. For example, if you're work on an event system, and your event
 * completes, sometimes it would be nice to notify multiple parts of your application that that event
 * was completed. The vipre::NotificationCenter makes this type of notification very easy to do. In order
 * to create such a notification, follow these steps:
 *
 * 1) Register all objects as observers with the NotificationCenter
 *       - boost::function<void (ObjectType1 *) Event *> eventCompletedCallback(&ObjectType1::eventCompleted);
 *       - vipre::NotificationCenter::instance()->addObserver(this, eventCompletedCallback, "EventCompleted");
 *
 * 2) Make sure to remove the observer from the NotificationCenter in its destructor
 *       - vipre::NotificationCenter::instance()->removeObserver(this);
 *
 * 3) When the event completes, post a notification that the event completed with a matching name
 *       - vipre::NotificationCenter::instance()->postNotificationWithObject("EventCompleted", event);
 *
 * And that's all there is to it! For more information, please see the vipreNotificationCenter example.
 */
class VIPRE_EXPORT NotificationCenter
{
public:

    /** Creates a singleton instance. */
    static NotificationCenter* instance() { static NotificationCenter nc; return &nc; }

    /** Registers the observer's function pointer without an object with the notification name. */
    template<typename T>
    void addObserver(T* observer, boost::function<void (T*)> function, const vipre::String& notificationName)
    {
        NotificationFunctor<T>* functor = new NotificationFunctor<T>(observer, function);
        _observers.insert(std::pair<osg::ref_ptr<AbstractFunctor>, vipre::String>(functor, notificationName));
    }

    /** Registers the observer's function pointer with an object with the notification name. */
    template<typename T1, typename T2>
    void addObserver(T1* observer, boost::function<void (T1*, T2)> function, const vipre::String& notificationName)
    {
        NotificationWithObjectFunctor<T1, T2>* functor = new NotificationWithObjectFunctor<T1, T2>(observer, function);
        _observers.insert(std::pair<osg::ref_ptr<AbstractFunctor>, vipre::String>(functor, notificationName));
    }

    /** Registers the observer's function pointer with a pointer object with the notification name. */
    template<typename T1, typename T2>
    void addObserver(T1* observer, boost::function<void (T1*, T2*)> function, const vipre::String& notificationName)
    {
        NotificationWithPointerFunctor<T1, T2>* functor = new NotificationWithPointerFunctor<T1, T2>(observer, function);
        _observers.insert(std::pair<osg::ref_ptr<AbstractFunctor>, vipre::String>(functor, notificationName));
    }

    /** Calls all observer's function pointers that have registered for the posted notification. */
    void postNotification(const vipre::String& notificationName);

    /**
     * Calls all observer's function pointers that have registered for the posted notification
     * with the given object.
     */
    void postNotificationWithObject(const vipre::String& notificationName, boost::any object);

    /** Removes the observer from the notification center. */
    void removeObserver(void* observer);

protected:

    /** Constructor. */
    NotificationCenter();

    /** Destructor. */
    ~NotificationCenter();

    /** Instance member variables. */
    std::multimap<osg::ref_ptr<AbstractFunctor>, vipre::String> _observers;
};

}   // End of vipre namespace

#endif  // End of VIPRE_NOTIFICATION_CENTER_HPP
