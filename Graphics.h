#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace Graphics
{
	class Renderer
	{
		private:
			static Renderer*				singleton;
			sf::RenderWindow				window;
			std::vector<sf::Drawable *> 	objectsToDraw;
		public:
			Renderer();
			~Renderer();
			void DrawObjects(sf::RenderWindow* win)
			{
				for(auto it=objectsToDraw.begin(); it!=objectsToDraw.end(); ++it) win->Draw(**it);
			}
	};
}
