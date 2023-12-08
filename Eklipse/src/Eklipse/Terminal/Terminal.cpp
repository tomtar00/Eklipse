#include "precompiled.h"
#include "Terminal.h"

namespace Eklipse
{
	static ParsedCommand ParseCommand(const std::string& command)
	{
		std::istringstream iss(command);
		std::string token;

		std::string mainCommand;
		std::unordered_map<std::string, std::string> args;
		std::vector<std::string> flags;

		iss >> mainCommand;

		while (iss >> token) 
		{
			if (token[0] == '-') 
			{
				size_t pos = token.find('=');
				if (pos != std::string::npos) 
				{
					std::string argName = token.substr(1, pos - 1);
					std::string argValue = token.substr(pos + 1);
					args[argName] = argValue;
				}
				else 
				{
					flags.push_back(token.substr(1));
				}
			}
		}

		return { mainCommand, args, flags };
	}

	Terminal::Terminal()
	{
		m_sink = CreateRef<TerminalSink<std::mutex>>();
		m_historyIndex = 0;
		SetLevel(spdlog::level::debug, "Debug");

		AddCommand({
			"help", "Display all available commands",
			[this](const ParsedCommand& cmd) 
				{ 
					std::stringstream ss;
					ss << "Available commands:\n";
					for (auto& command : m_commands)
					{
						ss << command.Command << " - " << command.Description << '\n';
					}
					EK_CORE_INFO(ss.str());
				}
			});
		AddCommand({ 
			"clear", "Clear the terminal window", 
			[this](const ParsedCommand& cmd) { Clear(); } 
		});
	}
	void Terminal::AddCommand(const TerminalCommand& command)
	{
		m_commands.push_back(command);
	}
	void Terminal::ExcecuteCommand(const std::string& command)
	{
		ParsedCommand parsedCommand = ParseCommand(command);

		m_history.push_back(command);
		m_historyIndex = m_history.size();
		if (m_history.size() > m_historySizeLimit)
			m_history.erase(m_history.begin());

		try
		{
			for (auto& cmd : m_commands)
			{
				if (cmd.Command == parsedCommand.Command)
				{
					cmd.Callback(parsedCommand);
					return;
				}
			}

			EK_CORE_WARN("Command '{0}' not found. Enter 'help' to get list of all available commands.", parsedCommand.Command);
		}
		catch (const std::exception& e)
		{
			EK_CORE_ERROR("Error while executing command: {0}", e.what());
			return;
		}
	}
	void Terminal::Clear()
	{
		m_sink->m_buffer.clear();
	}
	void Terminal::HistoryUp()
	{
		if (m_history.size() == 0)
			return;

		m_historyIndex--;
		if (m_historyIndex < 0)
			m_historyIndex = 0;

		m_historyBuffer = m_history[m_historyIndex];
	}
	void Terminal::HistoryDown()
	{
		if (m_history.size() == 0)
			return;

		m_historyIndex++;
		if (m_historyIndex >= m_history.size())
			m_historyIndex = m_history.size() - 1;

		m_historyBuffer = m_history[m_historyIndex];
	}
}