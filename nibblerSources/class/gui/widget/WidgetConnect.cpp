#include <gui/Gui.hpp>
#include "WidgetConnect.hpp"
#include "cores/Univers.hpp"
#include <network/SnakeServer.hpp>

WidgetConnect::WidgetConnect(Gui &core) :
		AWidget(core, "Connect", NIBBLER_IMGUI_WINDOW_FLAGS_BASIC),
		client_(false) {
	bzero(dnsBuffer_, IM_ARRAYSIZE(dnsBuffer_));
	bzero(portBuffer_, IM_ARRAYSIZE(portBuffer_));
	memcpy(dnsBuffer_, DEFAULT_DNS, sizeof(DEFAULT_DNS));
	memcpy(portBuffer_, DEFAULT_PORT, sizeof(DEFAULT_PORT));

}

void WidgetConnect::beginContent_() {
	SnakeClient::boost_shared_ptr ptrClient(core_.univers.getSnakeClient().lock());

	if (client_)
		Gui::beginColor(Gui::eColor::kRed);
	else
		Gui::beginColor(Gui::eColor::kGreen);
	if (ImGui::Button("Host", sf::Vector2f(ImGui::GetWindowSize().x / 2, 20)))
		client_ = false;
	ImGui::SameLine();
	Gui::endColor();

	if (client_)
		Gui::beginColor(Gui::eColor::kGreen);
	else
		Gui::beginColor(Gui::eColor::kRed);
	if (ImGui::Button("Client", sf::Vector2f(ImGui::GetWindowSize().x / 2 - 1, 20)))
		client_ = true;

	Gui::endColor();

	if (!(core_.univers.isServer() && client_)) {
		if (ImGui::InputText("DNS", dnsBuffer_,
							 IM_ARRAYSIZE(dnsBuffer_),
							 ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (!strcmp(dnsBuffer_, ""))
				strcpy(dnsBuffer_, "localhost");
		}

		if (ImGui::InputText("PORT", portBuffer_,
							 IM_ARRAYSIZE(portBuffer_),
							 ImGuiInputTextFlags_EnterReturnsTrue)) {
			if (!strcmp(portBuffer_, ""))
				strcpy(portBuffer_, "8000");
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();

	Gui::beginColor(Gui::eColor::kGreen);

	if (client_) {
		if (ptrClient && ptrClient->isOpen()) {
			if (ImGui::Button("Join new", sf::Vector2f(ImGui::GetWindowSize().x, 20))) {
				ptrClient->disconnect();
				core_.univers.connect(dnsBuffer_, portBuffer_);
			}
		} else if (ImGui::Button("Join", sf::Vector2f(ImGui::GetWindowSize().x, 20))) {
			if (!ptrClient)
				core_.univers.createClient();
			if (core_.univers.isServer())
				core_.univers.connect(core_.univers.getSnakeServer().getAddress_(), std::to_string(core_.univers.getSnakeServer().getPort_()));
			else
				core_.univers.connect(dnsBuffer_, portBuffer_);
		}
	}
	else if (!client_) {
		if (ImGui::Button("Create Server", sf::Vector2f(ImGui::GetWindowSize().x, 20))) {
			if (core_.univers.isServer())
				core_.univers.deleteServer();
			core_.univers.createServer(dnsBuffer_, std::stoul(portBuffer_));
		}
	}
	Gui::endColor();

}

