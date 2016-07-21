//
//  Editor.cpp
//  MonoiOS
//
//  Created by Niklas Damberg on 02/07/16.
//
//

#include "Editor.h"

#include "IRenderer.h"

#include "TextureFactory.h"
#include "ITexture.h"

#include "ZoneBase.h"
#include "ISprite.h"
#include "SpriteFactory.h"
#include "EntityBase.h"
#include "Color.h"

#include "UserInputController.h"

#include "ImGuiInputHandler.h"
#include "ImGuiInterfaceDrawer.h"
#include "ImGuiRenderer.h"

#include "EventFuncFwd.h"
#include "EventHandler.h"
#include "SurfaceChangedEvent.h"

namespace
{
    class SpriteDrawable : public mono::EntityBase
    {
    public:

        SpriteDrawable(const char* file)
        {
            m_sprite = mono::CreateSprite(file);
            SetPosition(math::Vector2f(100, 100));
            SetScale(math::Vector2f(50, 50));
        }

        virtual void Draw(mono::IRenderer& renderer) const
        {
            renderer.DrawSprite(*m_sprite.get());
            renderer.DrawText("hello", Position(), true, mono::Color::RGBA(1, 0, 1, 1));
        }

        virtual void Update(unsigned int delta)
        {
            m_sprite->doUpdate(delta);
        }

        mono::ISpritePtr m_sprite;
    };
}

using namespace editor;

EditorZone::EditorZone(const math::Vector2f& window_size,
                       mono::EventHandler& event_handler,
                       const std::vector<std::shared_ptr<editor::PolygonEntity>>& polygon_data)
    : m_windowSize(window_size),
      m_eventHandler(event_handler),
      m_inputHandler(event_handler),
      m_polygons(polygon_data)
{
    using namespace std::placeholders;

    m_context.showContextMenu = false;
    m_context.contextMenuCallback = std::bind(&EditorZone::OnContextMenu, this, _1);
    m_context.contextMenuItems = { "Polygon", "Decoration" };

    m_context.selectedPolygonIndex = -1;
    m_context.polygonSelected = std::bind(&EditorZone::OnSelectedPolygon, this, _1);
    m_context.polygonDeleted = std::bind(&EditorZone::OnDeletePolygon, this, _1);

    m_context.editorMenuCallback = std::bind(&EditorZone::EditorMenuCallback, this, _1);
    m_context.toolsMenuCallback = std::bind(&EditorZone::ToolsMenuCallback, this, _1);

    m_interfaceDrawer = std::make_shared<editor::ImGuiInterfaceDrawer>(m_context);

    mono::ITexturePtr texture = mono::CreateTexture("cacodemon.png");
    std::unordered_map<unsigned int, mono::ITexturePtr> textures;
    textures.insert(std::make_pair(texture->Id(), texture));

    m_guiRenderer = std::make_shared<editor::ImGuiRenderer>(m_windowSize, textures);

    const event::SurfaceChangedEventFunc surface_func = std::bind(&EditorZone::OnSurfaceChanged, this, _1);
    m_surfaceChangedToken = m_eventHandler.AddListener(surface_func);
}

EditorZone::~EditorZone()
{
    m_eventHandler.RemoveListener(m_surfaceChangedToken);
}

void EditorZone::OnLoad(mono::ICameraPtr camera)
{
    m_userInputController = std::make_shared<editor::UserInputController>(camera, this, &m_context, m_windowSize, m_eventHandler);

    AddUpdatable(m_interfaceDrawer);
    AddDrawable(m_guiRenderer, 1);
    AddEntity(std::make_shared<SpriteDrawable>("shuttle.sprite"), 0);
}

void EditorZone::OnUnload()
{ }

bool EditorZone::OnSurfaceChanged(const event::SurfaceChangedEvent& event)
{
    if(event.width > 0 && event.height > 0)
        m_guiRenderer->SetWindowSize(math::Vector2f(event.width, event.height));

    return false;
}

void EditorZone::AddPolygon(const std::shared_ptr<editor::PolygonEntity>& polygon)
{
    AddEntity(polygon, 0);
    m_polygons.push_back(polygon);
    m_context.polygonItems.push_back("Polygon: " + std::to_string(m_polygons.size()));
}

void EditorZone::OnContextMenu(int index)
{
    m_userInputController->HandleContextMenu(index);
}

void EditorZone::OnSelectedPolygon(int index)
{
    m_context.selectedPolygonIndex = index;

    for(int polygon_index = 0; polygon_index < m_polygons.size(); ++polygon_index)
        m_polygons[polygon_index]->SetSelected(polygon_index == index);
}

void EditorZone::OnDeletePolygon(int index)
{
    const auto remove_polygon_func = [this, index] {
        std::shared_ptr<editor::PolygonEntity> polygon = m_polygons[index];

        m_polygons.erase(m_polygons.begin() + index);
        this->RemoveEntity(polygon);

        m_context.polygonItems.erase(m_context.polygonItems.begin() + index);
    };

    SchedulePreFrameTask(remove_polygon_func);
}

void EditorZone::EditorMenuCallback(int index)
{
    std::printf("Editor %u\n", index);
}

void EditorZone::ToolsMenuCallback(int index)
{
    m_userInputController->SelectTool(index);
}
