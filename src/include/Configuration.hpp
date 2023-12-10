#ifndef ARTHUR_INCLUDE_CONFIGURATION_HPP
#define ARTHUR_INCLUDE_CONFIGURATION_HPP
#pragma once

#include "Colors.hpp"

constexpr ImGuiTableFlags GetTableFlags() {
    ImGuiTableFlags Flags = ImGuiTableFlags_RowBg;
    Flags |= ImGuiTableFlags_Resizable;
    Flags |= ImGuiTableFlags_ScrollX;
    Flags |= ImGuiTableFlags_ScrollY;
    Flags |= ImGuiTableFlags_Borders;
    Flags |= ImGuiTableFlags_Reorderable;
    //	Flags |= ImGuiTableFlags_NoHostExtendX;
    //	Flags |= ImGuiTableFlags_NoHostExtendY;

    return Flags;
}

constexpr ImGuiSelectableFlags GetSelectableFlags() {
    ImGuiSelectableFlags Flags = ImGuiSelectableFlags_SpanAllColumns;
    Flags |= ImGuiSelectableFlags_AllowItemOverlap;
    return Flags;
}

constexpr ImGuiTabBarFlags GetStrategyWorkspaceTabFlags() {
    ImGuiTabBarFlags Flags = ImGuiTabBarFlags_Reorderable;
    Flags |= ImGuiTabBarFlags_AutoSelectNewTabs;
    Flags |= ImGuiTabBarFlags_NoCloseWithMiddleMouseButton;
    Flags |= ImGuiTabBarFlags_TabListPopupButton;
    Flags |= ImGuiTabBarFlags_FittingPolicyScroll;
    return Flags;
}

constexpr ImGuiWindowFlags GetMenuBarFlags() {
    ImGuiWindowFlags Flags = ImGuiWindowFlags_NoScrollbar;
    Flags |= ImGuiWindowFlags_NoSavedSettings;
    Flags |= ImGuiWindowFlags_MenuBar;
    return Flags;
}

constexpr ImGuiTabItemFlags GetPortfolioTabFlags() {
    return ImGuiTabItemFlags_NoCloseWithMiddleMouseButton;
}

constexpr ImGuiWindowFlags GetOverlayFlags() {
    ImGuiWindowFlags Flags = ImGuiWindowFlags_NoDecoration;
    Flags |= ImGuiWindowFlags_NoDocking;
    Flags |= ImGuiWindowFlags_AlwaysAutoResize;
    Flags |= ImGuiWindowFlags_NoSavedSettings;
    Flags |= ImGuiWindowFlags_NoFocusOnAppearing;
    Flags |= ImGuiWindowFlags_NoNav;
    return Flags;
}

inline constexpr ImGuiWindowFlags      OverlayFlags              = GetOverlayFlags();
inline constexpr ImGuiSelectableFlags  TableSelectableFlags      = GetSelectableFlags();
inline constexpr ImGuiTableFlags       TableFlags                = GetTableFlags();
inline constexpr ImGuiTabBarFlags      StrategyWorkspaceTabFlags = GetStrategyWorkspaceTabFlags();
inline constexpr ImGuiTabItemFlags     PortfolioTabFlags         = GetPortfolioTabFlags();
inline constexpr ImGuiWindowFlags      MenuBarFlags              = GetMenuBarFlags();
inline constexpr ImGuiWindowFlags      MainWindowFlags           = ImGuiWindowFlags_ChildWindow;
inline constexpr ImGuiTableColumnFlags TableColumnFlags          = ImGuiTableColumnFlags_None;

template<typename Type>
void NextCell(int index_, const char* format_, Type type_, ImVec4 color_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::TextColored(color_, format_, type_);
    }
}

template<typename Type>
void FirstCellWithPadding(int index_, const char* format_, Type type_, ImVec4 color_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::AlignTextToFramePadding();
        ImGui::TextColored(color_, format_, type_);
    }
}

template<typename Type>
void NextCell(int index_, const char* format_, Type type_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::Text(format_, type_);
    }
}

template<typename Type>
void FirstCellWithPadding(int index_, const char* format_, Type type_) {
    if (ImGui::TableSetColumnIndex(index_)) {
        ImGui::AlignTextToFramePadding();
        ImGui::Text(format_, type_);
    }
}

template<typename Value>
bool FirstCell(int index_, const char* data_, Value& first_, Value second_) {
    ImGui::TableSetColumnIndex(index_);
    ImGui::AlignTextToFramePadding();
    bool ret = ImGui::Selectable(data_, first_ == second_, TableSelectableFlags);
    if (ret) {
        first_ = second_;
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImGui::GetColorU32(SELECTED_ROW_COLOR));
    }
    return ret;
}

#endif// ARTHUR_INCLUDE_CONFIGURATION_HPP
