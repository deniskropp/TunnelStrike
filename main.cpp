#include <memory>
#include <vector>

#include <Instance.hxx>

#include "utils/mouse.hpp"
#include "utils/parameters.hpp"
#include "geometry/camera3d.hpp"
#include "geometry/solid3d.hpp"
#include "geometry/geometry.hpp"

#include "Sfx.hpp"
#include "Shots.hpp"

#include "World.hpp"

#include <parallel_f/log.hpp>



namespace TunnelStrike {


	class TestObject : public Instance<TestObject>
	{
	public:
		TestObject()
		{
			LOG_INFO("TestObject::TestObject()\n");
		}

		~TestObject()
		{
			LOG_INFO("TestObject::~TestObject()\n");
		}

		void test()
		{
			LOG_INFO("TestObject::test()\n");
		}
	};




	class Main
	{
	private:
		sf::RenderWindow& window;
		sf::Vector2i tc;
		World world;

	public:
		Main(sf::RenderWindow& window)
			:
			window(window)
		{
			srand((unsigned int)time(NULL));


			TestObject::Handle obj(new TestObject());

			obj->test();

			TestObject::Handle obj2(obj);

			TestObject::Handle obj3;

			obj3 = obj2;
		}

		void run()
		{
			sf::Clock loop_timer;

			while (window.isOpen()) {
				ProcessEvents();

				HandleCamera();

				world.Tick();

				RenderFrame();

				// other
				Parameters::print_mean_CPU_usage(std::cout, loop_timer.getElapsedTime().asMilliseconds());

				sf::sleep(sf::milliseconds((sf::Int32)(MAX_MAIN_LOOP_DURATION - (double)loop_timer.getElapsedTime().asMilliseconds())));
				loop_timer.restart();
			}
		}

	private:
		void ProcessEvents()
		{
			LOG_DEBUG("Main::ProcessEvents()\n");

			sf::Event event;

			// handle events
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
					LOG_DEBUG("Main::ProcessEvents() -> CLOSE\n");

					window.close();
				}
				else if (event.type == sf::Event::Resized) {
					LOG_DEBUG("Main::ProcessEvents() -> Window resized (%ux%u)\n", event.size.width, event.size.height);

					window.setView(sf::View(sf::FloatRect(0, 0, (float)event.size.width, (float)event.size.height)));

					Parameters::update_window_size(event.size.width, event.size.height);

					Camera3d::instance().reload_frustrum();
				}
				else if (event.type == sf::Event::MouseButtonPressed || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
					LOG_DEBUG("Main::ProcessEvents() -> FIRE\n");

					Vector3d pos(0, 0, Camera3d::instance().center().get_z());
					Vector3d dir(0, 0, 50);

					dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), tc.x / 4.0f);
					dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -tc.y / 4.0f);

					world.shots->shots.push_back(std::make_shared<Shot>(pos, dir));

					Sfx::instance().PlayShot();
				}
			}
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

			int dx = Mouse::get_move_x(window);
			int dy = Mouse::get_move_y(window);

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

			if (dx || dy) {
				tc.x += dx;
				tc.y += dy;

				// rotate camera
				Camera3d::instance().rotate((float)dx, (float)dy);
			}

			Camera3d::instance().translate(Vector3d(0, 0, 1.0));
		}

		void RenderFrame()
		{
			window.clear();

			window.draw(world, sf::RenderStates::Default);

			// flip display
			window.display();
		}
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
