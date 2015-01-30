#include <SFML/Graphics.hpp>
#include "drawer.h"
#include "utility.h"
#include <cstdio>
#include <cmath>
#include "config.h"

class SFMLShape : public IShape {
public:
	SFMLShape(sf::Shape* in) : shape(in) {
		shape->setFillColor(sf::Color(50,50,50));
	}
	virtual ~SFMLShape() { delete shape; }
	virtual void SetPosition(Vector2d pos) {
		shape->setPosition(pos.x, pos.y);
	}
	virtual Vector2d GetPosition() { 
		sf::Vector2f out = shape->getPosition(); 
		Vector2d me; me.x = int(out.x); me.y = int(out.y);
		return me;
	}
	void Rotate(float angle) { shape->rotate(angle); }
	float GetRotation() {
		return shape->getRotation();
	}
	void Draw(void* context)
	{
		sf::RenderWindow* win = (sf::RenderWindow*)context;
		win->draw(*shape);
	}
	void Move(Vector2d move)  {
		shape->move(move.x, move.y);
	}

	void SetOrigin(Vector2d vec) {
		shape->setOrigin(vec.x, vec.y);
	}
	sf::Shape* shape;
};




class SFMLDrawer : public IDrawer {
	sf::RenderWindow* window_;
	sf::Clock tclock;
public:
	void Init() {
		window_ = new sf::RenderWindow(sf::VideoMode(640, 480), "Simulator");
		sf::View view(sf::FloatRect(0,0, 1024, 768));
		window_->setView(view);
	}
	void Done() {
		delete window_;
	}
	void DrawShape(IShape* shape) {
		shape->Draw((void*)window_);
	}
	void Present() {
		window_->display();
		//sleep enough time
		sf::Time t = tclock.restart();
		if(t.asMilliseconds() < 1000.0 / 60.0)
			sf::sleep(sf::milliseconds(1000.0 / 60.0) - t);
	}
	void Prepare() {
		//handle events also
        sf::Event event;
        while (window_->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window_->close();
        }
		window_->clear(sf::Color(255,255,255));
	}
	bool Alive() {
		return window_->isOpen();
	}


	IShape* CreateCircle(int r, int g, int b, int rad) {
		sf::CircleShape* shape = new sf::CircleShape(rad);
		shape->setOrigin(rad,rad);
		IShape* out = new SFMLShape(shape);
		shape->setFillColor(sf::Color(r,g,b));
		return out;
	}
	IShape* CreateLine(Vector2d p1, Vector2d p2, int r, int g, int b, int size) {
		sf::RectangleShape* rect = new sf::RectangleShape(sf::Vector2f(p1.x, p1.y));
		rect->setPosition(sf::Vector2f(p1.x, p1.y));
		float len = (p1-p2).Len();
		rect->setSize(sf::Vector2f(len, size));

		Vector2d target = p2-p1;
		Vector2d orig = p1; orig.x = int(len); orig.y = 0;
		orig = orig.GetNormalized();
		target = target.GetNormalized();
		float dot = target.Dot(orig);
		//float angle = atan2f(target.y-orig.y, target.x-orig.x) * 57.2957795;
		float angle = acos(dot) * 57.2957795;
		if(p1.y > p2.y)
			angle = -angle;

		rect->rotate(angle);
		SFMLShape * shape = new SFMLShape(rect);
		rect->setFillColor(sf::Color(r,g,b));

		return shape;
	}
};

IDrawer* CreateSFMLDrawer() { return new SFMLDrawer; }