#include "gui.hpp"

ImGuiLayer::ImGuiLayer(Window& window, float& stepTimer, SDL_Color& bgColor, Grid& grid):
m_windowRenderer(window.GetRenderer()), m_stepTimer(stepTimer), m_bgColor(bgColor), m_grid(grid), 
m_cellColor(m_grid.GetCellColor()), m_gridWidth(m_grid.GetWidth()), m_gridHeight(m_grid.GetHeight())
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
    ImGui_ImplSDL2_InitForSDLRenderer(window.GetWindow(), m_windowRenderer);
    ImGui_ImplSDLRenderer2_Init(m_windowRenderer);
}

void ImGuiLayer::SetFrame() const
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("CellAuto");

    ImGui::SliderFloat("Timer", &m_stepTimer, 0.001f, 1.0f);

    if (ImGui::SliderInt("Grid width", &m_gridWidth, 1, 200)){
        m_grid.Resize();
        m_grid.ComputeSize();
    }
    if (ImGui::SliderInt("Grid height", &m_gridHeight, 1, 200)){
        m_grid.Resize();
        m_grid.ComputeSize();
    }

    if (ImGui::Button("Fill")) 
        m_grid.Fill();
    ImGui::SameLine();
    if (ImGui::Button("Empty")) 
        m_grid.Empty();
    ImGui::SameLine();
    if (ImGui::Button("Randomize")) 
        m_grid.Randomize();

    float cellColor[3] = {m_cellColor.r/255.f, m_cellColor.g/255.f, m_cellColor.b/255.f};
    float bgColor[3] = {m_bgColor.r/255.f, m_bgColor.g/255.f, m_bgColor.b/255.f};
    ImGui::ColorEdit3("Background color", bgColor);
    ImGui::ColorEdit3("Cell color", cellColor);
    m_cellColor = {static_cast<Uint8>(cellColor[0]*255), static_cast<Uint8>(cellColor[1]*255), static_cast<Uint8>(cellColor[2]*255)};
    m_bgColor = {static_cast<Uint8>(bgColor[0]*255), static_cast<Uint8>(bgColor[1]*255), static_cast<Uint8>(bgColor[2]*255)};

    ImGui::End();
}

void ImGuiLayer::Draw() const
{
    SetFrame();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_windowRenderer);
}