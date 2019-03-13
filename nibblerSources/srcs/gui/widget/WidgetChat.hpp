#pragma once

#include "AWidget.hpp"
#include "nibbler.hpp"

class WidgetChat : public AWidget {
public:

	struct colorLog {
		eColorLog color;
		std::string log;

		colorLog(eColorLog acolor, std::string const &str) :
				color(acolor), log(str) {};
	};

	WidgetChat(Gui &);
	~WidgetChat() override = default;
	WidgetChat() = delete;
	WidgetChat &operator=(const WidgetChat &) = default;
	WidgetChat(const WidgetChat &) = default;

	void addLog(eColorLog color, char const *format, ...);

private:
	ImGuiTextBuffer bufferChat_;
	std::vector<colorLog> log_;
	bool scrollChat_;
	char bufferMessage_[CHAT_BUFFER];


	void beginContent_() override;
	bool chatCommand_();
	bool checkClientIsConnect_();
	bool checkClient_();
	void chatText_();
	void sendMessage_();
};