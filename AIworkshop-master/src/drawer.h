#ifndef WS_DRAWER_H
#define WS_DRAWER_H

#include "utility.h"

class IShape {
public:
	virtual ~IShape() {}

	virtual void SetPosition(Vector2d pos) = 0;
	virtual Vector2d GetPosition() = 0;
	virtual float GetRotation() = 0;
	virtual void Rotate(float angle) = 0;
	virtual void Move(Vector2d move) = 0;
	virtual void Draw(void* context) = 0;
	virtual void SetOrigin(Vector2d vec) = 0;
};
class IDrawer {
public:
	virtual void Init() = 0;
	virtual void Done() = 0;
	virtual void DrawShape(IShape* shape) = 0;
	virtual void Present() = 0;
	virtual void Prepare() = 0;
	virtual bool Alive() = 0;

	//factory stuff
	virtual IShape* CreateCircle(int, int, int, int) = 0; //r,g,b,radius
	virtual IShape* CreateLine(Vector2d p1, Vector2d p2, int, int, int, int) = 0; //r,g,b,width
};

#endif