#include <cores/Univers.hpp>
#include "Gui.hpp"
#include "widget/WidgetExit.hpp"
#include "widget/WidgetSnake.hpp"
#include "widget/WidgetLobby.hpp"
#include "gui/widget/WidgetOption.hpp"
#include "gui/widget/WidgetSettingGame.hpp"
#include "gui/widget/WidgetConnect.hpp"
#include "gui/widget/WidgetServerPannel.hpp"
#include "gui/widget/WidgetEtat.hpp"
#include <network/SnakeServer.hpp>

Gui::Gui(Univers &univers) :
univers(univers),
pathRessources_(boost::filesystem::path(NIBBLER_ROOT_PROJECT_PATH) / "ressources/"),
winSize_(sf::Vector2<unsigned int>(1000, 900)),
win_(sf::VideoMode(winSize_.x, winSize_.y), "Project Snake"),
io_(createContext_()),
chat_(*this) {
	addMessageChat(eColorLog::kYellow, "/help to display help usage.");
	io_.IniFilename = NULL;
}

ImGuiIO			&Gui::createContext_(void) {
	win_.setFramerateLimit(60);
	ImGui::SFML::Init(win_);

	// Give Focus on Widow
	sf::Event event;
	event.type = sf::Event::GainedFocus;
	ImGui::SFML::ProcessEvent(event);

	return (ImGui::GetIO());
}

Gui::~Gui() {
	ImGui::SFML::Shutdown();
}

boost::filesystem::path const	&Gui::getPathRessources() const {
	return (pathRessources_);
}

void			callbackExit(Gui &gui) {
	gui.exit();
}

void			callbackTest(void *vUnivers, std::string const &input) {
	Univers *univers = reinterpret_cast<Univers*>(vUnivers);

	if (!vUnivers)
		return ;
	else if (input == "A")
		univers->createBobby();
	else if (input == "B")
		univers->switchBorderless();
	else if (input == "C")
		univers->createClient();
	else if (input == "D")
		univers->deleteClient();
	else if (input == "E")
		univers->connect(univers->getIPAddress(), univers->isServer() ? std::to_string(univers->getSnakeServer().getPort_()) : DEFAULT_PORT);
	else if (input == "R")
		univers->switchReady();
	else if (input == "S")
		univers->createServer(univers->getIPAddress(), DEFAULT_PORT_NU);
	else if (input == "W")
		univers->deleteServer();
	else if (input == "X")
		univers->sendOpenGameToServer();
	else if (input == "H")
		univers->sendHostname();
}

void			Gui::updateEvent_() {
	sf::Event event;

	while (win_.pollEvent(event)) {
		processEvent_(event);
		ImGui::SFML::ProcessEvent(event);
	}
}

void					Gui::processEvent_(sf::Event const &event) {
	if (event.type == sf::Event::KeyPressed && event.key.control) {
		switch (event.key.code) {
			case sf::Keyboard::A:
				univers.createBobby();
				break;
			case sf::Keyboard::B:
				univers.switchBorderless();
				break;
			case sf::Keyboard::C:
				univers.createClient();
				break;
			case sf::Keyboard::D:
				univers.deleteClient();
				break;
			case sf::Keyboard::E:
				univers.connect(univers.getIPAddress(), univers.isServer() ? std::to_string(univers.getSnakeServer().getPort_()) : DEFAULT_PORT);
				break;
			case sf::Keyboard::R:
				univers.switchReady();
				break;
			case sf::Keyboard::S:
				univers.createServer(univers.getIPAddress(), DEFAULT_PORT_NU);
				break;
			case sf::Keyboard::W:
				univers.deleteServer();
				break;
			case sf::Keyboard::X:
				univers.sendOpenGameToServer();
				break;
			case sf::Keyboard::H:
				univers.sendHostname();
				break;
			default:
				break;
		}
	}

	if (event.type == sf::Event::Resized)
		winSize_ = sf::Vector2<unsigned int>(event.size.width, event.size.height);
	else if (event.type == sf::Event::Closed)
		exit();
}

void			Gui::mainLoop()
{
	WidgetExit wexit(*this, callbackExit);
	WidgetLobby lobby(*this);
	WidgetServerPannel serverPannel(*this);
	WidgetConnect wOptionConnect(*this);
	WidgetEtat wEtat(*this);
	WidgetSettingGame settings(*this);
	WidgetOption optionSnake(*this);

	while ((win_.isOpen() && !univers.isOpenGame_())) {

		ImGui::SFML::Update(win_, deltaClock_.restart());

		updateEvent_();

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(0, 50)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(50, 50)));
		chat_.render(true);

		lobby.render(true);

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(95, 0)), 0, sf::Vector2f(0.5f, 0.5f));
		wexit.render(true);

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(50, 50)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(20, 15)));
		settings.render(true);

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(70, 50)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(30, 15)));

		optionSnake.render(!univers.getSnakeClient().expired());

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(70, 65)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(30, 20)));
		wOptionConnect.render(true);
		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(50, 85)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(50, 15)));
		wEtat.render(true);

		ImGui::SetNextWindowPos(positionByPercent(sf::Vector2<unsigned int>(50, 65)));
		ImGui::SetNextWindowSize(positionByPercent(sf::Vector2<unsigned int>(20, 20)));
		serverPannel.render(univers.isServer());

		render_();
	}
}

void				Gui::render_(void) {
	win_.clear();
	ImGui::SFML::Render(win_);
	win_.display();
}

void 				Gui::exit(void) {
	win_.close();
	univers.setExit(true);
}

sf::Vector2<unsigned int>	Gui::positionByPercent(sf::Vector2<unsigned int> const &percent) const {
	return (sf::Vector2<unsigned int>(winSize_.x * percent.x / 100,
										winSize_.y * percent.y / 100));
}

void					Gui::beginHueColor_(float color, float power) {
	ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(color, 0.7f * power, 0.7f * power)));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(color, 0.8f * power, 0.8f * power)));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(color, 0.9f * power, 0.9f * power)));
}
void					Gui::beginColor(eColor color, eColor power) {
	assert(!Gui::useColor_);
	Gui::useColor_ = true;
	if (color == eColor::kGrey) {
		ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0.f, 0.f, 0.5f * Gui::mapEColor_.at(power))));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0.f, 0.0f, 0.5f * Gui::mapEColor_.at(power))));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0.f, 0.00f, 0.65f * Gui::mapEColor_.at(power))));
	} else
		beginHueColor_(Gui::mapEColor_.at(color), Gui::mapEColor_.at(power));
}
void					Gui::endColor() {
	ImGui::PopStyleColor(3);
	Gui::useColor_ = false;
}

void					Gui::addMessageChat(eColorLog color, std::string const &log) {
	chat_.addLog(color, log);
}
void					Gui::addMessageChat(std::string const &log) {
	chat_.addLog(eColorLog::kNone, log);
}

std::map< eColor, float > const Gui::mapEColor_ = { { eColor::kGrey, 0.f },
														 { eColor::kRed, 0.f },
														 { eColor::kGreen, 0.33f },
														 { eColor::kPurple, 0.77f },
														 { eColor::kHard, 1.f },
														 { eColor::kMedium, .75f },
														 { eColor::kSoft, .5f },
														 { eColor::kLight, .33f }};

bool 						Gui::useColor_ = false;

Gui::CoreConstructorException::~CoreConstructorException(void) noexcept {}
Gui::CoreConstructorException::CoreConstructorException(void) noexcept :
	error_("Error on Gui constructor") {}
Gui::CoreConstructorException::CoreConstructorException(std::string s) noexcept :
	error_(s) {}
Gui::CoreConstructorException::CoreConstructorException(Gui::CoreConstructorException const &src) noexcept :
	error_(src.error_)
	{ error_ = src.error_; }
const char	*Gui::CoreConstructorException::what() const noexcept
	{ return (error_.c_str()); }

