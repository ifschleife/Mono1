
#pragma once

#include "MonoFwd.h"
#include "Rendering/RenderFwd.h"
#include "Events/EventFwd.h"
#include "EventHandler/EventToken.h"
#include "Math/Vector.h"

namespace mono
{
    class MouseCameraController
    {
    public:
        
        MouseCameraController(int window_width, int window_height, ICamera* camera, EventHandler& event_handler);
        ~MouseCameraController();

        void Enable();
        void Disable();
        
    private:
        
        bool OnMouseDown(const event::MouseDownEvent& event);
        bool OnMouseUp(const event::MouseUpEvent& event);
        bool OnMouseMove(const event::MouseMotionEvent& event);
        bool OnMultiGesture(const event::MultiGestureEvent& event);
        
        int m_window_width;
        int m_window_height;
        ICamera* m_camera;
        EventHandler& m_event_handler;
        bool m_enabled;

        bool m_translate;
        math::Vector m_translate_delta;

        mono::EventToken<event::MouseDownEvent> m_mouse_down_token;
        mono::EventToken<event::MouseUpEvent> m_mouse_up_token;
        mono::EventToken<event::MouseMotionEvent> m_mouse_move_token;
        mono::EventToken<event::MultiGestureEvent> m_multi_gesture_token;
    };
}
