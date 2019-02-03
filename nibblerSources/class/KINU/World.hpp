#pragma once

#include "Entity.hpp"
#include "SystemsManager.hpp"
#include "EventsManager.hpp"
#include <vector>
#include <string>
#include <memory>
#include <IDisplay.hpp>
#include <Univers.hpp>
#include "MutantGrid.tpp"

namespace KINU {

// The World manages the creation and destruction of entities so that entities.
	class World {
	public:
		World(Univers &univers);

		Univers &getUnivers() const;

		EntitiesManager &getEntitiesManager() const;

		SystemsManager &getSystemsManager() const;

		EventsManager &getEventsManager() const;


		void update();

		Entity createEntity();

		void destroyEntity(Entity e);

		MutantGrid< eSprite > grid;
	private:
		// vector of entities that are awaiting creation
		std::vector<Entity> createdEntities;

		// vector of entities that are awaiting destruction
		std::vector<Entity> destroyedEntities;

		std::unique_ptr<EntitiesManager> entitiesManager;
		std::unique_ptr<SystemsManager> systemsManager;
		std::unique_ptr<EventsManager> eventsManager;

		Univers &univers_;
	};

}


