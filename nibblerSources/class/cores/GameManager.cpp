#include "GameManager.hpp"
#include "cores/Univers.hpp"
#include <network/SnakeServer.hpp>
#include <KINU/World.hpp>
#include <systems/MotionSystem.hpp>
#include <systems/JoystickSystem.hpp>
#include <systems/FollowSystem.hpp>
#include <systems/FoodCreationSystem.hpp>
#include <systems/CollisionSystem.hpp>
#include <systems/SpriteSystem.hpp>
#include <systems/RenderSystem.hpp>
#include <systems/FoodEatSystem.hpp>
#include <events/FoodEat.hpp>
#include <KINU/World.hpp>
#include <events/StartEvent.hpp>
#include <events/FoodCreation.hpp>
#include <events/JoystickEvent.hpp>

const uint32_t GameManager::ScaleByFrame = 10;
const uint32_t GameManager::ScaleByRealFood = 600;
const uint32_t GameManager::ScaleByFakeFood = 50;

const uint32_t GameManager::Easy = 1000000;  //1 sec frame
const uint32_t GameManager::Medium = 120000;  //0.12sec frame
const uint32_t GameManager::Hard = 80000;  //0.08sec frame
const uint32_t GameManager::Impossible = 10000;  //0.01sec frame

GameManager::GameManager(Univers &univers)
	:
	univers_(univers),
	timer_loop(univers_.getIoManager().getIo()) {
}


void GameManager::startNewGame() {

	world_ = std::make_unique<KINU::World>();

	univers_.setGrid_(std::make_shared<MutantGrid<eSprite>>(univers_.getMapSize()));
	univers_.getGrid_().fill(eSprite::kNone);

	world_->getSystemsManager().addSystem<CollisionSystem>(univers_);
	world_->getSystemsManager().addSystem<FollowSystem>();
	world_->getSystemsManager().addSystem<JoystickSystem>();
	world_->getSystemsManager().addSystem<MotionSystem>(univers_);
	world_->getSystemsManager().addSystem<SpriteSystem>(univers_);
	world_->getSystemsManager().addSystem<RenderSystem>(univers_);
	world_->getSystemsManager().addSystem<FoodCreationSystem>();
	world_->getSystemsManager().addSystem<FoodEatSystem>();


	if (univers_.isServer()) {
		univers_.getSnakeServer().startGame();
	}

	SnakeClient::boost_shared_ptr ptr(univers_.getSnakeClient().lock());

	std::vector<StartEvent> startEvent;

	for (;ptr && startEvent.empty();) {
		ptr->lock();
		startEvent = world_->getEventsManager().getEvents<StartEvent>();
		ptr->unlock();
	}

	world_->update();
	world_->getSystemsManager().getSystem<SpriteSystem>().update();
	world_->getSystemsManager().getSystem<RenderSystem>().update();
	univers_.updateDisplayUI();

	if (univers_.isServer()) {
		for (auto &bobby : univers_.getBobbys()) {
			bobby->buildIA();
//			bobby->sendDirection();
		}
	}

//	univers_.manageSnakeClientInput();

	threadWorldLoop_ = boost::thread([this, startEvent](){

		boost::asio::deadline_timer timer_start(univers_.getIoManager().getIo());

		timer_start.expires_at(startEvent.front().start_time);
		timer_start.wait();

		loopGame();
	});
}


void GameManager::loopGame() {
	SnakeClient::boost_shared_ptr ptr(univers_.getSnakeClient().lock());

	while (!ptr->allSnakeIsDead() && univers_.isOpenGame_() && univers_.displayIsAvailable()) {
		manageGlobalInput();
		loopWorld();
		timer_loop.expires_from_now(boost::posix_time::microsec(univers_.getMicroSecDeltaTime()));
		timer_loop.wait();
	}
}

//  ( Update Delta [Input Frame])
//			0.01s			1s

// Input < Frame
//

void GameManager::manageGlobalInput() {
	boost::asio::thread_pool pool(8);
	SnakeClient::boost_shared_ptr ptr(univers_.getSnakeClient().lock());
	if (!ptr)
		return;

	{ /** Manage Input **/
		Bobby::clearPriority();
		if (univers_.isServer()) {
			for (auto &bobby : univers_.getBobbys()) {
				if (world_->getEntitiesManager().hasEntityByTagId(bobby->getId() + eTag::kHeadTag)) {
					ptr->addScore(bobby->getId(), eScore::kFromTime);
					boost::asio::post(pool, [&bobby](){
						bobby->calculateDirection();
					});
				}
			}
			pool.join();
		}
		univers_.manageSnakeClientInput();
	}
	{ /** Manage Frame **/
		for (; nextFrame.empty() && univers_.isOpenGame_();) {
			ptr->lock();
			nextFrame = world_->getEventsManager().getEvents<NextFrame>();
			ptr->unlock();
		}
		nextFrame.clear();
		world_->getEventsManager().destroy<NextFrame>();
		//std::array<Snake, SNAKE_MAX>
		//std::array<eDirection, SNAKE_MAX>

		DirectionArray directions;
		size_t n = 0;
		SnakeArrayContainer array = *(ptr->getSnakeArray_());
		std::generate(directions.begin(), directions.end(), [&n, array]{ return array[n++].direction;});
		world_->getEventsManager().emitEvent<DirectionArray>(directions);
		for (int index = 0; index < SNAKE_MAX; ++index) {
			assert(array[index].direction == directions[index]);
		}
	}
}

void GameManager::loopWorld() {

	SnakeClient::boost_shared_ptr ptr(univers_.getSnakeClient().lock());
	if (!ptr) return;

	{ /** Manage game **/
		ptr->deliverEvents();
		world_->getSystemsManager().getSystem<FollowSystem>().update();
		world_->getSystemsManager().getSystem<JoystickSystem>().update();
		world_->getEventsManager().destroy<JoystickEvent>();
		world_->getSystemsManager().getSystem<MotionSystem>().update();
		world_->getSystemsManager().getSystem<CollisionSystem>().update();
		world_->getSystemsManager().getSystem<FoodCreationSystem>().update();
		world_->getEventsManager().destroy<FoodCreation>();
		world_->getSystemsManager().getSystem<SpriteSystem>().update();
		world_->getSystemsManager().getSystem<RenderSystem>().update();
		world_->getSystemsManager().getSystem<FoodEatSystem>().update();
		world_->getEventsManager().destroy<FoodEat>();
		world_->update();
	}




}

void GameManager::loopUI() {

	SnakeClient::boost_shared_ptr ptr(univers_.getSnakeClient().lock());

	while (univers_.isOpenGame_() && univers_.displayIsAvailable()) {

		if (!ptr || ptr->allSnakeIsDead())
			break;

		if (univers_.isSwitchLib())
			univers_.manageSwitchLibrary();

		univers_.updateDisplayUI();
	}

	log_error("%s %d", __PRETTY_FUNCTION__,  ptr->allSnakeIsDead());
}

void GameManager::refreshTimerLoopWorld() {
	timer_loop.expires_at(boost::posix_time::microsec_clock::universal_time());
}

void GameManager::finishGame() {
	threadWorldLoop_.join();
	nextFrame.clear();
	world_ = nullptr;
}

const std::shared_ptr<KINU::World> &GameManager::getWorld_() {
	return world_;
}
