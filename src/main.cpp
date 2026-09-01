#include <cstdlib>
#include <iostream>
#include <memory>
#include <random>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "utils/parameters.hpp"
#include "geometry/camera3d.hpp"
#include "geometry/solid3d.hpp"
#include "geometry/geometry.hpp"

#include "Sfx.hpp"
#include "Shots.hpp"

#include "World.hpp"
#include "sim/SimClock.hpp"

namespace TunnelStrike
{

	class Main
	{
	private:
		sf::RenderWindow &window;
		World world;
		SimClock clock;

		sf::Vector2i tc;
		sf::Vector2i cursor;

		sf::Clock time;

	public:
		Main(sf::RenderWindow &window)
			: window(window)
		{
			srand((unsigned int)::time(NULL));

			cursor.x = sf::Mouse::getPosition(window).x;
			cursor.y = sf::Mouse::getPosition(window).y;

			Camera3d::instance().translate(Vector3d(0, 0, 100.0f));
		}

		void ExportWorld()
		{
			world.store().snapshotWorld(world, world.evo(), clock.totalSteps());
		}

		void run()
		{
			sf::Clock loop_timer;

			while (window.isOpen())
			{
				sf::Time delta = loop_timer.restart();

				const int steps = clock.absorb(delta, [this](sf::Time step) {
					ProcessEvents();
					HandleCamera();
					world.Tick(step);
				});
				clock.noteSteps(steps);

				RenderFrame();

				Parameters::print_mean_CPU_usage(std::cout, delta.asMilliseconds());
			}
		}

	private:
		void ProcessEvents()
		{
			sf::Event event;

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				{
					window.close();
				}
				else if (event.type == sf::Event::Resized)
				{
					window.setView(sf::View(sf::FloatRect(sf::Vector2f(0.0f, 0.0f),
														  sf::Vector2f((float)event.size.width, (float)event.size.height))));

					Parameters::update_window_size(event.size.width, event.size.height);

					Camera3d::instance().reload_frustrum();
				}
				else if (event.type == sf::Event::MouseButtonPressed || sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
				{
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

			world.fire(pos, dir);

			Sfx::instance().PlayShot();
		}

		bool CheckShoot()
		{
			Vector3d pos(0, 0, Camera3d::instance().center().get_z());
			Vector3d dir(0, 0, 100);

			dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), tc.x / 4.0f);
			dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -tc.y / 4.0f);

			for (float a = 0.0f; a < 100.0f; a += 0.3f)
			{
				Vector3d current = pos + dir * a;

				for (auto t : world.targets->targets)
				{
					if (current.distance_to(t->GetCenter()) < 5.0f)
						return true;
				}
			}

			return false;
		}

		void HandleCamera()
		{
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

			if (tc.x + dx > 70)
				dx = 70 - tc.x;
			if (tc.x + dx < -70)
				dx = -70 - tc.x;
			if (tc.y + dy > 70)
				dy = 70 - tc.y;
			if (tc.y + dy < -70)
				dy = -70 - tc.y;

			if (dx || dy)
			{
				tc.x += dx;
				tc.y += dy;

				Camera3d::instance().rotate((float)dx, (float)dy);
			}

			Camera3d::instance().translate(Vector3d(0, 0, 0.04f));

			static int sh = 0;

			if (!sh)
			{
				if (CheckShoot())
				{
					sh = 40;

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
				if (!font.loadFromFile("data/calibri.ttf"))
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

			ss << "Kills: " << world.get_kills()
			   << "  Gen: " << world.generation()
			   << "  Best: " << world.evo().lastBestFitness();

			stats.setString(ss.str());
		}

		Stats stats;
	};

	int runEvoVerify(const std::string &dir, int ticks)
	{
		Camera3d::instance().translate(Vector3d(0, 0, 100.0f));

		World world(dir);
		SimClock clock;

		const unsigned loaded_gen = world.generation();
		const unsigned loaded_pool = static_cast<unsigned>(world.evo().pool().size());

		for (int i = 0; i < ticks; ++i) {
			Camera3d::instance().translate(Vector3d(0, 0, 0.04f));

			if (i % 20 == 0) {
				const Vector3d origin(0, 0, Camera3d::instance().center().get_z());
				for (const Vector3d &center : world.liveTargetCenters()) {
					Vector3d dir = center - origin;
					if (dir.norm() < 1e-6)
						continue;
					dir.normalize();
					dir *= 100.0;
					world.fire(origin, dir);
				}
			}

			world.Tick(clock.step());
			clock.noteSteps(1);
		}

		world.store().snapshotWorld(world, world.evo(), clock.totalSteps());

		std::cout << "game-evo-verify dir=" << dir
			<< " loaded_gen=" << loaded_gen
			<< " loaded_pool=" << loaded_pool
			<< " ticks=" << clock.totalSteps()
			<< " kills=" << world.get_kills()
			<< " generation=" << world.generation()
			<< " pending=" << world.evo().scoredPending()
			<< " best=" << world.evo().lastBestFitness()
			<< " mean=" << world.evo().lastMeanFitness()
			<< " diversity=" << world.evo().lastDiversity()
			<< std::endl;

		if (loaded_pool == 0)
			std::cerr << "game-evo-verify note: empty archive, started from random pool\n";

		if (world.generation() == loaded_gen) {
			std::cerr << "game evo verify failed: generation did not advance\n";
			return 1;
		}

		return 0;
	}

}

int main(int argc, char **argv)
{
	if (argc >= 2 && std::string(argv[1]) == "--evo-verify") {
		const std::string dir = argc >= 3 ? argv[2] : "evo";
		const int ticks = argc >= 4 ? std::atoi(argv[3]) : 18000;
		return TunnelStrike::runEvoVerify(dir, ticks);
	}

	sf::ContextSettings window_settings;

	window_settings.antialiasingLevel = 8;

	sf::RenderWindow window(TunnelStrike::make_video_mode(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT), "TunnelStrike", sf::Style::Close | sf::Style::Resize, window_settings);

	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
	window.setMouseCursorGrabbed(true);
	window.setMouseCursorVisible(false);

	TunnelStrike::Main main(window);

	main.run();

	return 0;
}
