#include "AssetManager.h"

namespace Meir
{
	void AssetManager::LoadTexture(std::string name, std::string fileName)
	{
		sf::Texture tex;

		if (tex.loadFromFile(fileName))
		{
			this->_textures[name] = tex;
		}
	}


	sf::Texture & AssetManager::getTexture(std::string name)
	{
		return this->_textures.at(name);
	}


	void AssetManager::LoadFont(std::string name, std::string fileName)
	{
		sf::Font font;

		if (font.loadFromFile(fileName))
		{
			this->_fonts[name] = font;
		}
	}


	sf::Font & AssetManager::getFont(std::string name)
	{
		return this->_fonts.at(name);
	}
}
