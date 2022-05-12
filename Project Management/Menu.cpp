#include "Menu.h"
#pragma warning(disable:4996)

std::wstring openfilename() {
	OPENFILENAME ofn;
	wchar_t fileName[MAX_PATH] = L"";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Excel Files (*.xls*;*.xlsx*)\0*.xls*;*.xlsx*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"";


	std::wstring fileNameStr;

	if (GetOpenFileName(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}

std::wstring savefilename() {
	OPENFILENAME ofn;
	wchar_t fileName[MAX_PATH] = L"ExFile.xlsx";
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"Excel Files (*.xls*;*.xlsx*)\0*.xls*;*.xlsx*\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"";


	std::wstring fileNameStr;

	if (GetSaveFileName(&ofn))
		fileNameStr = fileName;

	return fileNameStr;
}

std::string cMenu::utf8_encode(const std::wstring& wstr)
{
	if (wstr.empty()) return std::string();
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

std::wstring cMenu::utf8_decode(const std::string& str)
{
	if (str.empty()) return std::wstring();
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

bool check_task_exist(std::wstring t) {
	for (int i = 0; i < task.size(); i++)
		if (t == task.at(i).task)
			return 1;
	return 0;
}
bool run = true;
void cMenu::DrawMenu(LPDIRECT3DDEVICE9 pDevice) {
	InitializeImGuiStartFrame(pDevice);
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGui::Begin("Project Management", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
	ImVec2 WorkSpaceSize = ImGui::GetContentRegionAvail();   // Resize canvas to what's available
	ImGui::BeginChild("w1", ImVec2(WorkSpaceSize.x / 2.5, WorkSpaceSize.y), true);
	//ImGui::SetCursorPosX(ImGui::GetContentRegionAvail().x / 2 - 120 * 3 / 2);
	if (ImGui::Button("Add Task", ImVec2(100, 30))) {
		ImGui::OpenPopup("Add Task");
	}
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 10.0f);
	// Always center this window when appearing
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Add Task", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char t[1024] = "", descr[1024] = "", pre[1024] = "";
		static int dur = 0;
		ImGui::Text("Task:");
		ImGui::InputText("##task", t, IM_ARRAYSIZE(t));
		ImGui::Text("Describe:");
		ImGui::InputText("##descr", descr, IM_ARRAYSIZE(descr));
		ImGui::Text("Duration (> 0):");
		ImGui::InputInt("##dur", &dur, 1);
		ImGui::Text("Predecessors:");
		ImGui::InputText("##pre", pre, IM_ARRAYSIZE(pre));
		ImGui::SetCursorPosX(40);
		if (ImGui::Button("Add", ImVec2(120, 0))) {
			if (dur > 0 && !check_task_exist(utf8_decode(t))) {
				Task tmp;
				tmp.task = utf8_decode(t);
				tmp.descr = utf8_decode(descr);
				tmp.dur = dur;
				tmp.pre = utf8_decode(pre);
				if (tmp.pre != L"") {
					if (tmp.pre.find(L",") != std::wstring::npos) {
						std::wstring t = tmp.pre;
						int count = 0;
						size_t found = t.find(L",");
						while (found != std::wstring::npos) {
							count++;
							t.erase(0, found + 1);
							found = t.find(L",");
						}
						tmp.deg_in = count + 1;
					}
					else
						tmp.deg_in = 1;
				}
				else
					tmp.deg_in = 0;
				tmp.deg_out = 0;
				tmp.rank = 0;
				tmp.critical = false;
				task.push_back(tmp);
				t[0] = '\0';
				descr[0] = '\0';
				dur = 0;
				pre[0] = '\0';
				cG->calc();
				ImGui::CloseCurrentPopup();
			}
		}
		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(120, 0))) {
			t[0] = '\0';
			descr[0] = '\0';
			dur = 0;
			pre[0] = '\0';
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::PopStyleVar();
	ImGui::SameLine();
	if (ImGui::Button("Import Excel", ImVec2(120, 30))) {
		Excel->Import(openfilename().c_str());
		cG->calc();
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete", ImVec2(120, 30)) && task.size() > 0)
		ImGui::OpenPopup("my_select_popup");

	if (ImGui::BeginPopup("my_select_popup"))
	{
		ImGui::Text("Select");
		ImGui::Separator();
		for (int i = 0; i < task.size(); i++)
			if (ImGui::Selectable(utf8_encode(L"Task: " + task.at(i).task).c_str())) {
				if (task.size() > 1)
					cG->deletetask(i);
				else
					task.clear();
			}
				
		ImGui::EndPopup();
	}
	ImGui::SameLine();
	if(ImGui::Button("Delete All", ImVec2(120, 30))) {
		task.clear();
	}
	ImGui::PushStyleColor(ImGuiCol_Button, ImGuiCol_FrameBg);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.510f, 0.510f, 0.510f, 1.000f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.310f, 0.310f, 0.310f, 1.000f));
	ImGui::BeginTable("table1", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable);
	enum ContentsType { CT_Text, CT_FillButton };
	static int contents_type = CT_FillButton;
	ImGui::TableSetupColumn("Task", ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableSetupColumn("Describe");
	ImGui::TableSetupColumn("Duration", ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableSetupColumn("Predecessors", ImGuiTableColumnFlags_WidthFixed);
	ImGui::TableHeadersRow();
	wchar_t a[100];
	for (int row = 0; row < task.size(); row++)
	{

		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0);
		std::string tmp = utf8_encode(task.at(row).task.c_str());
		ImGui::Button(tmp.c_str(), ImVec2(-FLT_MIN, 0.0f));

		ImGui::TableSetColumnIndex(1);
		tmp = utf8_encode(task.at(row).descr.c_str());
		ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.50f));
		ImGui::Button(tmp.c_str(), ImVec2(-FLT_MIN, 0.0f));
		ImGui::PopStyleVar();
		ImGui::TableSetColumnIndex(2);
		ImGui::Button(std::to_string(task.at(row).dur).c_str(), ImVec2(-FLT_MIN, 0.0f));
		ImGui::TableSetColumnIndex(3);
		tmp = utf8_encode(task.at(row).pre.c_str());
		ImGui::Button(tmp.c_str(), ImVec2(-FLT_MIN, 0.0f));
	}
	ImGui::EndTable();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("w2", ImVec2(-FLT_MIN, WorkSpaceSize.y), true);
	if (task.size() > 0 && total_dur > 0 && run) {
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),"Duration: %d", total_dur);
		ImU32 Col = IM_COL32(41, 74, 122, 255);
		ImGui::BeginChild("gantt", ImVec2(35 * total_dur + 30, task.size() * 45 + 30), false, ImGuiWindowFlags_AlwaysAutoResize);
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, ImVec2(canvas_p0.x + 35 * total_dur , canvas_p0.y + 45 * task.size() ), IM_COL32(50, 50, 50, 255));
		draw_list->AddRect(canvas_p0, ImVec2(canvas_p0.x + 35 * total_dur , canvas_p0.y + 45 * task.size() ), IM_COL32(255, 255, 255, 255));
		draw_list->PushClipRect(canvas_p0, ImVec2(canvas_p0.x + 35 * total_dur , canvas_p0.y + 45 * task.size() ), true);
		for (int i = 0; i <= total_dur; i++) {
			draw_list->AddText(ImVec2(canvas_p0.x + 20 + (i) * 32 - ImGui::CalcTextSize(std::to_string(i).c_str()).x / 2,
				canvas_p0.y + 10 - ImGui::CalcTextSize(std::to_string(i).c_str()).y / 2),
				IM_COL32(255, 255, 255, 255), std::to_string(i).c_str());
			draw_list->AddLine(ImVec2(canvas_p0.x + (i + 1) * 32 + 3, canvas_p0.y), ImVec2(canvas_p0.x + (i + 1) * 32 + 3, canvas_p0.y + 20), IM_COL32(200, 200, 200, 40));
		}
		draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + 20), ImVec2(canvas_p0.x + (total_dur) * 32 + 40, canvas_p0.y + 20), IM_COL32(200, 200, 200, 40));
		for (int row = 0; row < task.size(); row++) {
			draw_list->AddText(ImVec2(canvas_p0.x + 20 - ImGui::CalcTextSize(utf8_encode(task.at(row).task).c_str()).x / 2,
				canvas_p0.y + (row + 1) * 40 - ImGui::CalcTextSize(utf8_encode(task.at(row).task).c_str()).y / 2),
				IM_COL32(255, 255, 255, 255), utf8_encode(task.at(row).task).c_str());
			if (task.at(row).critical)
				Col = IM_COL32(255, 0, 0, 255);
			else
				Col = IM_COL32(66, 150, 250, 255);
			draw_list->AddRectFilled(ImVec2(canvas_p0.x + 3 + (task.at(row).t + 1) * 32, canvas_p0.y + (row + 1) * 35 + row * 5),
				ImVec2(canvas_p0.x + 3 + (task.at(row).t + task.at(row).dur + 1) * 32, canvas_p0.y + (row + 1) * 35 + 15 + row * 5), Col);

			draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + (row + 1) * 40 + 20), ImVec2(canvas_p0.x + (total_dur) * 32 + 40, canvas_p0.y + (row + 1) * 40 + 20), IM_COL32(200, 200, 200, 40));

		}

		for (int row = 0; row < task.size(); row++) {
			for (int e = 0; e < task.at(row).adj_out.size(); e++) {
				if (task.at(row).critical && task.at(cG->find_id_task(task.at(row).adj_out.at(e))).critical)
					Col = IM_COL32(255, 0, 0, 255);
				else
					Col = IM_COL32(66, 150, 250, 255);
				ImVec2 pos_from(canvas_p0.x + 3 + (task.at(row).t + task.at(row).dur + 1) * 32 - 20, canvas_p0.y + (row + 1) * 35 + 15 + row * 5),
					pos_mid(canvas_p0.x + 3 + (task.at(row).t + task.at(row).dur + 1) * 32 - 20, canvas_p0.y + (cG->find_id_task(task.at(row).adj_out.at(e)) + 1) * 35 + cG->find_id_task(task.at(row).adj_out.at(e)) * 5 + 8),
					pos_to(canvas_p0.x + 3 + (task.at(cG->find_id_task(task.at(row).adj_out.at(e))).t + 1) * 32, canvas_p0.y + (cG->find_id_task(task.at(row).adj_out.at(e)) + 1) * 35 + cG->find_id_task(task.at(row).adj_out.at(e)) * 5 + 8);
				//draw_list->AddLine(pos_from,pos_to, IM_COL32(255, 255, 255, 255),1.5f);
				draw_list->AddBezierQuadratic(pos_from, pos_mid, pos_to, Col, 1.0f, 0);
				ImVec2 arr_p1(canvas_p0.x + 3 + (task.at(cG->find_id_task(task.at(row).adj_out.at(e))).t + 1) * 32 - 3, canvas_p0.y + (cG->find_id_task(task.at(row).adj_out.at(e)) + 1) * 35 + cG->find_id_task(task.at(row).adj_out.at(e)) * 5 + 8 - 2),
					arr_p2(canvas_p0.x + 3 + (task.at(cG->find_id_task(task.at(row).adj_out.at(e))).t + 1) * 32 - 3, canvas_p0.y + (cG->find_id_task(task.at(row).adj_out.at(e)) + 1) * 35 + cG->find_id_task(task.at(row).adj_out.at(e)) * 5 + 8 + 2);
				draw_list->AddTriangleFilled(pos_to, arr_p1, arr_p2, Col);
			}
		}
		draw_list->PopClipRect();
		ImGui::EndChild();
		ImGui::TextColored(ImVec4(1.000f, 0.000f, 0.000f, 1.000f), "Red: Critical");
		ImGui::TextColored(ImVec4(0.259f, 0.588f, 0.980f, 1.000f), "Blue: Non-critical");
	}
	ImGui::EndChild();
	ImGui::End();
	InitializeImGuiEndFrame(pDevice);
}

void cMenu::InitializeImGui(LPDIRECT3DDEVICE9 pDevice, HWND hwnd)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuiStyle& style = ImGui::GetStyle();
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(pDevice);

	char fontdir[MAX_PATH];
	GetWindowsDirectoryA(fontdir, sizeof(fontdir));
	strcat(fontdir, "\\Fonts\\tahoma.ttf");

	io.Fonts->AddFontFromFileTTF(fontdir, 20.0f, NULL, io.Fonts->GetGlyphRangesVietnamese());
	io.Fonts->AddFontFromFileTTF(fontdir, 26.0f, NULL, io.Fonts->GetGlyphRangesVietnamese());
	io.Fonts->AddFontFromFileTTF(fontdir, 14.0f, NULL, io.Fonts->GetGlyphRangesVietnamese());
	io.Fonts->AddFontFromFileTTF(fontdir, 18.0f, NULL, io.Fonts->GetGlyphRangesVietnamese());

	style.FrameRounding = 4.0f;
	style.Colors[ImGuiCol_Button] = ImVec4(0.153f, 0.682f, 0.376f, 0.784f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.153f, 0.682f, 0.376f, 1.000f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.000f, 0.682f, 0.376f, 1.000f);

	io.IniFilename = NULL;
}

void cMenu::InitializeImGuiStartFrame(LPDIRECT3DDEVICE9 pDevice)
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void cMenu::InitializeImGuiEndFrame(LPDIRECT3DDEVICE9 pDevice)
{
	// Rendering
	ImGui::EndFrame();
	pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	pDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, NULL, 1.0f, 0);
	if (pDevice->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		pDevice->EndScene();
	}
}