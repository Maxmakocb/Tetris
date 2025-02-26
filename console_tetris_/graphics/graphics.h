#pragma once
#include <iostream>
#include <vector>
#include <condition_variable>
#include <thread>
#include <atomic>


// ANSIColor Source: https://www.quora.com/In-C-how-do-I-print-out-text-at-different-font-colors-on-the-terminal#:~:text=cout%20%3C%3C%20%22Hello%20%22%3B,the%20color%20back%20to%20white.
namespace ANSIColour
{
    enum Code
    {
        FG_RED = 31,
        FG_GREEN = 32,
        FG_BLUE = 34,
        FG_DEFAULT = 39,

        BG_RED = 41,
        BG_GREEN = 42,
        BG_YELLOW = 43,
        BG_BLUE = 44,
        BG_WHITE = 47,
        BG_DEFAULT = 49,
        BG_CYAN = 106,
    };

    class Colour {
    private:
        Code code;
    public:
        Colour(Code _code) : code(_code) {}

        friend std::ostream& operator<<(std::ostream& os, const Colour& c)
        {
            return os << "\033[" << c.code << "m";
        }
        bool operator==(const Colour& other) {
            return this->code == other.code;
        }
    };

}

namespace colours {
    const ANSIColour::Colour bg_red(ANSIColour::BG_RED);
    const ANSIColour::Colour bg_green(ANSIColour::BG_GREEN);
    const ANSIColour::Colour bg_blue(ANSIColour::BG_BLUE);
    const ANSIColour::Colour bg_def(ANSIColour::BG_DEFAULT);
    const ANSIColour::Colour bg_white(ANSIColour::BG_WHITE);
    const ANSIColour::Colour bg_yellow(ANSIColour::BG_YELLOW);
    const ANSIColour::Colour bg_cyan(ANSIColour::BG_CYAN);

    const ANSIColour::Colour fg_def(ANSIColour::FG_DEFAULT);
}

// Cell is a single cell data for a frame structure
class Cell {
public: 
    char value;
    ANSIColour::Colour colour = colours::bg_def;

    Cell(char value, ANSIColour::Colour colour = colours::bg_def) : value(value), colour(colour) {}

    friend std::ostream& operator<<(std::ostream& os, Cell& c) {
        return os << c.colour << c.value << colours::bg_def;
    }

    Cell& operator=(Cell& other) {
        this->value = other.value;
        this->colour = other.colour;
        return *this;
    }
};

namespace geometry {
    struct Point {
        int x;
        int y;
        Point(int x = 0, int y = 0) : x(x), y(y) {}
        bool operator==(Point& other) {
            return this->x == other.x && this->y == other.y;
        }

        Point vector(const Point& other) {
            return Point(other.x - this->x, other.y - this->y);
        }

        std::vector<Point> expand();
    };

    class Shape {
    public:
        char symbol = ' ';
        std::vector<Point> points = {};
        ANSIColour::Colour colour = colours::bg_def;
        Shape() {}
        Shape(std::vector<Point> points, char symbol, ANSIColour::Colour colour) {
            this->points = points;
            this->colour = colour;
            this->symbol = symbol;
        }

        geometry::Shape intersects(Shape& other);
        void rotate(bool clockwise);

        void move(Point offset);
        void subtract(Shape other);
        void merge(const Shape& shape);

        std::vector<Shape> collapse();
    };

    class Rectangle : public Shape {
    public:
        Rectangle(Point p1, Point p2, char symbol, bool fill, ANSIColour::Colour colour);
    };

    class Line : public Shape {
    public:
        Line(Point p1, Point p2, char symbol, ANSIColour::Colour colour);
    };

    class Dot : public Shape {
    public:
        Dot(Point p1, char symbol, ANSIColour::Colour colour) {
            this->points.push_back(p1);
            this->symbol = symbol;
            this->colour = colour;
        }
    };
}

class Frame {
public:
    int width;
	int height;
    std::vector<std::vector<Cell>> cells;
    
    ~Frame();
    Frame(int width = 10, int height = 20);

    void render();
    void addShape(geometry::Shape& sh);
    Frame& operator=(Frame& other) {
        this->width = other.width;
        this->height = other.height;
        this->cells = other.cells;
        return *this;
    }
};

class Window {
private:
    std::thread* render_thread = NULL;
    std::condition_variable cv;
    std::mutex mu;
    Frame* frame;
    std::atomic<bool> signal = false;

    void setCursorVisibility(bool visible);
    void setWindowSize(int width, int height, int font_x = 0, int font_y = 30);
public:

    ~Window();
    Window();
    Window(int width, int height, int font_x = 0, int font_y = 30);

    void clear();
    void render();
    void start();
    bool tryInsertFrame(Frame frame);
};
