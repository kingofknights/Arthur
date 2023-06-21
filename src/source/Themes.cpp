#include "../include/Themes.hpp"

#include "../include/Colors.hpp"
#include "../include/PreCompileHeader.hpp"
#include "../include/Structure.hpp"

namespace Themes {
static const ImWchar icons_ranges[] = {ICON_MIN_MD, ICON_MAX_16_MD, 0};

void AddIconFonts(const std::string &ttf_, float size_) {
	ImGuiIO &io = ImGui::GetIO();
	//	 io.Fonts->AddFontDefault();

	float		 baseFontSize = size_;
	float		 iconFontSize = baseFontSize;
	ImFontConfig icons_config;
	icons_config.MergeMode	 = true;
	icons_config.PixelSnapH	 = true;
	icons_config.GlyphOffset = ImVec2(0, 4);
	io.Fonts->AddFontFromFileTTF(ttf_.data(), iconFontSize);
	io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_MD, iconFontSize, &icons_config, icons_ranges);

	LOG(INFO, "Adding Fonts Style {} : {}", ttf_, iconFontSize)
	LOG(INFO, "Adding Fonts Icon {} : {}", FONT_ICON_FILE_NAME_MD, iconFontSize)

	auto &style				= ImGui::GetStyle();
	style.WindowRounding	= 2.0f;
	style.WindowBorderSize	= 0.0f;
	style.FrameRounding		= 2.0f;
	style.FrameBorderSize	= 1.0f;
	style.IndentSpacing		= 12.0f;
	style.ItemSpacing		= ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing	= ImVec2(4.0f, 4.0f);
	style.ScrollbarSize		= 10.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize		= 8.0f;
	style.GrabRounding		= 1.0f;
}

void ImGuiMaterialDarkStyle() {
	auto &	style  = ImGui::GetStyle();
	ImVec4 *colors = style.Colors;

	// Change ImGui colors
	colors[ImGuiCol_Text]				   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	colors[ImGuiCol_TextDisabled]		   = ImVec4(0.07f, 0.08f, 0.09f, 1.00f);
	colors[ImGuiCol_WindowBg]			   = ImVec4(0.21f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_ChildBg]			   = ImVec4(0.12f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_PopupBg]			   = ImVec4(0.12f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_Border]				   = ImVec4(0.21f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_BorderShadow]		   = ImVec4(0.21f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_FrameBg]			   = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]		   = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_FrameBgActive]		   = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TitleBg]			   = ImVec4(0.21f, 0.22f, 0.22f, 1.00f);
	colors[ImGuiCol_TitleBgActive]		   = ImVec4(0.02f, 0.66f, 0.46f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]	   = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]			   = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]		   = ImVec4(0.18f, 0.18f, 0.19f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab]		   = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]			   = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	colors[ImGuiCol_SliderGrab]			   = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]	   = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_Button]				   = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_ButtonHovered]		   = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
	colors[ImGuiCol_ButtonActive]		   = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_Header]				   = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
	colors[ImGuiCol_HeaderHovered]		   = ImVec4(0.12f, 0.20f, 0.28f, 0.55f);
	colors[ImGuiCol_HeaderActive]		   = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_Separator]			   = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_SeparatorHovered]	   = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_SeparatorActive]	   = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
	colors[ImGuiCol_ResizeGrip]			   = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]	   = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]	   = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab]				   = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabHovered]			   = ImVec4(0.12f, 0.20f, 0.28f, 0.80f);
	colors[ImGuiCol_TabActive]			   = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_TabUnfocused]		   = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive]	   = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
	colors[ImGuiCol_DockingPreview]		   = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg]		   = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
	colors[ImGuiCol_PlotLines]			   = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]	   = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]		   = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]		   = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]		   = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_NavHighlight]		   = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
}

void ImGuiOneDarkStyle() {
	ImGuiStyle &style = ImGui::GetStyle();

	// Modify ImGui style colors
	style.Colors[ImGuiCol_Text]					= COLOR_WHITE;
	style.Colors[ImGuiCol_TextDisabled]			= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_WindowBg]				= ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_ChildBg]				= ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	style.Colors[ImGuiCol_PopupBg]				= ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_Border]				= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow]			= ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg]				= ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered]		= ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive]		= ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_TitleBg]				= ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive]		= ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed]		= ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg]			= ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab]		= ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive]	= ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	style.Colors[ImGuiCol_CheckMark]			= COLOR_WHITE;
	style.Colors[ImGuiCol_SliderGrab]			= ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive]		= ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	style.Colors[ImGuiCol_Button]				= ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered]		= ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive]			= ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	style.Colors[ImGuiCol_Header]				= ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered]		= ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive]			= ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_Separator]			= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered]		= ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive]		= ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip]			= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered]	= ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripActive]		= ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	style.Colors[ImGuiCol_Tab]					= ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_TabHovered]			= ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_TabActive]			= ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused]			= ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive]	= ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	style.Colors[ImGuiCol_PlotLines]			= COLOR_WHITE;
	style.Colors[ImGuiCol_PlotLinesHovered]		= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram]		= COLOR_WHITE;
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_TextSelectedBg]		= ImVec4(0.4f, 0.4f, 0.4f, 1.0f);

	// Modify table colors
	style.Colors[ImGuiCol_TableHeaderBg]	 = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight]	 = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg]		 = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt]	 = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
}

void ImGuiVibrantLightStyle() {
	auto &	style  = ImGui::GetStyle();
	ImVec4 *colors = style.Colors;

	// Change ImGui colors
	colors[ImGuiCol_Text]				   = ImVec4(0.286f, 0.286f, 0.286f, 1.00f);
	colors[ImGuiCol_TextDisabled]		   = ImVec4(0.694f, 0.694f, 0.694f, 1.00f);
	colors[ImGuiCol_WindowBg]			   = ImVec4(0.980f, 0.980f, 0.980f, 1.00f);
	colors[ImGuiCol_ChildBg]			   = ImVec4(0.890f, 0.890f, 0.890f, 0.00f);
	colors[ImGuiCol_PopupBg]			   = ImVec4(1.000f, 1.000f, 1.000f, 0.98f);
	colors[ImGuiCol_Border]				   = ImVec4(0.875f, 0.875f, 0.875f, 0.25f);
	colors[ImGuiCol_BorderShadow]		   = ImVec4(0.000f, 0.000f, 0.000f, 0.00f);
	colors[ImGuiCol_FrameBg]			   = ImVec4(0.957f, 0.957f, 0.957f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]		   = ImVec4(0.957f, 0.957f, 0.957f, 1.00f);
	colors[ImGuiCol_FrameBgActive]		   = ImVec4(0.957f, 0.957f, 0.957f, 1.00f);
	colors[ImGuiCol_TitleBg]			   = ImVec4(0.957f, 0.957f, 0.957f, 1.00f);
	colors[ImGuiCol_TitleBgActive]		   = ImVec4(0.957f, 0.957f, 0.957f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]	   = ImVec4(0.957f, 0.957f, 0.957f, 0.51f);
	colors[ImGuiCol_MenuBarBg]			   = ImVec4(0.957f, 0.957f, 0.957f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]		   = ImVec4(0.957f, 0.957f, 0.957f, 1.00f);
	colors[ImGuiCol_ScrollbarGrab]		   = ImVec4(0.835f, 0.835f, 0.835f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.694f, 0.694f, 0.694f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.576f, 0.576f, 0.576f, 1.00f);
	colors[ImGuiCol_CheckMark]			   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_SliderGrab]			   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]	   = ImVec4(0.576f, 0.824f, 0.576f, 1.00f);
	colors[ImGuiCol_Button]				   = ImVec4(0.286f, 0.694f, 0.286f, 0.25f);
	colors[ImGuiCol_ButtonHovered]		   = ImVec4(0.286f, 0.694f, 0.286f, 0.67f);
	colors[ImGuiCol_ButtonActive]		   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_Header]				   = ImVec4(0.286f, 0.694f, 0.286f, 0.25f);
	colors[ImGuiCol_HeaderHovered]		   = ImVec4(0.286f, 0.694f, 0.286f, 0.67f);
	colors[ImGuiCol_HeaderActive]		   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_Separator]			   = ImVec4(0.875f, 0.875f, 0.875f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]	   = ImVec4(0.875f, 0.875f, 0.875f, 1.00f);
	colors[ImGuiCol_SeparatorActive]	   = ImVec4(0.875f, 0.875f, 0.875f, 1.00f);
	colors[ImGuiCol_ResizeGrip]			   = ImVec4(0.286f, 0.694f, 0.286f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered]	   = ImVec4(0.286f, 0.694f, 0.286f, 0.67f);
	colors[ImGuiCol_ResizeGripActive]	   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_Tab]				   = ImVec4(0.286f, 0.694f, 0.286f, 0.25f);
	colors[ImGuiCol_TabHovered]			   = ImVec4(0.286f, 0.694f, 0.286f, 0.67f);
	colors[ImGuiCol_TabActive]			   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_TabUnfocused]		   = ImVec4(0.286f, 0.694f, 0.286f, 0.25f);
	colors[ImGuiCol_TabUnfocusedActive]	   = ImVec4(0.286f, 0.694f, 0.286f, 0.67f);
	colors[ImGuiCol_DockingPreview]		   = ImVec4(0.286f, 0.694f, 0.286f, 0.70f);
	colors[ImGuiCol_DockingEmptyBg]		   = ImVec4(0.890f, 0.890f, 0.890f, 0.51f);
	colors[ImGuiCol_PlotLines]			   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]	   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_PlotHistogram]		   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]		   = ImVec4(0.286f, 0.694f, 0.286f, 0.35f);
	colors[ImGuiCol_DragDropTarget]		   = ImVec4(0.286f, 0.694f, 0.286f, 0.95f);
	colors[ImGuiCol_NavHighlight]		   = ImVec4(0.286f, 0.694f, 0.286f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.286f, 0.694f, 0.286f, 0.70f);

	// ImGui::GetIO().FontDefault = ImGui::GetIO().Fonts->AddFontFromFileTTF("path/to/your/font.ttf", 16.0f);
}

void ImGuiMaterialLightStyle() {
	ImGuiStyle &style  = ImGui::GetStyle();
	ImVec4 *	colors = style.Colors;

	// Change ImGui colors
	colors[ImGuiCol_Text]				   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_TextDisabled]		   = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	colors[ImGuiCol_WindowBg]			   = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
	colors[ImGuiCol_ChildBg]			   = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	colors[ImGuiCol_PopupBg]			   = ImVec4(1.0f, 1.0f, 1.0f, 0.98f);
	colors[ImGuiCol_Border]				   = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
	colors[ImGuiCol_BorderShadow]		   = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	colors[ImGuiCol_FrameBg]			   = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	colors[ImGuiCol_FrameBgHovered]		   = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	colors[ImGuiCol_FrameBgActive]		   = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	colors[ImGuiCol_TitleBg]			   = ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
	colors[ImGuiCol_TitleBgActive]		   = ImVec4(0.92f, 0.92f, 0.92f, 1.0f);
	colors[ImGuiCol_TitleBgCollapsed]	   = ImVec4(0.92f, 0.92f, 0.92f, 0.51f);
	colors[ImGuiCol_MenuBarBg]			   = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
	colors[ImGuiCol_ScrollbarBg]		   = ImVec4(0.98f, 0.98f, 0.98f, 1.0f);
	colors[ImGuiCol_ScrollbarGrab]		   = ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.45f, 0.45f, 0.45f, 1.0f);
	colors[ImGuiCol_CheckMark]			   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_SliderGrab]			   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_SliderGrabActive]	   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_Button]				   = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
	colors[ImGuiCol_ButtonHovered]		   = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	colors[ImGuiCol_ButtonActive]		   = ImVec4(0.64f, 0.64f, 0.64f, 1.0f);
	colors[ImGuiCol_Header]				   = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
	colors[ImGuiCol_HeaderHovered]		   = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	colors[ImGuiCol_HeaderActive]		   = ImVec4(0.64f, 0.64f, 0.64f, 1.0f);
	colors[ImGuiCol_Separator]			   = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
	colors[ImGuiCol_SeparatorHovered]	   = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	colors[ImGuiCol_SeparatorActive]	   = ImVec4(0.64f, 0.64f, 0.64f, 1.0f);
	colors[ImGuiCol_ResizeGrip]			   = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
	colors[ImGuiCol_ResizeGripHovered]	   = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	colors[ImGuiCol_ResizeGripActive]	   = ImVec4(0.64f, 0.64f, 0.64f, 1.0f);
	colors[ImGuiCol_Tab]				   = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
	colors[ImGuiCol_TabHovered]			   = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	colors[ImGuiCol_TabActive]			   = ImVec4(0.64f, 0.64f, 0.64f, 1.0f);
	colors[ImGuiCol_TabUnfocused]		   = ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
	colors[ImGuiCol_TabUnfocusedActive]	   = ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	colors[ImGuiCol_DockingPreview]		   = ImVec4(0.5f, 0.5f, 0.5f, 0.7f);
	colors[ImGuiCol_DockingEmptyBg]		   = ImVec4(0.92f, 0.92f, 0.92f, 0.51f);
	colors[ImGuiCol_PlotLines]			   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_PlotLinesHovered]	   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_PlotHistogram]		   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_TextSelectedBg]		   = ImVec4(0.86f, 0.86f, 0.86f, 0.35f);
	colors[ImGuiCol_DragDropTarget]		   = ImVec4(0.0f, 0.0f, 0.0f, 0.95f);
	colors[ImGuiCol_NavHighlight]		   = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.0f, 0.0f, 0.0f, 0.70f);
}

void ImGuiOneLightStyle() {
	ImGuiStyle &style = ImGui::GetStyle();

	// Modify ImGui style colors
	style.Colors[ImGuiCol_Text]					= COLOR_BLACK;
	style.Colors[ImGuiCol_TextDisabled]			= ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	style.Colors[ImGuiCol_WindowBg]				= ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
	style.Colors[ImGuiCol_ChildBg]				= ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
	style.Colors[ImGuiCol_PopupBg]				= ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
	style.Colors[ImGuiCol_Border]				= COLOR_GRAY;
	style.Colors[ImGuiCol_BorderShadow]			= ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg]				= ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered]		= ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive]		= ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
	style.Colors[ImGuiCol_TitleBg]				= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_TitleBgActive]		= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_TitleBgCollapsed]		= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_MenuBarBg]			= ImVec4(0.86f, 0.86f, 0.86f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg]			= ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab]		= COLOR_GRAY;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_ScrollbarGrabActive]	= COLOR_WHITE;
	style.Colors[ImGuiCol_CheckMark]			= COLOR_BLACK;
	style.Colors[ImGuiCol_SliderGrab]			= COLOR_GRAY;
	style.Colors[ImGuiCol_SliderGrabActive]		= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_Button]				= ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered]		= ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive]			= ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
	style.Colors[ImGuiCol_Header]				= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_HeaderHovered]		= ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive]			= ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
	style.Colors[ImGuiCol_Separator]			= COLOR_GRAY;
	style.Colors[ImGuiCol_SeparatorHovered]		= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_SeparatorActive]		= COLOR_WHITE;
	style.Colors[ImGuiCol_ResizeGrip]			= COLOR_GRAY;
	style.Colors[ImGuiCol_ResizeGripHovered]	= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_ResizeGripActive]		= COLOR_WHITE;
	style.Colors[ImGuiCol_Tab]					= ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	style.Colors[ImGuiCol_TabHovered]			= ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
	style.Colors[ImGuiCol_TabActive]			= ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused]			= ImVec4(0.78f, 0.78f, 0.78f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive]	= ImVec4(0.82f, 0.82f, 0.82f, 1.0f);
	style.Colors[ImGuiCol_PlotLines]			= COLOR_BLACK;
	style.Colors[ImGuiCol_PlotLinesHovered]		= COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_PlotHistogram]		= COLOR_BLACK;
	style.Colors[ImGuiCol_PlotHistogramHovered] = COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_TextSelectedBg]		= COLOR_LIGHT_GRAY;

	// Table Colors
	style.Colors[ImGuiCol_TableHeaderBg]	 = COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_TableBorderStrong] = COLOR_GRAY;
	style.Colors[ImGuiCol_TableBorderLight]	 = COLOR_LIGHT_GRAY;
	style.Colors[ImGuiCol_TableRowBg]		 = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);
	style.Colors[ImGuiCol_TableRowBgAlt]	 = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);

	// Modify ImGui style
	style.WindowPadding		= ImVec2(8, 8);
	style.WindowRounding	= 0.0f;
	style.FramePadding		= ImVec2(4, 4);
	style.FrameRounding		= 0.0f;
	style.ItemSpacing		= ImVec2(8, 4);
	style.ItemInnerSpacing	= ImVec2(4, 4);
	style.IndentSpacing		= 25.0f;
	style.ScrollbarSize		= 15.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabRounding		= 0.0f;
	style.TabRounding		= 0.0f;
}

void ImGuiArthurDarkStyle() {
	ImGuiStyle &style  = ImGui::GetStyle();
	ImVec4 *	colors = style.Colors;

	// Dark color scheme
	ImVec4 darkBlue	   = ImVec4(0.09f, 0.15f, 0.26f, 1.0f);
	ImVec4 darkGrey	   = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	ImVec4 lightGrey   = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
	ImVec4 accentColor = ImVec4(0.87f, 0.36f, 0.26f, 1.0f);

	// Adjust ImGui colors
	colors[ImGuiCol_Text]				   = ImVec4(0.9f, 0.9f, 0.9f, 1.0f);
	colors[ImGuiCol_TextDisabled]		   = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	colors[ImGuiCol_WindowBg]			   = darkBlue;
	colors[ImGuiCol_ChildBg]			   = darkBlue;
	colors[ImGuiCol_PopupBg]			   = darkBlue;
	colors[ImGuiCol_Border]				   = lightGrey;
	colors[ImGuiCol_BorderShadow]		   = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	colors[ImGuiCol_FrameBg]			   = darkGrey;
	colors[ImGuiCol_FrameBgHovered]		   = accentColor;
	colors[ImGuiCol_FrameBgActive]		   = accentColor;
	colors[ImGuiCol_TitleBg]			   = darkBlue;
	colors[ImGuiCol_TitleBgActive]		   = darkBlue;
	colors[ImGuiCol_TitleBgCollapsed]	   = darkBlue;
	colors[ImGuiCol_MenuBarBg]			   = darkBlue;
	colors[ImGuiCol_ScrollbarBg]		   = darkBlue;
	colors[ImGuiCol_ScrollbarGrab]		   = lightGrey;
	colors[ImGuiCol_ScrollbarGrabHovered]  = accentColor;
	colors[ImGuiCol_ScrollbarGrabActive]   = accentColor;
	colors[ImGuiCol_CheckMark]			   = accentColor;
	colors[ImGuiCol_SliderGrab]			   = accentColor;
	colors[ImGuiCol_SliderGrabActive]	   = accentColor;
	colors[ImGuiCol_Button]				   = darkGrey;
	colors[ImGuiCol_ButtonHovered]		   = accentColor;
	colors[ImGuiCol_ButtonActive]		   = accentColor;
	colors[ImGuiCol_Header]				   = darkGrey;
	colors[ImGuiCol_HeaderHovered]		   = accentColor;
	colors[ImGuiCol_HeaderActive]		   = accentColor;
	colors[ImGuiCol_Separator]			   = lightGrey;
	colors[ImGuiCol_SeparatorHovered]	   = accentColor;
	colors[ImGuiCol_SeparatorActive]	   = accentColor;
	colors[ImGuiCol_ResizeGrip]			   = lightGrey;
	colors[ImGuiCol_ResizeGripHovered]	   = accentColor;
	colors[ImGuiCol_ResizeGripActive]	   = accentColor;
	colors[ImGuiCol_Tab]				   = darkGrey;
	colors[ImGuiCol_TabHovered]			   = accentColor;
	colors[ImGuiCol_TabActive]			   = accentColor;
	colors[ImGuiCol_TabUnfocused]		   = darkGrey;
	colors[ImGuiCol_TabUnfocusedActive]	   = accentColor;
	colors[ImGuiCol_PlotLines]			   = accentColor;
	colors[ImGuiCol_PlotLinesHovered]	   = accentColor;
	colors[ImGuiCol_PlotHistogram]		   = accentColor;
	colors[ImGuiCol_PlotHistogramHovered]  = accentColor;
	colors[ImGuiCol_TextSelectedBg]		   = accentColor;
	colors[ImGuiCol_DragDropTarget]		   = accentColor;
	colors[ImGuiCol_NavHighlight]		   = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.7f);
	colors[ImGuiCol_NavWindowingHighlight] = accentColor;
}

void ImGuiArthurLightStyle() {
	ImGuiStyle &style  = ImGui::GetStyle();
	ImVec4 *	colors = style.Colors;

	// Light color scheme
	ImVec4 lightBlue   = ImVec4(0.88f, 0.94f, 1.0f, 1.0f);
	ImVec4 lightGrey   = ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
	ImVec4 darkGrey	   = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	ImVec4 accentColor = ImVec4(0.87f, 0.36f, 0.26f, 1.0f);

	// Adjust ImGui colors
	colors[ImGuiCol_Text]				   = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
	colors[ImGuiCol_TextDisabled]		   = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	colors[ImGuiCol_WindowBg]			   = lightBlue;
	colors[ImGuiCol_ChildBg]			   = lightBlue;
	colors[ImGuiCol_PopupBg]			   = lightBlue;
	colors[ImGuiCol_Border]				   = darkGrey;
	colors[ImGuiCol_BorderShadow]		   = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	colors[ImGuiCol_FrameBg]			   = lightGrey;
	colors[ImGuiCol_FrameBgHovered]		   = accentColor;
	colors[ImGuiCol_FrameBgActive]		   = accentColor;
	colors[ImGuiCol_TitleBg]			   = lightBlue;
	colors[ImGuiCol_TitleBgActive]		   = lightBlue;
	colors[ImGuiCol_TitleBgCollapsed]	   = lightBlue;
	colors[ImGuiCol_MenuBarBg]			   = lightBlue;
	colors[ImGuiCol_ScrollbarBg]		   = lightBlue;
	colors[ImGuiCol_ScrollbarGrab]		   = darkGrey;
	colors[ImGuiCol_ScrollbarGrabHovered]  = accentColor;
	colors[ImGuiCol_ScrollbarGrabActive]   = accentColor;
	colors[ImGuiCol_CheckMark]			   = accentColor;
	colors[ImGuiCol_SliderGrab]			   = accentColor;
	colors[ImGuiCol_SliderGrabActive]	   = accentColor;
	colors[ImGuiCol_Button]				   = lightGrey;
	colors[ImGuiCol_ButtonHovered]		   = accentColor;
	colors[ImGuiCol_ButtonActive]		   = accentColor;
	colors[ImGuiCol_Header]				   = lightGrey;
	colors[ImGuiCol_HeaderHovered]		   = accentColor;
	colors[ImGuiCol_HeaderActive]		   = accentColor;
	colors[ImGuiCol_Separator]			   = darkGrey;
	colors[ImGuiCol_SeparatorHovered]	   = accentColor;
	colors[ImGuiCol_SeparatorActive]	   = accentColor;
	colors[ImGuiCol_ResizeGrip]			   = darkGrey;
	colors[ImGuiCol_ResizeGripHovered]	   = accentColor;
	colors[ImGuiCol_ResizeGripActive]	   = accentColor;
	colors[ImGuiCol_Tab]				   = lightGrey;
	colors[ImGuiCol_TabHovered]			   = accentColor;
	colors[ImGuiCol_TabActive]			   = accentColor;
	colors[ImGuiCol_TabUnfocused]		   = lightGrey;
	colors[ImGuiCol_TabUnfocusedActive]	   = accentColor;
	colors[ImGuiCol_PlotLines]			   = accentColor;
	colors[ImGuiCol_PlotLinesHovered]	   = accentColor;
	colors[ImGuiCol_PlotHistogram]		   = accentColor;
	colors[ImGuiCol_PlotHistogramHovered]  = accentColor;
	colors[ImGuiCol_TextSelectedBg]		   = accentColor;
	colors[ImGuiCol_DragDropTarget]		   = accentColor;
	colors[ImGuiCol_NavHighlight]		   = ImVec4(accentColor.x, accentColor.y, accentColor.z, 0.7f);
	colors[ImGuiCol_NavWindowingHighlight] = accentColor;
}
}  // namespace Themes
