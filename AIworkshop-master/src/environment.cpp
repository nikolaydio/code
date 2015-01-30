#include "environment.h"
#include "drawer.h"
#include <stdio.h>
#include <cmath>
#include "entity.h"

IEntity* CreateLine(Vector2d p1, Vector2d p2, IShape* shape);
IEntity* CreateBot(int, IAgent* agent, int team, IShape* shape);


class Environment : public IEnvironment {
public:
	Environment(IDrawer* d) : IEnvironment() {
		drawer = d;
		ticks = 0;
		Vector2d p1, p2;
		p1.x = 20; p1.y = 0; p2.x = 1020; p2.y = 0;
		AddLine(p1,p2);
		p1.x = 1020; p1.y = 750;
		AddLine(p1,p2);
		p2.x = 20; p2.y = 750;
		AddLine(p1,p2);
		p1.x = 20; p1.y = 0;
		AddLine(p1,p2);

		AddLine(Vector2d(150,0),Vector2d(150,620));
		AddLine(Vector2d(900,100),Vector2d(900,760));
		AddLine(Vector2d(760,100),Vector2d(760,620));
		AddLine(Vector2d(650, 100), Vector2d(300, 100));
		AddLine(Vector2d(300,100), Vector2d(300, 620));
		AddLine(Vector2d(400, 620), Vector2d(760, 620));
		cid = 0;
	}
	~Environment() {}
	void AddLine(Vector2d a, Vector2d b) {
		IEntity * ent = CreateLine(a, b, drawer->CreateLine(a,b, 150,150,150,3));
		AddEntity(EntityPtr(ent));
	}
	int cid;
	void AddAgent(IAgent* agent, int team, int num) {
		cid++;
		IEntity* ent = CreateBot(cid, agent, team, drawer->CreateCircle(team * 250,0,(!team) * 250,BOT_SIZE));
		ent->SetPosition(Vector2d(80 + team * 500, 100 + num * 80));
		ent->GetShape()->Rotate(90);
		AddEntity(EntityPtr(ent));
	}
	void AddEntity(EntityPtr entity) {
		if(entity.get() == 0) {
			printf("0 entity");
			while(1) {}
			return;
		}
		add_list.push_back(entity);
	}

	void RenderWorld() {
		EntityList::iterator i = entities.begin();
		EntityList::iterator end = entities.end();
		while(i != end) {
			(*i)->Draw(drawer);
			++i;
		}
	}
	void Update() {
		//printf("Obj to add: %i, Objs: %i, Objs to remove: %i\n", add_list.size(), entities.size(), remove_list.size());
		EntityList::iterator i = add_list.begin();
		while(i != add_list.end()) {
			entities.push_back(*i);
			++i;
		}
		add_list.clear();

		i = entities.begin();
		while(i != entities.end()) {
			(*i).get()->Update(this);
			++i;
		}

		//remove queued entities
		EntityList::iterator ri = remove_list.begin();
		EntityList::iterator rend = remove_list.end();
		while(ri != rend) {
			i = entities.begin();
			while(i != entities.end()) {
				if(*i == *ri) {
					i = entities.erase(i);
					break;
				}
				++i;
			}
			++ri;
		}
		remove_list.clear();

		ticks++;
	}


	EntityPtr LineIntersect(Vector2d p1, Vector2d p2, int dist, int* len) {
		EntityList::iterator i = entities.begin();
		EntityList::iterator end = entities.end();
		EntityPtr out;

		*len = dist;

		//printf("start\n");
		for(;i != end;++i) {
			IntersectionResult res = (*i)->IntersectVsLine(p1, p2, *len);
			if(!res.success) {
				continue;
			}
			//res.point.Print("I");

			*len = (int)(res.point - p2).Len();
			out = *i;
		}


		return out;
	}
	EntityPtr CircleIntersect(Vector2d p, float r, int d) {
		EntityList::iterator i = entities.begin();
		EntityList::iterator end = entities.end();
		EntityPtr out;
		for(;i != end;++i) {
			IntersectionResult res = (*i)->IntersectVsCircle(p, r, d);
			if(!res.success) {
				continue;
			}
			out = *i;
		}


		return out;
	}
	
	void DamageEntity(EntityPtr ent, int d) {
		DmgAction action = ent.get()->Damage(d);
		if(action != DA_NONE) {
			//prevent from adding the same thing twice
			{
				std::vector<EntityPtr>::iterator i = remove_list.begin();
				std::vector<EntityPtr>::iterator end = remove_list.end();
				while(i != end) {
					if(*(i) == ent) {
						return;
					}
					++i;
				}
			}
			remove_list.push_back(ent);
			//printf("Obj removed.\n");
		}
	}
	EntityList QueueEntities(Vector2d pos, float radius, EntityType type) {
		EntityList::iterator i = entities.begin();
		EntityList::iterator end = entities.end();
		EntityList e;
		while(i != end) {
			if(((*i)->GetPosition() - pos).Len() <= radius )
				e.push_back(*i);
			++i;
		}
		return e;
	}

	void SetDrawer(IDrawer*d) {
		drawer = d;
	}
	IDrawer* GetDrawer() {
		return drawer;
	}

	uint32_t GetTicks() {
		return ticks;
	}

	EntityPtr GetSmart(IEntity* ent) {
		EntityList::iterator i = entities.begin();
		EntityList::iterator end = entities.end();
		while(i != end) {
			if(ent == (*i).get())
				return *i;
			++i;
		}
		return EntityPtr();
	}
private:
	EntityList entities;
	EntityList remove_list;
	EntityList add_list;
	IDrawer* drawer;
	uint32_t ticks;
};

IEnvironment* CreateEnvironment(IDrawer* d) {
	return new Environment(d);
}