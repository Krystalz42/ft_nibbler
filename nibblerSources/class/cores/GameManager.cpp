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

const uint32_t GameManager::ScaleByFrame = 20;
const uint32_t GameManager::ScaleByRealFood = 600;
const uint32_t GameManager::ScaleByFakeFood = 50;

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
	world_->getSystemsManager().addSystem<FoodCreationSystem>(univers_.getMapSize());
	world_->getSystemsManager().addSystem<FoodEatSystem>();
	world_->getEventsManager().emitEvent(NextFrame());
	world_->getEventsManager().destroy<NextFrame>();

	if (univers_.isServer()) {
		univers_.getSnakeServer().startGame();
	}

	SnakeClient::boost_shared_ptr ptr(univers_.getSnakeClient().lock());

	std::vector<StartEvent> startEvent;

	for (;ptr && startEvent.empty();) {
		ptr->lock();
		startEvent = world_->getEventsManager().getEvents<StartEvent>();
		ptr->unlock();
		if (!univers_.isOpenGame_())
			return ;
	}

	world_->update();
	world_->getSystemsManager().getSystem<SpriteSystem>().update();
	world_->getSystemsManager().getSystem<RenderSystem>().update();
	univers_.updateDisplayUI();

	if (univers_.isServer()) {
		for (auto &bobby : univers_.getBobbys()) {
			bobby->buildIA();
		}
	}

	univers_.getSoundManager().playNoise(eNoise::kGo);
	threadWorldLoop_ = boost::thread([this, startEvent](){

		boost::asio::deadline_timer timer_start(univers_.getIoManager().getIo());
		assert(!startEvent.empty());
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
					boost::asio::post(pool, [&bobby](){ bobby->calculateDirection(); });
				}
			}
			pool.join();
		}
		univers_.manageSnakeClientInput();
	}
	{ /** Manage Frame **/
		for (; world_->getEventsManager().getEvents<NextFrame>().empty() && univers_.isOpenGame_(););
		ptr->lock();
		world_->getEventsManager().destroy<NextFrame>();

		DirectionArray directions;
		size_t n = 0;
		SnakeArrayContainer array = *(ptr->getSnakeArray_());
		assert(array.size() == directions.size());
		assert(array.size() == SNAKE_MAX);
		std::generate(directions.begin(), directions.end(), [&n, array]{ return array[n++].direction;});
		world_->getEventsManager().emitEvent<DirectionArray>(directions);
		for (int index = 0; index < SNAKE_MAX; ++index) {
			assert(array[index].direction == directions[index]);
		}
		ptr->unlock();
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

}

void GameManager::refreshTimerLoopWorld() {
	timer_loop.expires_at(boost::posix_time::microsec_clock::universal_time());
}

void GameManager::finishGame() {
	threadWorldLoop_.join();
	world_ = nullptr;
}

const std::shared_ptr<KINU::World> &GameManager::getWorld_() {
	return world_;
}
