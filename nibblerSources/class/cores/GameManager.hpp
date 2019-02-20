#pragma once

#include "GameManager.hpp"
#include <vector>
#include <KINU/World.hpp>
#include <events/NextFrame.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <cores/Univers.hpp>

class GameManager {
public:


	GameManager(Univers &univers);

	GameManager() = delete;

	~GameManager() = default;

	GameManager &operator=(const GameManager &) = delete;

	GameManager(const GameManager &) = delete;

	void startNewGame();

	void loopWorld();

	void loopUI();

	void refreshTimerLoopWorld();

	void finishGame();

	KINU::World &getWorld_();

	static const uint32_t ScaleByFrame;
	static const uint32_t ScaleByRealFood;
	static const uint32_t ScaleByFakeFood;
	static const uint32_t Easy;
	static const uint32_t Medium;
	static const uint32_t Hard;
	static const uint32_t Impossible;
private:

	boost::thread *thread;
	Univers &univers_;
	std::vector<NextFrame> nextFrame;
	boost::asio::deadline_timer timer_loop;
	std::shared_ptr<KINU::World> world_;
};


