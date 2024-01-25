#pragma once
#include <spdlog/sinks/base_sink.h>

namespace Eklipse
{
	struct Message
	{
		spdlog::details::log_msg msg;
		String str;
	};

	template<typename Mutex>
	class EK_API TerminalSink : public spdlog::sinks::base_sink<Mutex>
	{
		friend class Terminal;
	public:
		TerminalSink() = default;

	protected:
		void sink_it_(const spdlog::details::log_msg& msg) override
		{
			m_buffer.push_back({ msg, fmt::to_string(msg.payload) });

			while (m_buffer.size() > m_max_messages)
			{
				m_buffer.pop_front();
			}
		}

		void flush_() override {}

	private:
		std::deque<Message> m_buffer;
		size_t m_max_messages = 100;
	};

	struct ParsedCommand
	{
		String Command;
		std::unordered_map<String, String> Args;
		std::vector<String> Flags;
	};
	struct TerminalCommand
	{
		const char* Command;
		const char* Description;
		std::function<void(const ParsedCommand&)> Callback;
	};

	class EK_API Terminal
	{
	public:
		Terminal();
		void AddCommand(const TerminalCommand& command);

		void ExcecuteCommand(const String& command);
		void Clear();

		Ref<TerminalSink<std::mutex>> GetSink() { return m_sink; }
		std::deque<Message>& GetBuffer() { return m_sink->m_buffer; }
		spdlog::level::level_enum GetLevel() const { return m_level; }
		void SetLevel(spdlog::level::level_enum level, const char* name) { Clear(); m_level = level; m_levelString = name; spdlog::set_level(level); }
		
		String& GetLevelString() { return m_levelString; }
		String& GetHistoryBuffer() { return m_historyBuffer; }

		void HistoryUp();
		void HistoryDown();

	private:
		std::vector<TerminalCommand> m_commands;
		Ref<TerminalSink<std::mutex>> m_sink;
		spdlog::level::level_enum m_level;
		String m_levelString;
		std::vector<String> m_history;
		int32_t m_historyIndex;
		int32_t m_historySizeLimit = 10;
		String m_historyBuffer;
	};
}