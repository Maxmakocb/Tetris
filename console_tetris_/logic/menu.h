#pragma once

#include "game.h"
#include <map>
#include <chrono>
#include <iostream>
#include <thread>
#include <functional>

#include <windows.h>

namespace menu_objects {
	class Object {
	public:
		int id = 0;
		std::string name;
		geometry::Point ltop;
		geometry::Point rbot;
		std::map<std::string, geometry::Shape> shapes;
		char symbol = ' ';
		ANSIColour::Colour colour = colours::bg_def;

		Object() {}
		Object& operator=(Object& other) {
			return other;
		}
	};

	class Button: public Object {
		// time in ms
		std::function<void()> f;
		const int updateFreq = 2000;
	public:
		Button() {}
		Button(int id, std::string name, geometry::Point ltop, geometry::Point rbot, ANSIColour::Colour colour, char symbol = ' ') {
			this->id = id;
			this->name = name;
			this->ltop = ltop;
			this->rbot = rbot;
			this->symbol = symbol;
			this->colour = colour;

			geometry::Rectangle r = geometry::Rectangle(ltop, rbot, symbol, false, colour);
			this->shapes["outline"] = r;

			this->updateName();
		}

		void updatehighlight(bool highlited) {
			if (this->shapes.find("highlight") == this->shapes.end() && highlited) {
				auto tempLtop = geometry::Point(this->ltop.x + 1, this->ltop.y + 1);
				auto tempRbot = geometry::Point(this->rbot.x - 1, this->rbot.y - 1);
				geometry::Rectangle r = geometry::Rectangle(tempLtop, tempRbot, this->symbol, false, colours::bg_white);
				this->shapes["highlight"] = r;

				this->updateName();
			}
			else if (this->shapes.find("highlight") != this->shapes.end() && !highlited) {
				this->shapes.erase("highlight");
			}
		}

		void updateName() {
			int offset = 0;
			for (auto c : name) {
				std::stringstream letter_name = {};
				letter_name << "name_letter_" << offset;
				this->shapes[letter_name.str()] = geometry::Dot(geometry::Point((ltop.x + rbot.x) / 2 - int(name.size()) / 2 + offset++, (ltop.y + rbot.y) / 2), c, colours::fg_def);
			}
		}

		void addInteraction(std::function<void()> f) {
			this->f = f;
		}

		void interact() {
			this->f();
		}
	};
}

class Menu : public Window {
public:
	std::vector<menu_objects::Button> objects;

	Menu(int x = 0, int y = 0) : Window(x, y) {}
	void run() {
		//400px = 26 symbols
		auto b1 = menu_objects::Button(0, "start", geometry::Point(5, 2), geometry::Point(20, 6), colours::bg_blue);
		auto b2 = menu_objects::Button(1, "high scores", geometry::Point(5, 8), geometry::Point(20, 12), colours::bg_blue);
		auto b3 = menu_objects::Button(2, "quit", geometry::Point(5, 14), geometry::Point(20, 18), colours::bg_blue);
		bool quit = false;
		b1.addInteraction([&] {
			auto g = new Game(400, 600, 30, 20); 
		});
		b2.addInteraction([&] {return; });
		b3.addInteraction([&] {quit = true; });
		objects.push_back(b1);
		objects.push_back(b2);
		objects.push_back(b3);

		int total = 0;
		bool highlight = false;
		int selected = 0;
		while (!quit) {
			using namespace std::chrono_literals;

			std::this_thread::sleep_for(20ms);
			if (GetAsyncKeyState(VK_ESCAPE) & 0x01) {
				return;
			}

			// Down, S key
			if (GetAsyncKeyState(0x28) & 0x01) {
				if (selected < 3) {
					this->objects[selected].updatehighlight(false);
					selected += 1;
				}
			}
			if (GetAsyncKeyState(0x26) & 0x01) {
				if (selected > 0) {
					this->objects[selected].updatehighlight(false);
					selected -= 1;
				}
			}

			total += 10;
			if (total == 100010) {
				total = 10;
			}

			Frame fr = Frame(26, 24);
			if (total % 100 == 0) {
				this->objects[selected].updatehighlight(highlight);
				highlight = !highlight;
			}

			for (auto obj : this->objects) {
				for (auto shape : obj.shapes) {
					fr.addShape(shape.second);
				}
			}
			this->clear();
			while (!this->tryInsertFrame(fr)) {}

			if (GetAsyncKeyState(VK_RETURN)) {
				this->objects[selected].interact();
			}
		}
	}
};