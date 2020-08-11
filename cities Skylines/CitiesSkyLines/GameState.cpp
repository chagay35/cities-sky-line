#include "GameState.h"
#include "DEFINITIONS.h"
#include <iostream>


namespace Meir
{
	GameState::GameState(GameDataRef data)
		:_data(data)
	{
		sf::Vector2f pos = sf::Vector2f(this->_data->window.getSize());
		this->_guiView.setSize(pos);
		this->_gameView.setSize(pos);
		pos *= 0.5f;
		this->_guiView.setCenter(pos);
		this->_gameView.setCenter(pos);

		this->Init();

		this->city = City("city", this->tileSize, this->tileAtlas);
		this->city.shuffleTiles();

		this->zoomLevel = 1.0f;

		/* Centre the camera on the map */
		sf::Vector2f centre(this->city.map.width, this->city.map.height*0.5);
		centre *= float(this->city.map.tileSize);
		_gameView.setCenter(centre);

		this->actionState = ActionState::NONE;

		
	}

	//============================================================

	void GameState::Init()
	{
		this->_data->asset.LoadFont("main_font", MAIN_FONT_FILEPATH);
		this->loadTextures();
		this->loadTiles();
		loadStylesheets();
		this->_background.setTexture(this->_data->asset.getTexture("Game State Background"));

		createGuiSystem();

		this->selectionStart = sf::Vector2i(0, 0);
		this->selectionEnd = sf::Vector2i(0, 0);

		this->currentTile = &this->tileAtlas.at("grass");
		this->actionState = ActionState::NONE;

		

		
	}

	//============================================================

	void GameState::HandleInput()
	{
		sf::Event event;

		sf::Vector2f guiPos = this->_data->window.mapPixelToCoords(sf::Mouse::getPosition(this->_data->window), this->_guiView);
		sf::Vector2f gamePos = this->_data->window.mapPixelToCoords(sf::Mouse::getPosition(this->_data->window), this->_gameView);

		while (this->_data->window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
			{
				this->_data->window.close();
				break;
			}
			/* Resize the window */
			case sf::Event::Resized:
			{
				_gameView.setSize(event.size.width, event.size.height);
				_gameView.zoom(zoomLevel);
				this->_background.setPosition(this->_data->window.mapPixelToCoords(sf::Vector2i(0, 0), this->_guiView));
				this->_background.setScale(
					float(event.size.width) / float(this->_background.getTexture()->getSize().x),
					float(event.size.height) / float(this->_background.getTexture()->getSize().y));
				break;
			}

			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Escape)
					this->_data->machine.RemoveState();
				break;
			}
			case sf::Event::MouseMoved:
			{
				/* Pan the camera */
				if (this->actionState == ActionState::PANNING)
				{
					sf::Vector2f pos = sf::Vector2f(sf::Mouse::getPosition(this->_data->window) - this->panningAnchor);
					_gameView.move(-1.0f * pos * this->zoomLevel);
					panningAnchor = sf::Mouse::getPosition(this->_data->window);
				}
				/* Select tiles */
				else if (actionState == ActionState::SELECTING)
				{
					sf::Vector2f pos = this->_data->window.mapPixelToCoords(sf::Mouse::getPosition(this->_data->window), this->_gameView);
					selectionEnd.x = pos.y / (this->city.map.tileSize) + pos.x / (2 * this->city.map.tileSize) - this->city.map.width * 0.5 - 0.5;
					selectionEnd.y = pos.y / (this->city.map.tileSize) - pos.x / (2 * this->city.map.tileSize) + this->city.map.width * 0.5 + 0.5;

					this->city.map.clearSelected();
					if (this->currentTile->tileType == TileType::GRASS)
					{
						this->city.map.select(selectionStart, selectionEnd, { this->currentTile->tileType, TileType::WATER });
					}
					else
					{
						this->city.map.select(selectionStart, selectionEnd,
							{
								this->currentTile->tileType,    TileType::FOREST,
								TileType::WATER,                TileType::ROAD,
								TileType::RESIDENTIAL,          TileType::COMMERCIAL,
								TileType::INDUSTRIAL
							});
					}
					this->guiSystem.at("selectionCostText").setEntryText(0, "$" + std::to_string(this->currentTile->cost * this->city.map.numSelected));
					if (this->city.funds <= this->city.map.numSelected * this->currentTile->cost)
						this->guiSystem.at("selectionCostText").highlight(0);
					else
						this->guiSystem.at("selectionCostText").highlight(-1);
					this->guiSystem.at("selectionCostText").setPosition(guiPos + sf::Vector2f(16, -16));
					this->guiSystem.at("selectionCostText").show();
			}
			/* Highlight entries of the right click context menu */
			this->guiSystem.at("rightClickMenu").highlight(this->guiSystem.at("rightClickMenu").getEntry(guiPos));

				break;
			}
			case sf::Event::MouseButtonPressed:
			{
				/* Start panning */
				if (event.mouseButton.button == sf::Mouse::Middle)
				{
					this->guiSystem.at("rightClickMenu").hide();
					this->guiSystem.at("selectionCostText").hide();
					if (this->actionState != ActionState::PANNING)
					{
						this->actionState = ActionState::PANNING;
						this->panningAnchor = sf::Mouse::getPosition(this->_data->window);
					}
				}
				else if (event.mouseButton.button == sf::Mouse::Left)
				{
					/* Select a context menu entry. */
					if (this->guiSystem.at("rightClickMenu").visible == true)
					{
						std::string msg = this->guiSystem.at("rightClickMenu").activate(guiPos);
						if (msg != "null") this->currentTile = &this->tileAtlas.at(msg);

						this->guiSystem.at("rightClickMenu").hide();
					}
					/* Select map tile. */
					else
					{
						/* Select map tile. */
						if (this->actionState != ActionState::SELECTING)
						{
							this->actionState = ActionState::SELECTING;
							selectionStart.x = gamePos.y / (this->city.map.tileSize) + gamePos.x / (2 * this->city.map.tileSize) - this->city.map.width * 0.5 - 0.5;
							selectionStart.y = gamePos.y / (this->city.map.tileSize) - gamePos.x / (2 * this->city.map.tileSize) + this->city.map.width * 0.5 + 0.5;
						}
					}
				}
				else if (event.mouseButton.button == sf::Mouse::Right)
				{
					/* Stop selecting. */
					if (this->actionState == ActionState::SELECTING)
					{
						this->actionState = ActionState::NONE;
						this->guiSystem.at("selectionCostText").hide();
						this->city.map.clearSelected();
					}
					else
					{
						/* Open the tile select menu. */
						sf::Vector2f pos = guiPos;

						if (pos.x > this->_data->window.getSize().x - this->guiSystem.at("rightClickMenu").getSize().x)
						{
							pos -= sf::Vector2f(this->guiSystem.at("rightClickMenu").getSize().x, 0);
						}
						if (pos.y > this->_data->window.getSize().y - this->guiSystem.at("rightClickMenu").getSize().y)
						{
							pos -= sf::Vector2f(0, this->guiSystem.at("rightClickMenu").getSize().y);
						}
						this->guiSystem.at("rightClickMenu").setPosition(pos);
						this->guiSystem.at("rightClickMenu").show();
					}
				}
				break;
			}
			case sf::Event::MouseButtonReleased:
			{
				/* Stop panning */
				if (event.mouseButton.button == sf::Mouse::Middle)
				{
					this->actionState = ActionState::NONE;
				}
				/* Stop selecting */
				else if (event.mouseButton.button == sf::Mouse::Left)
				{
					if (this->actionState == ActionState::SELECTING)
					{
						/* Replace tiles if enough funds and a tile is selected */
						if (this->currentTile != nullptr)
						{
							unsigned int cost = this->currentTile->cost * this->city.map.numSelected;
							if (this->city.funds >= cost)
							{
								this->city.bulldoze(*this->currentTile);
								this->city.funds -= this->currentTile->cost * this->city.map.numSelected;
								this->city.tileChanged();
							}
						}
						this->guiSystem.at("selectionCostText").hide();
						this->actionState = ActionState::NONE;
						this->city.map.clearSelected();
					}
				}
				break;
			}
			/* Zoom the view */
			case sf::Event::MouseWheelMoved:
			{
				if (event.mouseWheel.delta < 0)
				{
					_gameView.zoom(2.0f);
					zoomLevel *= 2.0f;
				}
				else
				{
					_gameView.zoom(0.5f);
					zoomLevel *= 0.5f;
				}
				break;
			}
			default:
				break;
			}
		}
	}

	//=============================================================

	void GameState::update(float dt)
	{
		this->city.update(dt);

		/* Update the info bar at the bottom of the screen */
		this->guiSystem.at("infoBar").setEntryText(0, "Day: " + std::to_string(this->city.day));
		this->guiSystem.at("infoBar").setEntryText(1, "$" + std::to_string(long(this->city.funds)));
		this->guiSystem.at("infoBar").setEntryText(2, std::to_string(long(this->city.population)) + " (" + std::to_string(long(this->city.getHomeless())) + ")");
		this->guiSystem.at("infoBar").setEntryText(3, std::to_string(long(this->city.employable)) + " (" + std::to_string(long(this->city.getUnemployed())) + ")");
		this->guiSystem.at("infoBar").setEntryText(4, tileTypeToStr(currentTile->tileType));

		/* Highlight entries of the right click context menu */
		this->guiSystem.at("rightClickMenu").highlight(this->guiSystem.at("rightClickMenu").getEntry(this->_data->window.mapPixelToCoords(sf::Mouse::getPosition(this->_data->window), this->_guiView)));
		return;
	}

	//==========================

	void GameState::Draw(float dt)
	{
		this->_data->window.clear(sf::Color::Black);

		this->_data->window.setView(this->_guiView);
		this->_data->window.draw(this->_background);

		this->_data->window.setView(this->_gameView);
		city.map.draw(this->_data->window, dt);

		this->_data->window.setView(this->_guiView);
		for (auto gui : this->guiSystem)
			this->_data->window.draw(gui.second);

		this->_data->window.display();
	}

	//==================================================================================

	void GameState::createGuiSystem()
	{
		/* Create gui elements. */
		this->guiSystem.emplace("rightClickMenu", Gui(sf::Vector2f(196, 16), 2, false, this->stylesheets.at("button"),
			{
				std::make_pair("Flatten $" + this->tileAtlas["grass"].getCost(),         "grass"),
				std::make_pair("Forest $" + this->tileAtlas["forest"].getCost(),        "forest"),
				std::make_pair("Residential Zone $" + this->tileAtlas["residential"].getCost(),   "residential"),
				std::make_pair("Commercial Zone $" + this->tileAtlas["commercial"].getCost(),    "commercial"),
				std::make_pair("Industrial Zone $" + this->tileAtlas["industrial"].getCost(),    "industrial"),
				std::make_pair("Road $" + this->tileAtlas["road"].getCost(),          "road")
			}));

		this->guiSystem.emplace("selectionCostText", Gui(sf::Vector2f(196, 16), 0, false, this->stylesheets.at("text"),
			{ std::make_pair("", "") }));

		this->guiSystem.emplace("infoBar", Gui(sf::Vector2f(this->_data->window.getSize().x / 5, 16), 2, true, this->stylesheets.at("button"),
			{
				std::make_pair("time",          "time"),
				std::make_pair("funds",         "funds"),
				std::make_pair("population",    "population"),
				std::make_pair("employment",    "employment"),
				std::make_pair("current tile",  "tile")
			}));
		this->guiSystem.at("infoBar").setPosition(sf::Vector2f(0, this->_data->window.getSize().y - 16));
		this->guiSystem.at("infoBar").show();
	}

	//====================================================================

	void GameState::loadTextures()
	{
		this->_data->asset.LoadTexture("grass", GRASS_TEXTURE_FILEPATH);
		this->_data->asset.LoadTexture("forest", FOREST_TEXTURE_FILEPATH);
		this->_data->asset.LoadTexture("water", WATER_TEXTURE_FILEPATH);
		this->_data->asset.LoadTexture("residential", RESIDENTIAL_TEXTURE_FILEPATH);
		this->_data->asset.LoadTexture("commercial", COMMERCIAL_TEXTURE_FILEPATH);
		this->_data->asset.LoadTexture("industrial", INDUSTRIAL_TEXTURE_FILEPATH);
		this->_data->asset.LoadTexture("road", ROAD_TEXTURE_FILEPATH);

		this->_data->asset.LoadTexture("Game State Background", GAME_SCREEN_BACKGROUND_FILEPATH);
	}

	//====================================================================

	void GameState::loadTiles()
	{
		Animation staticAnim(0, 0, 1.0f);
		this->tileAtlas["grass"] =
			Tile(this->tileSize, 1, this->_data->asset.getTexture("grass"),
				{ staticAnim },
				TileType::GRASS, 50, 0, 1);
		tileAtlas["forest"] =
			Tile(this->tileSize, 1, this->_data->asset.getTexture("forest"),
				{ staticAnim },
				TileType::FOREST, 100, 0, 1);
		tileAtlas["water"] =
			Tile(this->tileSize, 1, this->_data->asset.getTexture("water"),
				{ Animation(0, 3, 0.5f),
				Animation(0, 3, 0.5f),
				Animation(0, 3, 0.5f) },
				TileType::WATER, 0, 0, 1);
		tileAtlas["residential"] =
			Tile(this->tileSize, 2, this->_data->asset.getTexture("residential"),
				{ staticAnim, staticAnim, staticAnim,
				staticAnim, staticAnim, staticAnim },
				TileType::RESIDENTIAL, 300, 50, 6);
		tileAtlas["commercial"] =
			Tile(this->tileSize, 2, this->_data->asset.getTexture("commercial"),
				{ staticAnim, staticAnim, staticAnim, staticAnim },
				TileType::COMMERCIAL, 300, 50, 4);
		tileAtlas["industrial"] =
			Tile(this->tileSize, 2, this->_data->asset.getTexture("industrial"),
				{ staticAnim, staticAnim, staticAnim,
				staticAnim },
				TileType::INDUSTRIAL, 300, 50, 4);
		tileAtlas["road"] =
			Tile(this->tileSize, 1, this->_data->asset.getTexture("road"),
				{ staticAnim, staticAnim, staticAnim,
				staticAnim, staticAnim, staticAnim,
				staticAnim, staticAnim, staticAnim,
				staticAnim, staticAnim },
				TileType::ROAD, 100, 0, 1);
	}

	//=============================================================================

	void GameState::loadStylesheets()
	{
		
		this->stylesheets["button"] = GuiStyle(&this->_data->asset.getFont("main_font"), 1,
			sf::Color(0xc6, 0xc6, 0xc6), sf::Color(0x94, 0x94, 0x94), sf::Color(0x00, 0x00, 0x00),
			sf::Color(0x61, 0x61, 0x61), sf::Color(0x94, 0x94, 0x94), sf::Color(0x00, 0x00, 0x00));
		this->stylesheets["text"] = GuiStyle(&this->_data->asset.getFont("main_font"), 0,
			sf::Color(0x00, 0x00, 0x00, 0x00), sf::Color(0x00, 0x00, 0x00), sf::Color(0xff, 0xff, 0xff),
			sf::Color(0x00, 0x00, 0x00, 0x00), sf::Color(0x00, 0x00, 0x00), sf::Color(0xff, 0x00, 0x00));

			return;
	}

	//======================================================================

	
}
