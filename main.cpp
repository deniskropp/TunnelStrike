#include <memory>
#include <random>
#include <sstream>
#include <vector>

#include <geometry.hpp>
//#include <parabolic-pointer.hpp>

#include "utils/parameters.hpp"
#include "geometry/camera3d.hpp"
#include "geometry/solid3d.hpp"
#include "geometry/geometry.hpp"

#include "Sfx.hpp"
#include "Shots.hpp"

#include "World.hpp"

//#include <parallel_f/log.hpp>



namespace TunnelStrike {


	class Main
	{
	private:
		sf::RenderWindow& window;
		World world;

		sf::Vector2i tc;
		sf::Vector2i cursor;

	public:
		Main(sf::RenderWindow& window)
			:
			window(window)
		{
			srand((unsigned int)time(NULL));

			cursor.x = sf::Mouse::getPosition(window).x;
			cursor.y = sf::Mouse::getPosition(window).y;

			Camera3d::instance().translate(Vector3d(0, 0, 100.0f));
		}

		void run()
		{
			sf::Clock loop_timer;

			while (window.isOpen()) {
				sf::Time delta = loop_timer.restart();

				ProcessEvents();

				float d = delta.asSeconds();

				while (d > 0.01f) {
					//if (d < 0.1f)
						HandleCamera();

					if (d > 0.01f)
						world.Tick(sf::seconds(0.01f));
					else
						world.Tick(sf::seconds(d));

					d -= 0.01f;
				}

				RenderFrame();

				// other
				Parameters::print_mean_CPU_usage(std::cout, delta.asMilliseconds());

//				sf::sleep(sf::milliseconds((sf::Int32)(MAX_MAIN_LOOP_DURATION - (double)delta.asMilliseconds())));
			}
		}

	private:
		void ProcessEvents()
		{
			//LOG_DEBUG("Main::ProcessEvents()\n");

			sf::Event event;

			// handle events
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					//LOG_DEBUG("Main::ProcessEvents() -> CLOSE\n");

					window.close();
				}
				else if (event.type == sf::Event::Resized) {
					//LOG_DEBUG("Main::ProcessEvents() -> Window resized (%ux%u)\n", event.size.width, event.size.height);

					window.setView(sf::View(sf::FloatRect(sf::Vector2f(0.0f, 0.0f),
									      sf::Vector2f((float)event.size.width, (float)event.size.height))));

					Parameters::update_window_size(event.size.width, event.size.height);

					Camera3d::instance().reload_frustrum();
				}
				else if (event.type == sf::Event::MouseButtonPressed || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					//LOG_DEBUG("Main::ProcessEvents() -> FIRE\n");

					Shoot();
				}
			}
		}

		void Shoot()
		{
			Vector3d pos(0, 0, Camera3d::instance().center().get_z());
			Vector3d dir(0, 0, 100);

			dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), tc.x / 4.0f);
			dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -tc.y / 4.0f);

			world.shots->shots.push_back(std::make_shared<Shot>(pos, dir));

			Sfx::instance().PlayShot();
		}

		bool CheckShoot()
		{
			Vector3d pos(0, 0, Camera3d::instance().center().get_z());
			Vector3d dir(0, 0, 100);

			dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), tc.x / 4.0f);
			dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -tc.y / 4.0f);

			for (float a = 0.0f; a < 100.0f; a += 0.3f) {
				Vector3d current = pos + dir * a;

				for (auto t : world.targets->targets) {
					if (current.distance_to(t->GetCenter()) < 5.0f)
						return true;
				}
			}

			return false;
		}

		void HandleCamera()
		{
			// move camera
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
				Camera3d::instance().move(Camera3d::DIRECTION::FRONT);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				Camera3d::instance().move(Camera3d::DIRECTION::BACK);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				Camera3d::instance().move(Camera3d::DIRECTION::RIGHT);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				Camera3d::instance().move(Camera3d::DIRECTION::LEFT);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				Camera3d::instance().move(Camera3d::DIRECTION::UP);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
				Camera3d::instance().move(Camera3d::DIRECTION::DOWN);

			int dx = (sf::Mouse::getPosition(window).x - cursor.x);
			int dy = (sf::Mouse::getPosition(window).y - cursor.y);

			cursor.x += dx;
			cursor.y += dy;

			if (dx > 0) {
				if (tc.x + dx > 70)
					dx = 70 - tc.x;
			}
			else if (dx < 0) {
				if (tc.x + dx < -70)
					dx = -70 - tc.x;
			}

			if (dy > 0) {
				if (tc.y + dy > 70)
					dy = 70 - tc.y;
			}
			else if (dy < 0) {
				if (tc.y + dy < -70)
					dy = -70 - tc.y;
			}


#if 1
			static int sx = 1;
			static int sy = 1;

			if (::rand() % 80 == 0)
				sx = -sx;

			if (::rand() % 80 == 0)
				sy = -sy;

			if (tc.x > 5) {
				sx = -1;
			}

			if (tc.x < -5) {
				sx = 1;
			}

			if (tc.y > 5) {
				sy = -1;
			}

			if (tc.y < -5) {
				sy = 1;
			}

			dx += sx;
			dy += sy;
#else

			if (!world.targets->targets.empty()) {
				auto target = world.targets->targets.back();

				Vector3d pos(0, 0, 0);
				Vector3d dir(0, 0, 50);

				dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), tc.x / 4.0f);
				dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -tc.y / 4.0f);

				polymer::float3 v_s_h(dir.get_x(), 0, 1);
				polymer::float3 v_s_v(0, dir.get_y(), 1);
				polymer::float3 v_t_h(target->GetCenter().get_x(), 0, 1);
				polymer::float3 v_t_v(0, target->GetCenter().get_y(), 1);

				std::cout << "v_s_h " << v_s_h.x << std::endl;
				std::cout << "v_t_h " << v_t_h.x << std::endl;

				float a_h = polymer::angle_between(v_s_h, v_t_h, polymer::float3());
				float a_v = polymer::angle_between(v_s_v, v_t_v, polymer::float3());

				std::cout << "a_h " << a_h << std::endl;
				std::cout << "a_v " << a_v << std::endl;


				if (v_s_h.x - v_t_h.x < 0) {
					dx = 1;
				}
				else {
					dx = -1;
				}

				if (v_s_h.y - v_t_h.y < 0) {
					dy = 1;
				}
				else {
					dy = -1;
				}
			}
#endif			
			if (dx || dy) {
				tc.x += dx;
				tc.y += dy;

				// rotate camera
				Camera3d::instance().rotate((float)dx, (float)dy);
			}

			Camera3d::instance().translate(Vector3d(0, 0, 0.3f));


			static int sh = 0;

			if (!sh) {
				if (CheckShoot()) {
					sh = 10;

					Shoot();
				}
			}
			else
				sh--;
		}

		void RenderFrame()
		{
			window.clear();

			window.draw(world);

			updateStats();
			window.draw(stats);

			window.display();
		}

	private:
		class Stats : public sf::Text
		{
		private:
			sf::Font font;

		public:
			Stats()
			{
				if (!font.loadFromFile("calibri.ttf"))
					throw std::runtime_error("font loading failed");

				setFont(font);
				setCharacterSize(48);
				setFillColor(sf::Color::White);
				setPosition(sf::Vector2f(20.0f, 20.0f));
			}
		};

		void updateStats()
		{
			std::stringstream ss;

			ss << "Kills: " << world.get_kills();

			stats.setString(ss.str());
		}

		Stats stats;
	};


}


int main()
{
	// setup window
	sf::ContextSettings window_settings;

	window_settings.antialiasingLevel = 8;

	sf::RenderWindow window(sf::VideoMode(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT), "TunnelStrike", sf::Style::Close | sf::Style::Resize, window_settings);

	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
	window.setMouseCursorGrabbed(true);
	window.setMouseCursorVisible(false);

	TunnelStrike::Main main(window);

	main.run();

	return 0;
}
