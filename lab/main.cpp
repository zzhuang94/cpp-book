#include <iostream>
#include <memory>
#include <vector>

class Shape;    // 前置声明
using ShapePtr = std::unique_ptr<Shape>;    // 使用别名

class Shape {
public:
    virtual ~Shape() = default;
    virtual void draw() const = 0;
};

class Circle : public Shape {
public:
    void draw() const override {
        std::cout << "draw circle" << std::endl;
    }
};

class Rectangle : public Shape {
public:
    void draw() const override {
        std::cout << "draw rectangle" << std::endl;
    }
};

class Triangle : public Shape {
public:
    void draw() const override {
        std::cout << "draw triangle" << std::endl;
    }
};

void draw(const Shape& shape) {
    shape.draw();
}

void reset(Shape* shape) {
    if (shape != NULL) {
        std::cout << "reset shape" << std::endl;
        shape->draw();
    }
}

void add(std::vector<ShapePtr>& shapes, ShapePtr shape) {
    shapes.push_back(std::move(shape));
}

int main() {
    Circle circle;
    draw(circle);
    reset(&circle);

    std::cout << "--------------------------------" << std::endl;

    std::vector<ShapePtr> shapes;
    add(shapes, ShapePtr(new Circle()));
    add(shapes, ShapePtr(new Triangle()));
    add(shapes, ShapePtr(new Rectangle()));
    for (const auto& shape : shapes) {
        shape->draw();
    }
    return 0;
}