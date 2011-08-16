#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

namespace Graphics
{
	class RendererImpl
	{
		friend class Renderer;
		private:
			sf::RenderWindow				window;
			std::vector<sf::Drawable *> 	objectsToDraw;
			void Draw()
			{
				window.Clear();
				for(auto it=objectsToDraw.begin(); it!=objectsToDraw.end(); ++it) window.Draw(**it);
				window.Display();
			}
			RendererImpl(uint w, uint h) : window(sf::VideoMode(w,h,32), "") {}
	};

	class Renderer
	{
		private:
			static RendererImpl* 	impl;
			static uint				references;
		public:
			Renderer(uint w, uint h);
			~Renderer();
			void AddObject(sf::Drawable& obj) {impl->objectsToDraw.push_back(&obj);}
			void Draw() {impl->Draw();}
	};
	/*
	class EventManager
	{
		private:
			sf::Event e;
		public:
			sf::Event& getEvent() {Renderer::impl->window.PollEvent(e); return e;}
	};
	*/
}
