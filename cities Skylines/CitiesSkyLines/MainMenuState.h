#pragma once

#include <SFML/Graphics.hpp>
#include "State.h"
#include "Game.h"

namespace Meir
{
	class MainMenuState: public State
	{
	public:
		MainMenuState(GameDataRef data);

		virtual void Init();

		virtual void HandleInput();

		virtual void update(float dt);

		virtual void Draw(float dt);

	private:
		GameDataRef _data;

		sf::View _view;

		sf::Clock _clock;

		sf::Sprite _background;

		sf::Sprite _playButton;
	};
}

