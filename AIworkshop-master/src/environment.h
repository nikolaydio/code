#ifndef ENVIR_H
#define ENVIR_H

#include "utility.h"
#include <vector>
#include "config.h"

struct AgentStatus {
	bool forward : 1;
	bool backward : 1;
	bool left : 1;
	bool right : 1;
	bool fire : 1;
	bool zoomin : 1;
	bool zoomout : 1;
};
enum EntityType {
	ET_NONE = 0,
	ET_LINE,
	ET_BOT,
	ET_MISSLE,
	ET_UNKNOWN
};


struct Coord2d
{
	float x,y;
};
struct EnvirInfo {
	//3 intersection points. range of the agent they intersect.
	int ray_length[RAY_COUNT];
	EntityType ray_detail[RAY_COUNT];
	int ray_udata[RAY_COUNT];
	Coord2d ray_pos[RAY_COUNT];

	int team;
	int id;
	Vector2d position;
};
class IAgent
{
public:
	virtual AgentStatus Update(EnvirInfo) = 0;
};


#endif