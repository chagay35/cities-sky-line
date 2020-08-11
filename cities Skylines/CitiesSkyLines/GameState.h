#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "State.h"
#include "Game.h"
#include "Tile.h"
#include "City.h"
#include "Gui.h"

enum class ActionState { NONE, PANNING, SELECTING};

namespace Meir
{
	class GameState: public State
	{
	public:
		GameState(GameDataRef data);

		virtual void Init();

		virtual void HandleInput();

		virtual void update(float dt);

		virtual void Draw(float dt);

	private:
		void createGuiSystem();

		ActionState actionState;

		sf::View _gameView;
		sf::View _guiView;

		City city;

		sf::Vector2i panningAnchor;
		float zoomLevel;

		sf::Vector2i selectionStart;
		sf::Vector2i selectionEnd;

		Tile* currentTile;

		const static int tileSize = 8;

		void loadTextures();
		void loadTiles();
		void loadStylesheets();

		GameDataRef _data;

		sf::Clock _clock;

		sf::Sprite _background;

		std::map<std::string, Tile> tileAtlas;
		std::map<std::string, GuiStyle> stylesheets;
		std::map<std::string, Gui> guiSystem;
	};
}

