#pragma once
#include "AWidget.hpp"
#include <string>
#include <vector>
#include <SFML/Graphics.hpp>
#include "WidgetChat.hpp"

class WidgetSnake : public AWidget {
public:
	WidgetSnake(void);
	WidgetSnake(void (*callback)(void *), void *ptr);
	~WidgetSnake(void);


	void	render(void);

private:
	std::vector<sf::Texture>					_texture;
	std::vector<std::string>					_color;
	unsigned int								_indexColor;

	WidgetSnake &operator=(const WidgetSnake&);
	WidgetSnake(const WidgetSnake&);
};