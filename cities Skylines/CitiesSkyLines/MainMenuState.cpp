#include "MainMenuState.h"
#include "DEFINITIONS.h"
#include "GameState.h"
#include <iostream>


namespace Meir
{
	MainMenuState::MainMenuState(GameDataRef data)
		:_data(data)
	{
		sf::Vector2f pos = sf::Vector2f(this->_data->window.getSize());
		this->_view.setSize(pos);
		pos *= 0.5f;
		this->_view.setCenter(pos);
	}


	void MainMenuState::Init()
	{
		this->_data->asset.LoadTexture("Background", MAIN_MENU_BACKGROUND_FILEPATH);

		this->_data->asset.LoadTexture("Play Button", MAIN_MENU_PLAY_BUTTON);


		
		this->_background.setTexture(this->_data->asset.getTexture("Background"));

		this->_playButton.setTexture(this->_data->asset.getTexture("Play Button"));

	

		this->_playButton.setPosition((SCREEN_WIDTH / 2) - (this->_playButton.getGlobalBounds().width / 2),
			(SCREEN_HEIGHT / 2) - (this->_playButton.getGlobalBounds().height / 2)*5);

	}


	//=====================================================================


	void MainMenuState::HandleInput()
	{
		sf::Event event;

		sf::Vector2f mousePos = this->_data->window.mapPixelToCoords(sf::Mouse::getPosition(this->_data->window), this->_view);

		while (this->_data->window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				this->_data->window.close();
				break;
			}
			case sf::Event::Resized:
			{
				this->_view.setSize(event.size.width, event.size.height);
				this->_background.setPosition(this->_data->window.mapPixelToCoords(sf::Vector2i(0, 0)));
				this->_playButton.setPosition(this->_data->window.mapPixelToCoords(sf::Vector2i(0, 0)));

				this->_background.setScale(
					float(event.size.width) / float(this->_background.getTexture()->getSize().x),
					float(event.size.height) / float(this->_background.getTexture()->getSize().y));
				this->_playButton.setScale(
					float(event.size.width) / float(this->_playButton.getTexture()->getSize().x),
					float(event.size.height) / float(this->_playButton.getTexture()->getSize().y));

				break;
			}
			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Escape) 
					this->_data->window.close();
				break;
			}
			default:
				break;
			}

			if (this->_data->input.IsSpriteClicked(this->_playButton, sf::Mouse::Left, this->_data->window))
			{
				this->_data->machine.AddState(StateRef(new GameState(this->_data)),false);
			}

		}
	}


	//=====================================================================


	void MainMenuState::update(float dt)
	{


	}


	//======================================================================


	void MainMenuState::Draw(float dt)
	{
		this->_data->window.clear();

		this->_data->window.setView(this->_view);

		this->_data->window.draw(this->_background);
		this->_data->window.draw(this->_playButton);

		this->_data->window.display();
	}
}