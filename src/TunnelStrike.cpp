#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

//#include <gl/glu.h>

#include "Quad.hpp"

namespace TunnelStrike {

	class Walls
	{
	public:
		Camera3d camera;
		float maxz;
		std::vector<Quad> q;

		Walls()
			:
			camera(Vector3d(0,0,0), 0, 0, 0, 0, 0),
			maxz(0.0f)
		{
		}

		void extend(float z)
		{
			if (z >= maxz) {
				q.clear();

				for (int i = 0; i < 4; i++) {
					q.push_back(Quad(sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f),
						sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f),
						sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
						sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						camera));
					q.push_back(Quad(sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f),
						sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f),
						sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
						sf::Vector3f(-10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						camera));
					q.push_back(Quad(sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f),
						sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f),
						sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
						sf::Vector3f(10.0f, -10.0f, maxz + i * 100.0f + 100.0f),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						camera));
					q.push_back(Quad(sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f),
						sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f),
						sf::Vector3f(10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
						sf::Vector3f(-10.0f, 10.0f, maxz + i * 100.0f + 100.0f),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - i * 60, 255 - i * 60, 255 - i * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						sf::Color(255 - (i + 1) * 60, 255 - (i + 1) * 60, 255 - (i + 1) * 60),
						camera));
				}

				maxz += 100.0f;
			}
		}

		void draw() const
		{
			for (auto i : q)
				i.draw();
		}
	};

	class Game
	{
	private:
		sf::RenderWindow window;
		sf::Vector2f look;
		Walls walls;

	public:
		Game() : window(sf::VideoMode(1280, 800), "Tunnel Strike")
		{
/*			walls[0] = Quad(sf::Vector3f(-10.0f, -10.0f, 0),
				sf::Vector3f(-10.0f, 10.0f, 0),
				sf::Vector3f(-10.0f, 10.0f, 10.0f),
				sf::Vector3f(-10.0f, -10.0f, 10.0f));
			walls[1] = Quad(sf::Vector3f(-10.0f, 10.0f, 0),
				sf::Vector3f(10.0f, -10.0f, 0),
				sf::Vector3f(-10.0f, 10.0f, 10.0f),
				sf::Vector3f(10.0f, -10.0f, 10.0f));
			walls[2] = Quad(sf::Vector3f(-1.0f, -1.0f, 0),
				sf::Vector3f(-1.0f, 1.0f, 0),
				sf::Vector3f(1.0f, 1.0f, 0),
				sf::Vector3f(1.0f, -1.0f, 0));*/

			walls.extend(0);
		}

	public:
		sf::RenderWindow& getWindow()
		{
			return window;
		}

		void dispatchEvent(const sf::Event& event)
		{
			switch (event.type) {
			case sf::Event::MouseMoved:
				handleMotion(event.mouseMove.x, event.mouseMove.y);
			}
		}

		void handleMotion(int x, int y)
		{
			sf::Mouse::setPosition(sf::Vector2i(window.getSize().x / 2, window.getSize().y / 2), window);

			look.x += x;
			look.y += y;

			glLoadIdentity();
			gluLookAt(0, 0, -10, 0, 0, 0, 0, 1, 0);
			glRotatef(look.y, 1, 0, 0);
			glRotatef(look.x, 0, 1, 0);
		}

		void draw() const
		{
			glPushMatrix();

			walls.draw();

			glPopMatrix();
		}
	};

}

int xmain()
{
	TunnelStrike::Game game;

	sf::RenderWindow& window = game.getWindow();

	glMatrixMode(GL_PROJECTION);
	glFrustum(0, 1280, 800, 0, 0, 100);

	glMatrixMode(GL_MODELVIEW);

	while (window.isOpen()) {
		glClearColor(0.8f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);


		sf::RectangleShape rect;

		rect.setPosition(sf::Vector2f(100.0f, 100.0f));
		rect.setSize(sf::Vector2f(100.0f, 100.0f));
		rect.setFillColor(sf::Color(200, 100, 255));

		window.draw(rect);


		glMatrixMode(GL_PROJECTION);
		glFrustum(0, 1280, 800, 0, 0, 100);

		glMatrixMode(GL_MODELVIEW);

		game.draw();

#if 0
		window.resetGLStates();

		window.clear();

		window.pushGLStates();
		
		window.draw(game);

		window.popGLStates();
#endif		
		window.display();


		sf::Event event;
		while (window.pollEvent(event)) {
			game.dispatchEvent(event);
		}
	}

	return 0;
}
