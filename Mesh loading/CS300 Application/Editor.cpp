#include "Editor.h"
#include "Engine.h"
#include "Graphics.h"
#include "InputSystem.h"
#include "Camera.h"
#include "SceneManager.h"
// Include ImGui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int Editor::Init()
{
	// Initializing IMGUI.

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(Engine::Instance() ->GetSystem<Graphics>()->GetWindow(), true);
	ImGui_ImplOpenGL3_Init(NULL);
	
	return 0;
}

void Editor::PreUpdate()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// We do not want ImGui
	if (Engine::Instance()->GetSystem<InputSystem>()->IsCursorShown() == false)
	{
		// Do not pass inputs to ImGui if the cursor not shown (Let appplication handle inputs)
		ImGui::SetWindowFocus(NULL);
	}

	// Set the window size and position
	ImGui::SetNextWindowSize(ImVec2{447, 598}, ImGuiCond_Once);
	ImGui::SetNextWindowPos(ImVec2{ 16,56 }, ImGuiCond_Once);
}

int Editor::Update(float dt)
{
	PreUpdate();

	Graphics* graphics = Engine::Instance()->GetSystem<Graphics>();
	Camera* camera = Engine::Instance()->GetSystem<Camera>();
	{
		// Create an ImGui window. Only 1 of these will be created so no need to encapsulate this.
		if (ImGui::Begin("Graphics Application"))
		{
			// Display the controls for the application
			ImGuiTabBarFlags tab_flags = ImGuiTabBarFlags_NoTooltip | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_NoCloseWithMiddleMouseButton | ImGuiTabBarFlags_FittingPolicyScroll;
			if (ImGui::BeginTabBar("Graphics Application", tab_flags))
			{
				Engine::Instance()->GetSystem<SceneManager>()->GetActiveScene()->DisplayImGui();

				// Display editable camera settings
				if (ImGui::BeginTabItem("Camera Settings"))
				{
					ImGui::NewLine();
					ImGui::DragFloat("Mouse Sensitivity", (float*)&camera->sensitivity, 0.01f, 0.01f, 0.5f);
					ImGui::DragFloat("Movement Speed", (float*)&camera->movSpeed);
					if (ImGui::DragFloat("Vertical Rotate", &camera->pitch, 0.1f))
						camera->updateCameraVectors();
					if (ImGui::DragFloat("Horizontal Rotate", &camera->yaw, 0.1f))
						camera->updateCameraVectors();
					if (ImGui::Button("Reset Camera", ImVec2{ 100, 40 }))
					{
						Engine::Instance()->GetSystem<Camera>()->Reset();
						Engine::Instance()->GetSystem<Camera>()->position = glm::vec3{ 0,0,4 };
					}

					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Help"))
				{
					ImGui::NewLine();
					ImGui::TextColored(ImVec4{ 0,1,0,1 }, "- Click outside the editor and press TAB to enable camera.");
					ImGui::TextColored(ImVec4{ 0,1,0,1 }, "- WASD to move.");
					ImGui::TextColored(ImVec4{ 0,1,0,1 }, "- SPACE to go higher. SHIFT to go down. ");
					ImGui::TextColored(ImVec4{ 0,1,0,1 }, "- MOUSE to look around.");
					ImGui::NewLine();
					
					// Display the debug stuff.
					DebugDisplay();
					ImGui::EndTabItem();
				}

				// Display the scene specific details.
				// ImGui::Text("Scene Settings"); ImGui::Separator();

				ImGui::EndTabBar();
			}
		}
		ImGui::End();
		
	}

	PostUpdate();

	return 0;
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
		// Show FPS
		Camera* camera = Engine::Instance()->GetSystem<Camera>();
		ImGui::Text("Application Debug");
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		// Check for inputs and other ImGui functions
		ImGui::Text("Input Debug");
		ImGui::Separator();
		InputSystem* inputs = Engine::Instance()->GetSystem<InputSystem>();
		ImGui::TextColored(inputs->IsCursorShown() ? ImVec4{ 0,1,0,1 } : ImVec4{ 1,0,0,1 }, "Cursor Shown");
		ImGui::TextColored(ImGui::IsAnyWindowFocused() ? ImVec4{ 0,1,0,1 } : ImVec4{ 1,0,0,1 }, "Window Focused");
		ImGui::TextColored(ImGui::IsAnyWindowHovered() ? ImVec4{ 0,1,0,1 } : ImVec4{ 1,0,0,1 }, "Window Hovered");

		// Camera variables
		ImGui::Text("Camera Debug");
		ImGui::Separator();
		ImGui::Text("Yaw: %f", camera->yaw);
		ImGui::Text("Pitch: %f", camera->pitch);
		ImGui::Text("Zoom: %f", camera->zoom);

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


