#include "gui.hpp"

ImGuiLayer::ImGuiLayer(Window& window, float& stepTimer, SDL_Color& bgColor, Grid* grid, Camera& camera):
m_windowRenderer(window.GetRenderer()), m_stepTimer(stepTimer), m_bgColor(bgColor), m_grid(grid), m_selectedAutomata(1), m_camera(camera)
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

void ImGuiLayer::SetFrame()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowSize(ImVec2(400, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("CellAuto");

    if (ImGui::BeginTabBar("TabBar")){
        if (ImGui::BeginTabItem("Automata")){
            const char* items[] = { "Elementary", "Game of Life", "Langton's Ant", "Greenberg-Hastings", "Forest fire model"}; // Must be the same order than SetAutomata

            if (ImGui::BeginCombo("Automata", items[m_selectedAutomata])){
                for (int i = 0; i < IM_ARRAYSIZE(items); i++){
                    if (ImGui::Selectable(items[i])){
                        m_selectedAutomata = i;
                        Notify(static_cast<SetAutomata>(m_selectedAutomata));
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::Button("Fill")) 
                m_grid->Fill();
            ImGui::SameLine();
            if (ImGui::Button("Empty")) 
                m_grid->Empty();
            ImGui::SameLine();
            if (ImGui::Button("Randomize")) 
                m_grid->Randomize();
            int& gridDensity = m_grid->GetDensity();
            ImGui::SliderInt("Density (%)", &gridDensity, 0, 100);

            m_grid->SetAutomataGUI();
            
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Settings")){
            ImGui::SliderFloat("Timer", &m_stepTimer, 0.001f, 1.0f);
            int& gridWidth = m_grid->GetWidth();
            if (ImGui::SliderInt("Grid width", &gridWidth, 1, 200)){
                m_grid->Resize();
                m_grid->ComputeCellSize();
                m_camera.LookAtGrid(m_grid->GetSize()/2);
            }
            int& gridHeight = m_grid->GetHeight();
            if (ImGui::SliderInt("Grid height", &gridHeight, 1, 200)){
                m_grid->Resize();
                m_grid->ComputeCellSize();
                m_camera.LookAtGrid(m_grid->GetSize()/2);
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors")){
            float bgColor[3] = {m_bgColor.r/255.f, m_bgColor.g/255.f, m_bgColor.b/255.f};
            ImGui::ColorEdit3("Background color", bgColor);
            m_bgColor = {static_cast<Uint8>(bgColor[0]*255), static_cast<Uint8>(bgColor[1]*255), static_cast<Uint8>(bgColor[2]*255)};
            
            // Should be in SetColorGUI() ?
            SDL_Color& currentColor = m_grid->GetCellColor();
            float cellColor[3] = {currentColor.r/255.f, currentColor.g/255.f, currentColor.b/255.f};
            ImGui::ColorEdit3("Cell color", cellColor);
            currentColor = {static_cast<Uint8>(cellColor[0]*255), static_cast<Uint8>(cellColor[1]*255), static_cast<Uint8>(cellColor[2]*255)};

            m_grid->SetColorGUI();
            
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
}

void ImGuiLayer::Draw()
{
    SetFrame();
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_windowRenderer);
}

void ImGuiLayer::SetGrid(Grid* grid){
    m_grid = grid;
}