#pragma once

namespace Meir
{
	class State
	{
	public:
		virtual void Init() = 0;
		
		virtual void HandleInput() = 0;

		virtual void update(float dt) = 0;

		virtual void Draw(float dt) = 0;

		virtual void Pause() {}
		virtual void Resume() {}
	};
}