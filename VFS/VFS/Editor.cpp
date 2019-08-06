#include "Editor.h"
#include "Core.h"
#include "Graphics.h"

// Include ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void Editor::Init()
{
	// Initializing IMGUI.

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(Core::Get()->GetGraphics()->GetWindow(), true);
	ImGui_ImplOpenGL3_Init(NULL);
	
	// return 0;
}

void Editor::PreUpdate()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Set the window size and position
	ImGui::SetNextWindowSize(ImVec2{447, 598}, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2{ 16,56 }, ImGuiCond_Once);
}

void Editor::Update(float dt)
{
	PreUpdate();

	Graphics* graphics = Core::Get()->GetGraphics();
	// Camera* camera = Engine::Instance()->GetSystem<Camera>();
	{
		// Create an ImGui window. Only 1 of these will be created so no need to encapsulate this.
		if (ImGui::Begin("Graphics Application"))
		{
			// Display the controls for the application
			ImGuiTabBarFlags tab_flags = ImGuiTabBarFlags_NoTooltip | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll;
			if (ImGui::BeginTabBar("Graphics Application", tab_flags))
			{
				
				ImGui::EndTabBar();
			}
		}
		ImGui::End();
		
	}

	PostUpdate();
}

void Editor::PostUpdate()
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Editor::DebugDisplay()
{
	if (ImGui::TreeNode("Debug"))
	{
		
		ImGui::TreePop();
	}
}

void Editor::Shutdown()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}


