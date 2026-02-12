#include "gui.hpp"

ImGuiLayer::ImGuiLayer(Window& window):
m_window_renderer(window.GetRenderer())
{
    Init(window);
}

ImGuiLayer::~ImGuiLayer()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::Init(Window& window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplSDL2_InitForSDLRenderer(window.GetWindow(), m_window_renderer);
    ImGui_ImplSDLRenderer2_Init(m_window_renderer);
}

void ImGuiLayer::SetFrame() const
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("CellAuto");
    ImGui::Text("Hello ImGui !");
    ImGui::End();
}

void ImGuiLayer::Draw() const
{
    SetFrame();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_window_renderer);
}