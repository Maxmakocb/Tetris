#pragma once

#include "../graphics/graphics.h"
#include <map>
#include <windows.h>
#include <random>
#include <time.h>

namespace gameObjects {
	class Object {
	public:
		geometry::Shape s;
		bool moving = true;

		Object(geometry::Shape s) {
			this->s = s;
		}
		//Object& operator=(Object& obj) {
		//	this->s = obj.s;
		//	this->moving = obj.moving;
		//	return *this;
		//}
	};

	Object hLine(geometry::Point spawnPoint, ANSIColour::Colour colour) {
		geometry::Shape line = geometry::Line(geometry::Point(spawnPoint.x, spawnPoint.y), geometry::Point(spawnPoint.x + 3, spawnPoint.y), ' ', colour);
		return Object(line);
	}

	Object square(geometry::Point spawnPoint, ANSIColour::Colour colour) {
		geometry::Shape line = geometry::Rectangle(geometry::Point(spawnPoint.x, spawnPoint.y), geometry::Point(spawnPoint.x + 1, spawnPoint.y + 1), ' ', false, colour);
		return Object(line);
	}
	Object vLine(geometry::Point spawnPoint, ANSIColour::Colour colour) {
		geometry::Shape line = geometry::Line(geometry::Point(spawnPoint.x, spawnPoint.y), geometry::Point(spawnPoint.x, spawnPoint.y + 3), ' ', colour);
		return Object(line);
	}
	Object LShape(geometry::Point spawnPoint, ANSIColour::Colour colour) {
		geometry::Shape line = geometry::Shape(std::vector<geometry::Point>{geometry::Point(spawnPoint.x, spawnPoint.y), geometry::Point(spawnPoint.x, spawnPoint.y + 1), geometry::Point(spawnPoint.x, spawnPoint.y + 2), geometry::Point(spawnPoint.x + 1, spawnPoint.y + 2)}, ' ', colour);
		return Object(line);
	}
	Object JShape(geometry::Point spawnPoint, ANSIColour::Colour colour) {
		geometry::Shape line = geometry::Shape(std::vector<geometry::Point>{geometry::Point(spawnPoint.x, spawnPoint.y), geometry::Point(spawnPoint.x, spawnPoint.y + 1), geometry::Point(spawnPoint.x, spawnPoint.y + 2), geometry::Point(spawnPoint.x - 1, spawnPoint.y + 2)}, ' ', colour);
		return Object(line);
	}
	Object Snake(geometry::Point spawnPoint, ANSIColour::Colour colour) {
		geometry::Shape line = geometry::Shape(std::vector<geometry::Point>{geometry::Point(spawnPoint.x, spawnPoint.y + 1), geometry::Point(spawnPoint.x + 1, spawnPoint.y + 1), geometry::Point(spawnPoint.x + 1, spawnPoint.y), geometry::Point(spawnPoint.x + 2, spawnPoint.y)}, ' ', colour);
		return Object(line);
	}
	Object Plus(geometry::Point spawnPoint, ANSIColour::Colour colour) {
		geometry::Shape line = geometry::Shape(std::vector<geometry::Point>{geometry::Point(spawnPoint.x, spawnPoint.y + 1), geometry::Point(spawnPoint.x + 1, spawnPoint.y + 1), geometry::Point(spawnPoint.x + 1, spawnPoint.y), geometry::Point(spawnPoint.x + 1, spawnPoint.y + 2), geometry::Point(spawnPoint.x + 2, spawnPoint.y + 1)}, ' ', colour);
		return Object(line);
	}
}

class Game : public Window {
public:
	geometry::Point g = geometry::Point(0, 1);
	geometry::Point left = geometry::Point(-1, 0);
	geometry::Point right = geometry::Point(1, 0);
	geometry::Point down = geometry::Point(0, 1);
	geometry::Line floor = geometry::Line(geometry::Point(0, 22), geometry::Point(11, 22), '#', colours::fg_def);
	geometry::Line lside = geometry::Line(geometry::Point(0, 0), geometry::Point(0, 22), '#', colours::fg_def);
	geometry::Line rside = geometry::Line(geometry::Point(11, 0), geometry::Point(11, 22), '#', colours::fg_def);
	geometry::Line topside = geometry::Line(geometry::Point(0, 0), geometry::Point(0, 22), '#', colours::fg_def);

	std::vector<gameObjects::Object> gameObjs;
	Game() {}
	Game(int width, int height, int font_x = 0, int font_y = 30) : Window(width, height, font_x, font_y) {
		std::srand((unsigned)std::time(NULL));
		this->clear();
		this->start();
		this->run();
	}

	void run() {
		bool quit = false;
		int total = 0;

		bool popStage = false;
		bool moving = true;
		bool controlStage = false;
		while (!quit) {
			bool cumulativeMoving = true;
			for (auto obj : this->gameObjs) {
				cumulativeMoving = cumulativeMoving && obj.moving;
			}
			if (this->gameObjs.size() == 0) {
				cumulativeMoving = false;
			}
			moving = cumulativeMoving;
			if (!moving) {
				popStage = false;
			}

			using namespace std::chrono_literals;

			std::this_thread::sleep_for(20ms);
			total += 20;
			if (total == 100020) {
				total = 20;
			}

			if (GetAsyncKeyState(VK_ESCAPE) & 0x01) {
				return;
			}

			if (!popStage && !controlStage || this->gameObjs.size()==0) {
				int shapeT = rand() % 6;
				int colourT = rand() % 3;
				auto colour = colours::bg_def;
				switch (colourT) {
				case 0:
					colour = colours::bg_blue;
					break;
				case 1:
					colour = colours::bg_red;
					break;
				case 2:
					colour = colours::bg_green;
					break;
				case 3:
					colour = colours::bg_yellow;
					break;
				case 4:
					colour = colours::bg_cyan;
					break;
				}
				switch (shapeT) {
				case 0:
					this->gameObjs.push_back(gameObjects::hLine(geometry::Point(1, 1), colour));
					break;
				case 1:
					this->gameObjs.push_back(gameObjects::square(geometry::Point(4, 1), colour));
					break;
				case 2:
					this->gameObjs.push_back(gameObjects::LShape(geometry::Point(4, 1), colour));
					break; 
				case 3:
					this->gameObjs.push_back(gameObjects::JShape(geometry::Point(4, 1), colour));
					break;
				case 4:
					this->gameObjs.push_back(gameObjects::Snake(geometry::Point(4, 1), colour));
					break;
				case 5:
					this->gameObjs.push_back(gameObjects::Plus(geometry::Point(4, 1), colour));
					break;
				}
			}
			
			if (!popStage) {
				controlStage = true;
				if (GetAsyncKeyState(0x25) & 0x01) {
					if (this->objCanMoveSideways(this->gameObjs[int(gameObjs.size()) - 1], int(gameObjs.size()) - 1, true)) {
						this->gameObjs[gameObjs.size() - 1].s.move(this->left);
					}
				}
				if (GetAsyncKeyState(0x27) & 0x01) {
					if (this->objCanMoveSideways(this->gameObjs[int(gameObjs.size()) - 1], int(gameObjs.size()) - 1, false)) {
						this->gameObjs[gameObjs.size() - 1].s.move(this->right);
					}
				}
				if (GetAsyncKeyState(0x28) & 0x01) {
					if (this->objCanMoveDown(this->gameObjs[int(gameObjs.size()) - 1], int(gameObjs.size()) - 1)) {
							this->gameObjs[gameObjs.size() - 1].s.move(this->down);
					}
				}
				if (GetAsyncKeyState(0x45) & 0x01) {
					if (this->objCanRotate(this->gameObjs[int(gameObjs.size()) - 1], true, int(gameObjs.size()) - 1)) {
							this->gameObjs[gameObjs.size() - 1].s.rotate(true);
					}
				}
				if (GetAsyncKeyState(0x51) & 0x01) {
					if (this->objCanRotate(this->gameObjs[int(gameObjs.size()) - 1], false, int(gameObjs.size()) - 1)) {
							this->gameObjs[gameObjs.size() - 1].s.rotate(false);
					}
				}
			}

			//this->gameShapes.push_back();
			Frame fr = Frame(12, 25);

			if (total % 100 == 0) {
				for (int i = 0; i < gameObjs.size(); i++) {
					if (objCanMoveDown(gameObjs[i], i)) {
						gameObjs[i].s.move(this->g);
						gameObjs[i].moving = true;
					} else {
						gameObjs[i].moving = false;
						if (i == int(gameObjs.size()) - 1) {
							popStage = true;
						}
					}
				}
			}

			for (auto item : this->gameObjs) {
				fr.addShape(item.s);
			}
			fr.addShape(this->floor);
			fr.addShape(this->lside);
			fr.addShape(this->rside);

			if (popStage) {
				controlStage = false;
				std::vector<int> rows;
				for (int i = 3; i < 22; i++) {
					bool full = true;
					for (int j = 1; j < int(fr.cells[i].size()) - 1; j++) {
						full = full && !(fr.cells[i][j].colour == colours::bg_def);
					}
					if (full) {
						rows.push_back(i);
					}
				}
				for (int r : rows) {
					geometry::Line l(geometry::Point(1, r), geometry::Point(10, r), ' ', colours::bg_def);
					int erased = 0;
					int objSize = int(gameObjs.size());
					for (int i = 0; i < objSize; i++) {
						this->gameObjs[i - erased].s.subtract(l);
						if (this->gameObjs[i - erased].s.points.size() == 0) {
							if (this->gameObjs.size() == 1) {
								this->gameObjs.clear();
							}
							else {
								this->gameObjs.erase(this->gameObjs.begin() + (i - erased));
								//bool popStage = false;
							}
							erased++;
						}
					}
				}
				for (int i = 0; i < gameObjs.size(); i++) {
					std::vector<geometry::Shape> collapsed = gameObjs[i].s.collapse();
					for (int k = 0; k < collapsed.size(); k++) {
						gameObjs.push_back(gameObjects::Object(collapsed[k]));
					}
				}
			}

			this->clear();
			while (!this->tryInsertFrame(fr)) {}
		}
	}

	bool objCanMoveDown(gameObjects::Object obj, int ignore) {
		obj.s.move(this->g);
		for (int i = 0; i < this->gameObjs.size(); i++) {
			if (ignore != i && gameObjs[i].s.intersects(obj.s).points.size() != 0) {
				if (!gameObjs[i].moving) {
					return false;
				}
			}
		}

		if (obj.s.intersects(this->floor).points.size() != 0) {
			return false;
		}

		return true;
	}

	bool objCanMoveSideways(gameObjects::Object obj, int ignore, bool left) {
		if (!obj.moving) {
			return false;
		}
		if (left) {
			obj.s.move(this->left);
		}
		else {
			obj.s.move(this->right);
		}
		for (int i = 0; i < this->gameObjs.size(); i++) {
			if (ignore != i && gameObjs[i].s.intersects(obj.s).points.size() != 0) {
				return false;
			}
		}

		if (obj.s.intersects(this->lside).points.size() != 0 || obj.s.intersects(this->rside).points.size() != 0) {
			return false;
		}
		return true;
	}

	bool objCanRotate(gameObjects::Object obj, bool clockwise, int ignore) {
		obj.s.rotate(clockwise);
		for (int i = 0; i < this->gameObjs.size(); i++) {
			if (ignore != i && gameObjs[i].s.intersects(obj.s).points.size() != 0) {
				if (!gameObjs[i].moving) {
					return false;
				}
			}
		}

		if (obj.s.intersects(this->floor).points.size() != 0) {
			return false;
		}

		if (obj.s.intersects(this->lside).points.size() != 0 || obj.s.intersects(this->rside).points.size() != 0) {
			return false;
		}

		if (obj.s.intersects(this->topside).points.size() != 0) {
			return false;
		}

		return true;
	}
};
