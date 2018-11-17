#pragma once

#include <ostream>
#include <json/json.h>

struct PositionComponent {
private:
	static std::string const POS_Y;
	static std::string const POS_X;
public:
	static std::string const NAME_COMPONENT;

	explicit PositionComponent(int x = 0, int y = 0);
	PositionComponent(PositionComponent const &);
	PositionComponent(Json::Value);

	int y, x;

	Json::Value serializeComponent();

	PositionComponent &
	operator=(PositionComponent const &motionComponent);

	bool operator==(const PositionComponent &rhs) const;

	bool operator!=(const PositionComponent &rhs) const;

	friend std::ostream &
	operator<<(std::ostream &os, const PositionComponent &component) {
		os << "y: " << component.y << " x: " << component.x;
		return os;
	}
};

