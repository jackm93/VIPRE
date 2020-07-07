//
//  main.cpp
//  vipreNotificationCenter
//
//  Created by Christian Noon on 11/30/11.
//  Copyright (c) 2011 Christian Noon. All rights reserved.
//

#include <boost/function.hpp>

#include <osg/ArgumentParser>
#include <osg/Notify>
#include <osgDB/FileNameUtils>

#include <vipre/Log.hpp>
#include <vipre/NotificationCenter.hpp>

#define examplePrefix   "[vipreNotificationCenter] "

using namespace vipre;

class Renderer
{
public:

    Renderer(const vipre::String& name) :
        _name(name),
        _renderPasses(2)
    {
        // Create all the function pointers for notification callbacks
        boost::function<void (Renderer*)> redrawCallback(&Renderer::requestRedraw);
        boost::function<void (Renderer*, unsigned int)> updateCallback(&Renderer::updateNumRenderPasses);
        boost::function<void (Renderer*, const char*)> changeNameCallback(&Renderer::changeName);
        boost::function<void (Renderer*, vipre::String)> changeNameWithStringCallback(&Renderer::changeNameWithString);

        // Register all the callbacks with the NotificationCenter
        NotificationCenter::instance()->addObserver(this, redrawCallback, "RequestRedraw");
        NotificationCenter::instance()->addObserver(this, updateCallback, "UpdateNumRenderPasses");
        NotificationCenter::instance()->addObserver(this, changeNameCallback, "ChangeRendererName");
        NotificationCenter::instance()->addObserver(this, changeNameWithStringCallback, "ChangeRendererNameWithString");
    }

    ~Renderer()
    {
        vipreLogINFO(examplePrefix) << "Renderer destructor" << std::endl;
        vipre::NotificationCenter::instance()->removeObserver(this);
    }

    void requestRedraw()
    {
        vipreLogINFO(examplePrefix) << _name << " requesting redraw" << std::endl;
    }

    void updateNumRenderPasses(unsigned int renderPasses)
    {
        _renderPasses = renderPasses;
        vipreLogINFO(examplePrefix) << _name << " update numRenderingPasses: " << _renderPasses << std::endl;
    }

    void changeName(const char* name)
    {
        vipreLogINFO(examplePrefix) << _name << " changed name to: " << name << " using const char* data type." << std::endl;
        _name = name;
    }

    void changeNameWithString(const vipre::String& name)
    {
        vipreLogINFO(examplePrefix) << _name << " changed name to: " << name << " using vipre::String data type." << std::endl;
        _name = name;
    }


protected:

    vipre::String _name;
    unsigned int _renderPasses;
};


/**
 * This example demonstrates how to use the vipre NotificationCenter API.
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
 * And that's all there is to it! This example shows off some do's and don't's for passing notifications
 * with and without objects. It also demonstrates what will happen if you don't pass objects with the
 * correct type.
 *
 * The most important thing to remember when using the NotificationCenter is to make sure to remove
 * your observers from the NotificationCenter in the observer object's destructor. If you don't do
 * this, you could potentially be sending notifications to NULL which could crash your application.
 * One safe-guard to this is that the NotificationCenter will post a warning message whenever it is
 * shutdown and still contains observers.
 */
int main(int argc, char **argv)
{
    // Set the logging levels for both vipre and osg
    vipre::Log::instance()->setLogLevel(vipre::INFO_LVL);
    osg::setNotifyLevel(osg::WARN);

    // Use an ArgumentParser object to manage the program arguments.
    osg::ArgumentParser arguments(&argc, argv);
    osg::ApplicationUsage* appUsage = arguments.getApplicationUsage();

    // Set up the usage document, in case we need to print out how to use this program.
    vipre::String description(osgDB::getSimpleFileName(arguments.getApplicationName()) +
                              " is an example which demonstrates how to use the vipre "
                              "NotificationCenter API.");
    appUsage->setDescription(description);
    appUsage->setCommandLineUsage(osgDB::getSimpleFileName(arguments.getApplicationName()));
    appUsage->addCommandLineOption("-h or --help","Display this information");

    // If user request help write it out to std::cout
    if (arguments.read("-h") || arguments.read("--help"))
    {
        appUsage->write(std::cout);
        return 1;
    }

    // Create a few renderers
    Renderer* r1 = new Renderer("Renderer 1");
    Renderer* r2 = new Renderer("Renderer 2");
    Renderer r3("Renderer 3");

    // Delete the first one to make sure the renderer is removed from the notification center appropriately
    vipreLogINFO(examplePrefix) << "Forcibly deleting Renderer 1" << std::endl;
    delete r1;

    // Post a correct and incorrect RequestRedraw notification
    vipre::NotificationCenter::instance()->postNotification("RequestRedraw");
    vipre::NotificationCenter::instance()->postNotificationWithObject("RequestRedraw", 27);

    // Post an UpdateNumRenderPasses notification with an int and an unsigned int.
    // NOTE: if the method signature takes an unsigned int, you must cast the int
    // to an unsigned int before posting the notification.
    vipre::NotificationCenter::instance()->postNotificationWithObject("UpdateNumRenderPasses", 4);
    vipre::NotificationCenter::instance()->postNotificationWithObject("UpdateNumRenderPasses", (unsigned int)8);

    // Change the renderer name using both the const char* and vipre::String methods.
    const char* name1 = "Custom Renderer";
    vipre::String name2 = "Ultimate Renderer";
    vipre::NotificationCenter::instance()->postNotificationWithObject("ChangeRendererName", name1);
    vipre::NotificationCenter::instance()->postNotificationWithObject("ChangeRendererNameWithString", name2);

    // Cleanup renderer 2
    vipreLogINFO(examplePrefix) << "Forcibly deleting Renderer 2" << std::endl;
    delete r2;

    return 0;
}
