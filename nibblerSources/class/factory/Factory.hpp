#pragma once

#include <nibbler.hpp>
#include <cores/Snake.hpp>

class Univers;

class Factory {
public:
	Factory(Univers &univers);
	Factory() = delete;
	~Factory() = default;
	Factory &operator=(const Factory &) = delete;
	Factory(const Factory &) = delete;

	void createAllSnake(SnakeArrayContainer &snake_array, int16_t nu);

private:

	void createSnake(Snake const &snake, int maxSnakes);

	void createWalls();

	void createWall(int x, int y);

	Univers &univers_;
};
