
#include "SysLibs.h"

#include "WindowFactory.h"
#include "TraceCamera.h"
#include "Engine.h"

#include "EventHandler.h"
#include "IWindow.h"
#include "TextFunctions.h"
#include "Color.h"

#include "Editor.h"

int main(int argc, const char* argv[])
{
    std::vector<std::shared_ptr<editor::PolygonEntity>> polygon_data;

    // This is assumed to be the file argument
    const char* file = (argc < 2) ? nullptr : argv[1];
    if(file)
    {
        // Load data from file here, and populate the vector.
    }


    constexpr math::Vector2f window_size(1200.0f, 800.0f);

    System::Init();

    // The "global" event handler used throughout the game
    mono::EventHandler eventHandler;

    mono::IWindowPtr window = mono::CreateWindow("Editor", window_size.x, window_size.y, false);
    window->SetBackgroundColor(mono::Color::RGBA(0.7, 0.7, 0.7));

    mono::ICameraPtr camera = std::make_shared<mono::TraceCamera>(window_size.x, window_size.y);
    mono::LoadFont("pixelette.ttf", 10.0f);

    mono::Engine engine(window, camera, eventHandler);
    engine.Run(std::make_shared<editor::EditorZone>(window_size, eventHandler, polygon_data));

    System::Exit();

    return 0;
}


