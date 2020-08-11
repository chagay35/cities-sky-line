#include <sstream>
#include <iostream>
#include "SplashState.h"
#include "DEFINITIONS.h"
#include "MainMenuState.h"

namespace Meir
{
	SplashState::SplashState(GameDataRef data)
		:_data(data)
	{
		sf::Vector2f pos = sf::Vector2f(this->_data->window.getSize());
		this->_view.setSize(pos);
		pos *= 0.5f;
		this->_view.setCenter(pos);
	}


	void SplashState::Init()
	{
		this->_data->asset.LoadTexture("Splash State Background", SPLASH_SCREEN_BACKGROUND_FILEPATH);

		_background.setTexture(this->_data->asset.getTexture("Splash State Background"));
	}


	void SplashState::HandleInput()
	{
		sf::Event event;

		while (this->_data->window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				this->_data->window.close();
				break;
			}
			default:
				break;
			}
		}
	}

	
	void SplashState::update(float dt)
	{
		if (this->_clock.getElapsedTime().asSeconds() > SPLASH_STATE_SHOW_TIME)
		{
			//Switch to the Main Menu
			this->_data->machine.AddState(StateRef(new MainMenuState(_data)), true);
		}
	}


	void SplashState::Draw(float dt)
	{
		this->_data->window.clear(sf::Color::Red);
		this->_data->window.draw(this->_background);
		this->_data->window.display();
	}
}