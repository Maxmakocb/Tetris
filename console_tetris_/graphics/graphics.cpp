#include "graphics.h"
#include <cstdlib>
#include <sstream>
#include<algorithm>
#include <iostream>
// necessary for GetConsoleWindow to work
#define _WIN32_WINNT 0x0500

#include <windows.h>

#include "../lib/tree.h"

Window::~Window()
{
    if (this->render_thread != NULL) {
        this->render_thread->join();
    }
}

Window::Window() {
    setCursorVisibility(false);
}

Window::Window(int width, int height, int font_x, int font_y)
{
    this->setWindowSize(width, height, font_x, font_y);
    this->setCursorVisibility(false);
}

void Window::setCursorVisibility(bool visible) {
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    // set the cursor visibility
    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(out, &cursorInfo);
}

void Window::setWindowSize(int width, int height, int font_x, int font_y) {
    /*
    * This code upscales the fond in the console.
    * Source: https://www.purebasic.fr/english/viewtopic.php?t=75649
    */
    CONSOLE_FONT_INFOEX cfi;
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = font_x;                   // Width of each character in the font
    cfi.dwFontSize.Y = font_y;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    wcscpy_s(cfi.FaceName, L"Consolas"); // Choose your font
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), FALSE, &cfi);

    int centerX = GetSystemMetrics(SM_CXSCREEN)/2;
    int centerY = GetSystemMetrics(SM_CYSCREEN)/2;

    HWND hwnd = GetConsoleWindow();
    SetWindowPos(
        hwnd, 
        NULL, 
        centerX - width/2,
        centerY - height/2, 
        width, 
        height, 
        NULL
    );
}

void Window::clear() {
	std::system("cls");
}

void Window::render()
{
    while (true) {
        std::unique_lock<std::mutex> lk(this->mu);
        this->cv.wait(lk, [&] {return this->signal ? true: false; });
        if (this->frame != NULL) {
            this->frame->render();
            delete this->frame;
            this->frame = NULL;
        }
        this->signal = false;
    }
    //std::lock_guard<std::mutex>(this->mu);
}

void Window::start()
{
    std::thread* t = new std::thread([&] {this->render(); });
    this->render_thread = std::move(t);
    t = NULL;
}

bool Window::tryInsertFrame(Frame frame)
{
    std::unique_lock<std::mutex> lock(this->mu);
    if (this->frame != NULL) {
        return false;
    }
    this->frame = new Frame();
    *this->frame = frame;
    this->signal = true;
    this->cv.notify_one();
}

geometry::Rectangle::Rectangle(Point p1, Point p2, char symbol, bool fill, ANSIColour::Colour colour) {
    this->symbol = symbol;
    this->colour = colour;

    if (fill) {
        for (int x = p1.x; x <= p2.x; x++) {
            for (int y = p1.y; y <= p2.y; y++) {
                this->points.push_back(Point(x, y));
            }
        }
        return;
    }

    for (int x = p1.x; x <= p2.x; x++) {
        for (int y : {p1.y, p2.y}) {
            this->points.push_back(Point(x, y));
        }
        if (x == p1.x || x == p2.x) {
            for (int y = p1.y + 1; y <= p2.y - 1; y++) {
                this->points.push_back(Point(x, y));
            }
        }
    }
}

Frame::~Frame()
{
    for (int i = 0; i < this->height; i++) {
        //for (int j = 0; i < this->width; j++) {
        //    delete this->cells[i][j];
        //}
        this->cells[i].clear();
    }
    this->cells.clear();
}

Frame::Frame(int width, int height)
{
    this->width = width;
    this->height = height;

    for (int i = 0; i < this->height; i++) {
        this->cells.push_back(std::vector<Cell>{});
        for (int j = 0; j < this->width; j++) {
            this->cells[i].push_back(Cell(' ', colours::bg_def));
        }
    }
}

void Frame::render()
{
    std::stringstream buffer;
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            Cell cell = this->cells[i][j];
            bool add_color = false;
            add_color = !(cell.colour == colours::bg_def);
            if (add_color) {
                buffer << cell.colour;
            }
            buffer << cell.value;
            if (add_color) {
                buffer << colours::bg_def;
            }
        }
        buffer << '\n';
    }
    std::cout << buffer.str();
    buffer.clear();
}

void Frame::addShape(geometry::Shape& sh)
{
    for (auto pos : sh.points) {
        this->cells[pos.y][pos.x].value = sh.symbol;
        this->cells[pos.y][pos.x].colour = sh.colour;
    }
}

geometry::Shape geometry::Shape::intersects(Shape& other)
{
    std::vector<Point> result;
    for (int i = 0; i < this->points.size(); i++) {
        for (int j = 0; j < other.points.size(); j++) {
            if (this->points[i] == other.points[j]) {
                result.push_back(this->points[i]);
            }
        }
    }
    
    return Shape(result, other.symbol, other.colour);
}

void geometry::Shape::rotate(bool clockwise)
{
    if (this->points.size() == 0 || this->points.size() == 1) {
        return;
    }

    int sin90 = (clockwise) ? -1 : 1;

    // selecting bottom most left most point.
    auto center = this->points[0];

    for (int i = 1; i < this->points.size(); i++) {
        if (this->points[i].y > center.y || (this->points[i].y == center.y && this->points[i].x < center.x)) {
            center = this->points[i];
        }
    }
    
    for (int i = 0; i < this->points.size(); i++) {
        auto centerVector = center.vector(this->points[i]);
        int x = centerVector.x;
        int y = centerVector.y;
        centerVector.x = -y * sin90;
        centerVector.y = x * sin90;
        this->points[i].x = center.x + centerVector.x;
        this->points[i].y = center.y + centerVector.y;
    }
}

void geometry::Shape::move(Point offset)
{
    for (int i = 0; i < this->points.size(); i++) {
        this->points[i].x += offset.x;
        this->points[i].y += offset.y;
    }
}

void geometry::Shape::subtract(Shape other)
{
    auto intersection = this->intersects(other);
    if (intersection.points.size() == 0) {
        return;
    }

    int s = int(this->points.size());
    int removed = 0;
    for (int i = 0; i < s; i++) {
        if (i == 3) {
            i = 3;
        }
        for (auto p : intersection.points) {
            if (this->points[i - removed].x == p.x && this->points[i - removed].y == p.y) {
                if (this->points.size() == 1) {
                    this->points.clear();
                }
                else {
                    this->points.erase(this->points.begin() + (i - removed));
                }
                removed+=1;
                break;
            }
        }
    }
}

void geometry::Shape::merge(const Shape& shape)
{
    this->subtract(shape);
    for (auto p : shape.points) {
        this->points.push_back(p);
    }
}

std::vector<geometry::Shape> geometry::Shape::collapse()
{
    if (this->points.size() == 1 || this->points.size() == 0) {
        return std::vector<geometry::Shape>{};
    }
    Node<geometry::Point> tree = Node<geometry::Point>(this->points[0]);
    for (int i = 0; i < this->points.size(); i++) {
        auto ps = this->points[i].expand();
        bool notFound = true;
        for (int j = 0; j < ps.size(); j++) {
            if (j == ps.size() - 1 && notFound) {
                tree.addNode(this->points[i], ps[j]);
            }
            else {
                notFound = notFound && !tree.addNode(this->points[i], ps[j], false);
            }
        }
    }
    auto trees = tree.toVectorArray<geometry::Point>();
    for (int i = 1; i < trees.size(); i++) {
        std::sort<>(trees[i].begin(), trees[i].end(), [](geometry::Point a, geometry::Point b) {
                if (a.x > b.x || (a.x < b.x && a.y > b.y)) {
                    return true;
                }
                return false;
            });
    }
    std::unique(trees.begin(), trees.end(), [](std::vector<geometry::Point> a, std::vector<geometry::Point> b) {
            if (a.size() != b.size()) {
                return false;
            }
            for (int i = 0; i < a.size(); i++) {
                if (a[i].x != b[i].x || a[i].y != b[i].y) {
                    return false;
                }
            }
            return true;
        });
    if (trees.size() == 1) {
        return std::vector<geometry::Shape>{};
    }

    this->points = trees[0];

    auto shapes = std::vector<geometry::Shape>{};
    for (int i = 1; i < trees.size(); i++) {
        shapes.push_back(geometry::Shape(trees[i], this->symbol, this->colour));
    }
    return shapes;
}

geometry::Line::Line(Point p1, Point p2, char symbol, ANSIColour::Colour colour)
{
    this->symbol = symbol;
    this->colour = colour;
    if (p1.x == p2.x) {
        for (int i = p1.y; i <= p2.y; i++) {
            this->points.push_back(Point(p1.x, i));
        }
    }
    if (p1.y == p2.y) {
        for (int i = p1.x; i <= p2.x; i++) {
            this->points.push_back(Point(i, p1.y));
        }
    }
}

std::vector<geometry::Point> geometry::Point::expand()
{
    return std::vector<geometry::Point> ({
        geometry::Point(this->x, this->y + 1), geometry::Point(this->x, this->y - 1), geometry::Point(this->x + 1, this->y), geometry::Point(this->x - 1, this->y)
    });
}
