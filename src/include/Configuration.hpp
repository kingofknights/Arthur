// #pragma once

#include "Colors.hpp"
#include "Logger.hpp"
#include "imgui.h"
#include "include/Structure.hpp"

constexpr auto GetTableFlags() -> ImGuiTableFlags {
    ImGuiTableFlags flags = ImGuiTableFlags_RowBg;
    flags |= ImGuiTableFlags_Resizable;
    flags |= ImGuiTableFlags_ScrollX;
    flags |= ImGuiTableFlags_ScrollY;
    flags |= ImGuiTableFlags_Borders;
    flags |= ImGuiTableFlags_Reorderable;
    //	Flags |= ImGuiTableFlags_NoHostExtendX;
    //	Flags |= ImGuiTableFlags_NoHostExtendY;

    return flags;
}

constexpr auto GetSelectableFlags() -> ImGuiSelectableFlags {
    ImGuiSelectableFlags flags = ImGuiSelectableFlags_SpanAllColumns;
    flags |= ImGuiSelectableFlags_AllowItemOverlap;
    return flags;
}

constexpr auto GetStrategyWorkspaceTabFlags() -> ImGuiTabBarFlags {
    ImGuiTabBarFlags flags = ImGuiTabBarFlags_Reorderable;
    flags |= ImGuiTabBarFlags_AutoSelectNewTabs;
    flags |= ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
    flags |= ImGuiTabBarFlags_TabListPopupButton;
    flags |= ImGuiTabBarFlags_FittingPolicyScroll;
    return flags;
}

constexpr auto GetMenuBarFlags() -> ImGuiWindowFlags {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar;
    flags |= ImGuiWindowFlags_NoSavedSettings;
    flags |= ImGuiWindowFlags_MenuBar;
    return flags;
}

constexpr auto GetPortfolioTabFlags() -> ImGuiTabItemFlags {
    return ImGuiTabItemFlags_NoCloseWithMiddleMouseButton;
}

constexpr auto GetOverlayFlags() -> ImGuiWindowFlags {
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration;
    flags |= ImGuiWindowFlags_NoDocking;
    flags |= ImGuiWindowFlags_AlwaysAutoResize;
    flags |= ImGuiWindowFlags_NoSavedSettings;
    flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    flags |= ImGuiWindowFlags_NoNav;
    return flags;
}

inline constexpr ImGuiWindowFlags      OverlayFlags              = GetOverlayFlags();
inline constexpr ImGuiSelectableFlags  TableSelectableFlags      = GetSelectableFlags();
inline constexpr ImGuiTableFlags       TableFlags                = GetTableFlags();
inline constexpr ImGuiTabBarFlags      StrategyWorkspaceTabFlags = GetStrategyWorkspaceTabFlags();
inline constexpr ImGuiTabItemFlags     PortfolioTabFlags         = GetPortfolioTabFlags();
inline constexpr ImGuiWindowFlags      MenuBarFlags              = GetMenuBarFlags();
inline constexpr ImGuiWindowFlags      MainWindowFlags           = ImGuiWindowFlags_ChildWindow;
inline constexpr ImGuiTableColumnFlags TableColumnFlags          = ImGuiTableColumnFlags_None;

template <typename Type>
void NextCell(int index_, Type type_, ImVec4 color_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::PushStyleColor(ImGuiCol_Text, color_);
        const std::string data = FORMAT("{}", type_);
        ImGui::TextUnformatted(data.data(), data.data() + data.size());
        ImGui::PopStyleColor();
    }
}

template <>
inline void NextCell(int index_, PriceT type_, ImVec4 color_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::PushStyleColor(ImGuiCol_Text, color_);
        const std::string data = FORMAT("{:.2f}", type_);
        ImGui::TextUnformatted(data.data(), data.data() + data.size());
        ImGui::PopStyleColor();
    }
}

template <typename Type>
void FirstCellWithPadding(int index_, Type type_, ImVec4 color_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::AlignTextToFramePadding();
        ImGui::PushStyleColor(ImGuiCol_Text, color_);
        const std::string data = FORMAT("{}", type_);
        ImGui::TextUnformatted(data.data(), data.data() + data.size());
        ImGui::PopStyleColor();
    }
}

template <typename Type>
void NextCell(int index_, Type type_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        const std::string data = FORMAT("{}", type_);
        ImGui::TextUnformatted(data.data(), data.data() + data.size());
    }
}
template <>
inline void NextCell(int index_, PriceT type_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        const std::string data = FORMAT("{:.2f}", type_);
        ImGui::TextUnformatted(data.data(), data.data() + data.size());
    }
}

template <typename Type>
void FirstCellWithPadding(int index_, Type type_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::AlignTextToFramePadding();
        const std::string data = FORMAT("{}", type_);
        ImGui::TextUnformatted(data.data(), data.data() + data.size());
    }
}

template <typename Value>
auto FirstCell(int index_, const char* data_, Value& first_, Value second_) -> bool {
    ImGui::TableSetColumnIndex(index_);
    ImGui::AlignTextToFramePadding();
    bool ret = ImGui::Selectable(data_, first_ == second_, TableSelectableFlags);
    if (ret) {
        first_ = second_;
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(SELECTED_ROW_COLOR));
    }
    return ret;
}
