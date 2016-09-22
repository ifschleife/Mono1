//
//  ImGuiInterfaceDrawer.cpp
//  MonoiOS
//
//  Created by Niklas Damberg on 20/06/16.
//
//

#include "ImGuiInterfaceDrawer.h"
#include "imgui/imgui.h"

using namespace editor;

ImGuiInterfaceDrawer::ImGuiInterfaceDrawer(UIContext& context)
    : m_context(context)
{ }

void ImGuiInterfaceDrawer::doUpdate(unsigned int delta)
{
    ImGui::GetIO().DeltaTime = float(delta) / 1000.0f;

    ImGui::NewFrame();

    ImGui::BeginMainMenuBar();
    if(ImGui::BeginMenu("Editor"))
    {
        if(ImGui::MenuItem("Open"))
            m_context.editorMenuCallback(EditorMenuOptions::OPEN);

        if(ImGui::MenuItem("Save"))
            m_context.editorMenuCallback(EditorMenuOptions::SAVE);

        if(ImGui::MenuItem("Export"))
            m_context.editorMenuCallback(EditorMenuOptions::EXPORT);

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Tools"))
    {
        if(ImGui::MenuItem("Polygon", "1"))
            m_context.toolsMenuCallback(ToolsMenuOptions::POLYGON_TOOL);

        if(ImGui::MenuItem("Polygon Brush", "2"))
            m_context.toolsMenuCallback(ToolsMenuOptions::POLYGON_BRUSH_TOOL);

        if(ImGui::MenuItem("Translate", "3"))
            m_context.toolsMenuCallback(ToolsMenuOptions::TRANSLATE_TOOL);

        if(ImGui::MenuItem("Rotate", "4", true))
            m_context.toolsMenuCallback(ToolsMenuOptions::ROTATE_TOOL);

        if(ImGui::MenuItem("Camera", "5", false, false))
            m_context.toolsMenuCallback(ToolsMenuOptions::CAMERA_TOOL);
        
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Options"))
    {
        static ImVec4 color(1.0f, 0.0f, 1.0f, 1.0f);
        ImGui::ColorEdit3("BG Color", &color.x);

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    if(m_context.has_selection)
    {
        ImGui::Begin("Selection", nullptr, ImVec2(200, 120), true, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

        ImGui::Value("X", m_context.polygon_x);
        ImGui::SameLine();
        ImGui::Value("Y", m_context.polygon_y);
        ImGui::Value("Rotation", m_context.polygon_rotation);
        if(ImGui::SliderFloat("Repeate", &m_context.texture_repeate, 1.0f, 10.0f))
            m_context.texture_repeate_callback(m_context.texture_repeate);

        if(ImGui::Combo("Texture", &m_context.texture_index, m_context.texture_items, m_context.texture_items_count))
            m_context.texture_changed_callback(m_context.texture_index);

        if(ImGui::Button("Delete"))
            m_context.delete_callback();

        ImGui::End();
    }

    if(m_context.showContextMenu)
    {
        ImGui::OpenPopup("context");
        m_context.showContextMenu = false;
    }

    if(ImGui::BeginPopup("context"))
    {
        ImGui::Text("Create...");
        ImGui::Separator();

        int menu_index = -1;

        for(size_t index = 0; index < m_context.contextMenuItems.size(); ++index)
        {
            if(ImGui::Selectable(m_context.contextMenuItems.at(index).c_str()))
                menu_index = index;
        }

        ImGui::EndPopup();

        if(menu_index != -1)
            m_context.contextMenuCallback(menu_index);
    }

    constexpr int notification_window_flags =
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings;

    const ImVec2 window_size = ImVec2(160.0f, 50.0f);
    const float window_position = ImGui::GetIO().DisplaySize.x - window_size.x;

    for(size_t index = 0; index < m_context.notifications.size(); ++index)
    {
        Notification& note = m_context.notifications[index];

        float window_alpha = 0.6f;
        ImColor tint = ImColor(1.0f, 1.0f, 1.0f, 1.0f);

        if(note.time_left < 500)
        {
            const float alpha_scale = float(note.time_left) / 500.0f;
            window_alpha *= alpha_scale;
            tint.Value.w *= alpha_scale;
        }

        note.time_left -= delta;

        char window_id[16];
        std::sprintf(window_id, "overlay: %zu", index);

        ImGui::SetNextWindowPos(ImVec2(window_position - 10, index * 60 + 30));

        ImGui::Begin(window_id, nullptr, window_size, window_alpha, notification_window_flags);
        ImGui::Image(reinterpret_cast<void*>(note.image), ImVec2(32.0f, 32.0f), ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), tint);
        ImGui::SameLine();
        ImGui::TextColored(tint, "%s", note.text.c_str());
        ImGui::End();
    }

    const auto remove_notification_func = [](const Notification& note) {
        return note.time_left <= 0;
    };

    const auto it = std::remove_if(m_context.notifications.begin(), m_context.notifications.end(), remove_notification_func);
    m_context.notifications.erase(it, m_context.notifications.end());

    //ImGui::ShowTestWindow();

    ImGui::Render();
}