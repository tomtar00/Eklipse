#include "precompiled.h"
#include "Terminal.h"

namespace Eklipse
{
	static ParsedCommand ParseCommand(const String& command)
	{
		std::istringstream iss(command);
		String token;

		String mainCommand;
		std::unordered_map<String, String> args;
		Vec<String> flags;

		iss >> mainCommand;

		while (iss >> token) 
		{
			if (token[0] == '-') 
			{
				size_t pos = token.find('=');
				if (pos != String::npos) 
				{
					String argName = token.substr(1, pos - 1);
					String argValue = token.substr(pos + 1);
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

#ifdef EK_DEBUG
		SetLevel(spdlog::level::trace, "Trace");
#else
        SetLevel(spdlog::level::info, "Info");
#endif

		AddCommand({
			"help", "Display all available commands",
			"", {},
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
			"", {},
			[this](const ParsedCommand& cmd) { Clear(); } 
		});
	}

	void Terminal::AddCommand(const TerminalCommand& command)
	{
		m_commands.push_back(command);
	}
	void Terminal::ExcecuteCommand(const String& command)
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
					// check if the command has required arguments
					for (auto& arg : cmd.RequiredArgs)
                    {
                        if (parsedCommand.Args.find(arg) == parsedCommand.Args.end())
                        {
                            EK_CORE_ERROR("Command '{0}' requires argument '{1}'.\nUsage:\n{2}", parsedCommand.Command, arg, cmd.Usage);
                            return;
                        }
                    }
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

	String& Terminal::GetLevelString()
	{
		return m_levelString;
	}
	String& Terminal::GetHistoryBuffer()
	{
		return m_historyBuffer;
	}
	Ref<TerminalSink<std::mutex>> Terminal::GetSink()
	{
		return m_sink;
	}
	std::deque<Message>& Terminal::GetBuffer()
	{
		return m_sink->m_buffer;
	}
	spdlog::level::level_enum Terminal::GetLevel() const
	{
		return m_level;
	}

	void Terminal::SetLevel(spdlog::level::level_enum level, const char* name)
	{
		Clear(); 
		m_level = level; m_levelString = name; 
		spdlog::set_level(level);
	}
}