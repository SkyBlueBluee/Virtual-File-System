#include "TestSys.h"

void TestSys::init()
{
	// vfs = new VFS{};

	// Should allow you to mount directories under an alias.
	// EG:
	// Instead of user typing "C:/......../resource/textures/blah.png"
	// We should support "/resource/textures".
	// This allows our code to be more modular (as long as the alias doesnt change, you can change the underlying dir path)
	
	// When "." is passed as the path parameter, we use the current working directory.
	// vfs->mount(".", "/data");
	// vfs->mount(vfs->getAppDataPath(), "/saves"
}

void TestSys::update(float dt)
{
	ImGui::NewFrame();
	ImGui::Begin("Test Driver", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Below are the tests for the virtual file system...");
	ImGui::NewLine();
	if (ImGui::TreeNode("Stats"))
	{
		ImGui::TreePop();
	}
	//ImGui::TextColored(ImVec4{ 0,1,0,1 }, "Stats:");
	ImGui::Text("Testing mount/dismount");
	ImGui::SameLine();
	ImGui::Text("");

	ImGui::End();
	ImGui::EndFrame();
	ImGui::Render();

}

void TestSys::shutdown()
{
}
