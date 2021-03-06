#include "WidgetLobby.hpp"
#include <cores/Univers.hpp>
#include <gui/Gui.hpp>

WidgetLobby::WidgetLobby(Gui &core) :
		AWidget(core, "None", 0) {
	addColor(eSprite::kGreen, "Green", (core_.getPathRessources() /
										WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
										"snake_green.png").generic_string());
	addColor(eSprite::kBlue, "Blue", (core_.getPathRessources() /
									  WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
									  "snake_blue.png").generic_string());
	addColor(eSprite::kPurple, "Purple", (core_.getPathRessources() /
										  WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
										  "snake_purple.png").generic_string());
	addColor(eSprite::kPink, "Pink", (core_.getPathRessources() /
									  WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
									  "snake_pink.png").generic_string());
	addColor(eSprite::kGrey, "Grey", (core_.getPathRessources() /
									  WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
									  "snake_grey.png").generic_string());
	addColor(eSprite::kYellow, "Yellow", (core_.getPathRessources() /
										  WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
										  "snake_yellow.png").generic_string());
	addColor(eSprite::kOrange, "Orange", (core_.getPathRessources() /
										  WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
										  "snake_orange.png").generic_string());
	addColor(eSprite::kRed, "Red", (core_.getPathRessources() /
									WIDGET_LOBBY_DIRECTORY_SNAKE_PRESENTATION /
									"snake_red.png").generic_string());
}

void WidgetLobby::update_() {
	active_ = false;
	snakeWidget_.clear();
	snakes_ = core_.univers.getSnakeArray_();
	sf::Vector2<unsigned int> placesForSnakes(4, 2);
	sf::Vector2<unsigned int> percentPlaceOfSnake;

	if (!reload_())
			return ;

	for (size_t i = 0; i < snakeWidget_.size(); i++) {

		percentPlaceOfSnake.x = ((100 / placesForSnakes.x) *
								 (i % placesForSnakes.x));
		percentPlaceOfSnake.y = ((50 / placesForSnakes.y) *
								 (i / placesForSnakes.x));
		ImGui::SetNextWindowPos(
				core_.positionByPercent(percentPlaceOfSnake));
		ImGui::SetNextWindowSize(core_.positionByPercent(
				sf::Vector2<unsigned int>(100 / placesForSnakes.x,
										  50 / placesForSnakes.y)));
		snakeWidget_[i].render(true);
	}
}

void WidgetLobby::addColor(eSprite color, std::string const &name,
						   std::string const &pathColor) {
	mapSprite_[color].color = color;
	mapSprite_[color].name = name;
	if (!(mapSprite_[color].sprite.loadFromFile(pathColor)))
		throw (AWidget::Constructor(std::string("WidgetLobby: Cant load [") + pathColor + "]"));
}


void WidgetLobby::addSnake(const Snake &snake, WidgetSnake::SnakeType type) {
	if (snakeWidget_.size() < SNAKE_MAX)
		snakeWidget_.emplace_back(core_, snake, mapSprite_, type);
}

bool WidgetLobby::reload_() {
	snakeWidget_.clear();

	SnakeClient::boost_shared_ptr ptr(core_.univers.getSnakeClient().lock());
	SnakeArrayPtr ptr_snakes(snakes_.lock());

	if (!ptr_snakes) {
		return false;
	}

	for (unsigned int i = 0; i < SNAKE_MAX; i++) {
		if ((*ptr_snakes)[i].isIA)
			addSnake((*ptr_snakes)[i], WidgetSnake::kIa);
		else if (ptr && i == ptr->getId_() && ptr->isOpen())
			addSnake((*ptr_snakes)[i], WidgetSnake::kYour);
		else
			addSnake((*ptr_snakes)[i], WidgetSnake::kBasic);
	}
	return true;
}


void WidgetLobby::beginContent_() {}
