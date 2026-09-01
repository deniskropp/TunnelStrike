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
#include "evo/PilotMind.hpp"
#include "sim/SimClock.hpp"

namespace TunnelStrike
{

	struct Options
	{
		bool help = false;
		bool evo_verify = false;
		bool self_run = false;
		bool headless = false;
		int ticks = 0;
		std::string evo_dir = "evo";
	};

	static void printUsage()
	{
		std::cout
			<< "TunnelStrike\n"
			<< "  (no flags)                 interactive play; existing auto-fire assist\n"
			<< "  --self-run                 windowed self-running mode with evolving pilot AI\n"
			<< "  --self-run --headless [N]  headless self-run for N ticks (default 18000)\n"
			<< "  --evo-verify [dir] [N]     creature GA verify (no window)\n"
			<< "  --evo-dir DIR              archive directory (default evo)\n"
			<< "  --ticks N                  stop after N sim ticks (windowed or headless)\n"
			<< "  --help                     this text\n";
	}

	static Options parseOptions(int argc, char **argv)
	{
		Options opt;
		for (int i = 1; i < argc; ++i) {
			const std::string a = argv[i];
			if (a == "--help" || a == "-h") {
				opt.help = true;
			} else if (a == "--evo-verify") {
				opt.evo_verify = true;
				opt.headless = true;
			} else if (a == "--self-run" || a == "--self-running") {
				opt.self_run = true;
			} else if (a == "--headless") {
				opt.headless = true;
			} else if (a == "--ticks" && i + 1 < argc) {
				opt.ticks = std::atoi(argv[++i]);
			} else if (a == "--evo-dir" && i + 1 < argc) {
				opt.evo_dir = argv[++i];
			} else if (!a.empty() && a[0] != '-') {
				char *end = nullptr;
				const long n = std::strtol(a.c_str(), &end, 10);
				if (end && *end == '\0' && n > 0)
					opt.ticks = static_cast<int>(n);
				else
					opt.evo_dir = a;
			}
		}
		if (opt.headless && opt.ticks <= 0)
			opt.ticks = 18000;
		return opt;
	}

	class Main
	{
	private:
		sf::RenderWindow &window;
		World world;
		SimClock clock;
		bool self_run = false;
		unsigned long long max_ticks = 0;
		std::unique_ptr<PilotMind> mind;

		sf::Vector2i cursor;
		float aimx = 0.0f;
		float aimy = 0.0f;
		int assist_cooldown = 0;

		sf::Clock time;

	public:
		Main(sf::RenderWindow &window, bool self_run, const std::string &evo_dir,
			unsigned long long max_ticks)
			: window(window)
			, world(evo_dir)
			, self_run(self_run)
			, max_ticks(max_ticks)
		{
			srand((unsigned int)::time(NULL));

			cursor.x = sf::Mouse::getPosition(window).x;
			cursor.y = sf::Mouse::getPosition(window).y;

			Camera3d::instance().translate(Vector3d(0, 0, 100.0f));

			if (self_run)
				mind = std::make_unique<PilotMind>(world.store());
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
					HandleCamera(step);
					world.Tick(step);
				});
				clock.noteSteps(steps);

				RenderFrame();

				Parameters::print_mean_CPU_usage(std::cout, delta.asMilliseconds());

				if (max_ticks > 0 && clock.totalSteps() >= max_ticks)
					window.close();
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
				else if (!self_run && (event.type == sf::Event::MouseButtonPressed
					|| sf::Keyboard::isKeyPressed(sf::Keyboard::Space)))
				{
					Shoot();
				}
			}
		}

		void Shoot()
		{
			Vector3d pos(0, 0, Camera3d::instance().center().get_z());
			Vector3d dir(0, 0, 100);

			dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), aimx / 4.0f);
			dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -aimy / 4.0f);

			world.fire(pos, dir);

			Sfx::instance().PlayShot();
		}

		bool CheckShoot()
		{
			Vector3d pos(0, 0, Camera3d::instance().center().get_z());
			Vector3d dir(0, 0, 100);

			dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), aimx / 4.0f);
			dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -aimy / 4.0f);

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

		void HandleCamera(sf::Time step)
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

			if (self_run && mind) {
				const float before_x = aimx;
				const float before_y = aimy;
				if (mind->step(step, world, aimx, aimy))
					Shoot();
				Camera3d::instance().rotate(aimx - before_x, aimy - before_y);
				Camera3d::instance().translate(Vector3d(0, 0, 0.04f));
				return;
			}

			int dx = (sf::Mouse::getPosition(window).x - cursor.x);
			int dy = (sf::Mouse::getPosition(window).y - cursor.y);

			cursor.x += dx;
			cursor.y += dy;

			if (aimx + dx > 70)
				dx = static_cast<int>(70 - aimx);
			if (aimx + dx < -70)
				dx = static_cast<int>(-70 - aimx);
			if (aimy + dy > 70)
				dy = static_cast<int>(70 - aimy);
			if (aimy + dy < -70)
				dy = static_cast<int>(-70 - aimy);

			if (dx || dy)
			{
				aimx += static_cast<float>(dx);
				aimy += static_cast<float>(dy);

				Camera3d::instance().rotate((float)dx, (float)dy);
			}

			Camera3d::instance().translate(Vector3d(0, 0, 0.04f));

			if (!assist_cooldown)
			{
				if (CheckShoot())
				{
					assist_cooldown = 40;

					Shoot();
				}
			}
			else
				assist_cooldown--;
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
			if (mind) {
				ss << "  Pilot: " << mind->generation()
				   << "  PBest: " << mind->lastBestFitness();
			}

			stats.setString(ss.str());
		}

		Stats stats;
	};

	static int runTickLoop(World &world, bool use_pilot, int ticks, const char *tag)
	{
		Camera3d::instance().translate(Vector3d(0, 0, 100.0f));

		SimClock clock;
		std::unique_ptr<PilotMind> mind;
		float aimx = 0.0f;
		float aimy = 0.0f;
		if (use_pilot)
			mind = std::make_unique<PilotMind>(world.store());

		const unsigned loaded_creatures = world.generation();
		const unsigned loaded_pilot = mind ? mind->generation() : 0;

		for (int i = 0; i < ticks; ++i) {
			Camera3d::instance().translate(Vector3d(0, 0, 0.04f));

			if (mind) {
				const sf::Time step = clock.step();
				if (mind->step(step, world, aimx, aimy)) {
					Vector3d pos(0, 0, Camera3d::instance().center().get_z());
					Vector3d dir(0, 0, 100);
					dir.rotate(Vector3d(0, 0, 0), Vector3d(0, 1, 0), aimx / 4.0f);
					dir.rotate(Vector3d(0, 0, 0), Vector3d(1, 0, 0), -aimy / 4.0f);
					world.fire(pos, dir);
				}
				world.Tick(step);
			} else {
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
			}
			clock.noteSteps(1);
		}

		world.store().snapshotWorld(world, world.evo(), clock.totalSteps());

		std::cout << tag
			<< " dir=" << world.store().directory()
			<< " ticks=" << clock.totalSteps()
			<< " kills=" << world.get_kills()
			<< " misses=" << world.get_misses()
			<< " generation=" << world.generation()
			<< " pending=" << world.evo().scoredPending()
			<< " best=" << world.evo().lastBestFitness();
		if (mind) {
			std::cout << " pilot_gen=" << mind->generation()
				<< " pilot_pending=" << mind->scoredPending()
				<< " pilot_best=" << mind->lastBestFitness();
		}
		std::cout << std::endl;

		if (use_pilot && mind->generation() == loaded_pilot) {
			std::cerr << tag << " failed: pilot generation did not advance\n";
			return 1;
		}
		if (!use_pilot && world.generation() == loaded_creatures) {
			std::cerr << tag << " failed: generation did not advance\n";
			return 1;
		}

		return 0;
	}

	int runEvoVerify(const std::string &dir, int ticks)
	{
		World world(dir);
		return runTickLoop(world, false, ticks, "game-evo-verify");
	}

	int runSelfRunHeadless(const std::string &dir, int ticks)
	{
		World world(dir);
		return runTickLoop(world, true, ticks, "self-run");
	}

}

int main(int argc, char **argv)
{
	const TunnelStrike::Options opt = TunnelStrike::parseOptions(argc, argv);

	if (opt.help) {
		TunnelStrike::printUsage();
		return 0;
	}

	if (opt.evo_verify)
		return TunnelStrike::runEvoVerify(opt.evo_dir, opt.ticks > 0 ? opt.ticks : 18000);

	if (opt.self_run && opt.headless)
		return TunnelStrike::runSelfRunHeadless(opt.evo_dir, opt.ticks);

	sf::ContextSettings window_settings;

	window_settings.antialiasingLevel = 8;

	sf::RenderWindow window(TunnelStrike::make_video_mode(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT), "TunnelStrike", sf::Style::Close | sf::Style::Resize, window_settings);

	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
	window.setMouseCursorGrabbed(!opt.self_run);
	window.setMouseCursorVisible(opt.self_run);

	TunnelStrike::Main main(window, opt.self_run, opt.evo_dir,
		opt.ticks > 0 ? static_cast<unsigned long long>(opt.ticks) : 0ull);

	main.run();

	return 0;
}
