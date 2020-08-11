#pragma once

#include <memory>
#include<string>
#include <SFML/Graphics.hpp>
#include"AssetManager.h"
#include "InputManager.h"
#include "StateMechine.h"

namespace Meir {

	struct  GameData
	{
		StateMechine machine;
		sf::RenderWindow window;
		AssetManager asset;
		InputManager input;
	};


	typedef std::shared_ptr<GameData> GameDataRef;

	
	class Game
	{
	public:
		Game(int width, int height, std::string title);
		
	private:
		const float dt = 1.f / 60.f;
		sf::Clock _clock;

		GameDataRef _data = std::make_shared<GameData>();

		void Run();
	};
}
