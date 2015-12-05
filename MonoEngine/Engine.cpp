/*
 *  MonolithEngine.cpp
 *  Monolith1
 *
 *  Created by Niblit on 2011-02-07.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Engine.h"
#include "InputHandler.h"

#include "IWindow.h"
#include "ICamera.h"
#include "IZone.h"

#include "SysTime.h"
#include "SysEvents.h"

#include "EventHandler.h"
#include "EventFuncFwd.h"
#include "PauseEvent.h"
#include "QuitEvent.h"
#include "SurfaceChangedEvent.h"
#include "ActivatedEvent.h"

#include "Renderer.h"

#include "Vector2f.h"
#include "Quad.h"

using namespace mono;

namespace Func
{
    void ScreenToWorld(float& x, float& y, IWindowPtr window, ICameraPtr camera)
    {
        const math::Vector2f& windowSize = window->Size();
        const math::Quad& viewport = camera->GetViewport();
        
        const math::Vector2f& scale = viewport.mB / windowSize;
        
        const float screenX = x;
        const float screenY = windowSize.y - y;
        
        const float tempx = screenX * scale.x;
        const float tempy = screenY * scale.y;
        
        x = tempx + viewport.mA.x;
        y = tempy + viewport.mA.y;
    }
}


Engine::Engine(IWindowPtr window, ICameraPtr camera, EventHandler& eventHandler)
    : mPause(false),
      mQuit(false),
      mWindow(window),
      mCamera(camera),
      mEventHandler(eventHandler)
{
    using namespace std::placeholders;
    const auto func = std::bind(Func::ScreenToWorld, _1, _2, mWindow, mCamera);
    mInputHandler = std::make_shared<InputHandler>(func, mEventHandler);
    
    const Event::PauseEventFunc pauseFunc = std::bind(&Engine::OnPause, this, _1);
    mPauseToken = mEventHandler.AddListener(pauseFunc);
    
    const Event::QuitEventFunc quitFunc = std::bind(&Engine::OnQuit, this, _1);
    mQuitToken = mEventHandler.AddListener(quitFunc);

    const Event::SurfaceChangedEventFunc surfaceChangedFunc = std::bind(&Engine::OnSurfaceChanged, this, _1);
    mSurfaceChangedToken = mEventHandler.AddListener(surfaceChangedFunc);
	
    const Event::ActivatedEventFunc activatedFunc = std::bind(&Engine::OnActivated, this, _1);
    mActivatedToken = mEventHandler.AddListener(activatedFunc);
}

Engine::~Engine()
{
    mEventHandler.RemoveListener(mPauseToken);
    mEventHandler.RemoveListener(mQuitToken);
    mEventHandler.RemoveListener(mSurfaceChangedToken);
    mEventHandler.RemoveListener(mActivatedToken);
}

void Engine::Run(IZonePtr zone)
{
    zone->OnLoad(mCamera);
    
    Renderer renderer(mCamera, mWindow);
    unsigned int lastTime = Time::GetMilliseconds();

    while(!mQuit)
    {
        const unsigned int beforeTime = Time::GetMilliseconds();
        const unsigned int delta = beforeTime - lastTime;

        // Handle input events
        Events::ProcessSystemEvents(mInputHandler);
        if(!mPause)
        {
            // Let the zone add stuff that will be rendered
            zone->Accept(renderer);

            // Update all the stuff, and draw complete frame
            renderer.Update(delta);
            renderer.DrawFrame();
        }
        
        lastTime = beforeTime;

        // Sleep for a millisecond
        Time::Sleep(1);        
    }

    // Remove possible follow entity and unload the zone
    mCamera->Unfollow();
    zone->OnUnload();

    // Reset the quit and pause flag for when you want
    // to reuse the engine for another zone.
    mQuit = false;
    mPause = false;
}

void Engine::OnPause(const Event::PauseEvent&)
{
    mPause = !mPause;
}

void Engine::OnQuit(const Event::QuitEvent&)
{
    mQuit = true;
}

void Engine::OnSurfaceChanged(const Event::SurfaceChangedEvent& event)
{
    mWindow->SurfaceChanged(event.width, event.height);
}

void Engine::OnActivated(const Event::ActivatedEvent& event)
{
    mWindow->Activated(event.gain);
}


