#include "precompiled.h"
#include "TerminalPanel.h"
#include <Eklipse/Core/Application.h>

namespace Eklipse
{
    static int TerminalCommandInputCallback(ImGuiInputTextCallbackData* data)
	{
		if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory)
		{
			if (data->EventKey == ImGuiKey_UpArrow)
			{
                Application::Get().GetTerminal()->HistoryUp();
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, Application::Get().GetTerminal()->GetHistoryBuffer().c_str());
			}
			else if (data->EventKey == ImGuiKey_DownArrow)
			{
                Application::Get().GetTerminal()->HistoryDown();
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, Application::Get().GetTerminal()->GetHistoryBuffer().c_str());
			}
		}
        return 0;
	}

    TerminalPanel::TerminalPanel()
    {
        m_terminal = CreateUnique<Terminal>();
    }
    bool TerminalPanel::OnGUI(float deltaTime)
    {
        ImGui::Begin("Terminal");
        if (!m_terminal->GetSink())
        {
            ImGui::TextUnformatted("No sink attached");
            ImGui::End();
            return false;
        }

        if (ImGui::Button("Clear"))
        {
            m_terminal->Clear();
        }

        ImGui::SameLine();

        static bool autoScroll = true;
        ImGui::Checkbox("Auto Scroll", &autoScroll);

        ImGui::SameLine();

        if (ImGui::BeginCombo("Level", m_terminal->GetLevelString().c_str()))
		{
            static std::vector<spdlog::level::level_enum> levels = { spdlog::level::trace, spdlog::level::debug, spdlog::level::info, spdlog::level::warn, spdlog::level::err, spdlog::level::critical, spdlog::level::off };
            static std::vector<const char*> levelStrings = { "Trace", "Debug", "Info", "Warn", "Error", "Critical", "Off"};
			for (int i = 0; i < levels.size(); i++)
			{
				if (ImGui::Selectable(levelStrings[i], m_terminal->GetLevel() == levels[i]))
				{
                    m_terminal->SetLevel(levels[i], levelStrings[i]);
				}
			}
			ImGui::EndCombo();
		}

        const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + 25;
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
		    for (auto& message : m_terminal->GetBuffer())
            {
                switch (message.msg.level)
                {
                    case spdlog::level::trace:
					    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
					    break;
                    case spdlog::level::debug:
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                        break;
                    case spdlog::level::info:
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                        break;
                    case spdlog::level::warn:
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
                        break;
                    case spdlog::level::err:
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                        break;
                    case spdlog::level::critical:
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                        break;
                }
            
        	    ImGui::TextUnformatted(message.str.c_str());

                ImGui::PopStyleColor();
            }

            if (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }


        ImGui::EndChild();

        static bool setFocusOnTerminal = false;
        if (setFocusOnTerminal)
		{
			ImGui::SetKeyboardFocusHere();
			setFocusOnTerminal = false;
		}

        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##CommandInput", &m_terminal->GetHistoryBuffer(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, TerminalCommandInputCallback))
        {
            if (!m_terminal->GetHistoryBuffer().empty())
            {
                m_terminal->ExcecuteCommand(m_terminal->GetHistoryBuffer());
                m_terminal->GetHistoryBuffer().clear();
                setFocusOnTerminal = true;
            }
        }

		ImGui::End();
		return true;
    }
}