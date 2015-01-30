#include "entity.h"
#include "utility.h"
#include "environment.h"
#include <cmath>
#include <stdio.h>

class Bot;
IEntity* CreateMissle(EntityPtr owner, Vector2d pos, float rot, IDrawer * d);
#include <ctime>

class Bot : public IEntity {
public:
	int cid;
	EnvirInfo CalculateAbstractInfo(IEnvironment* env, Vector2d pos, float rotation) {
		EnvirInfo out;
		for(int i = 0; i < RAY_COUNT; ++i) {
			Vector2d orient = Vector2d(1,0);
			float angle = rotation + RAY_ANGLE / (RAY_COUNT - 1) * (i - RAY_COUNT / 2);
			orient.x = cos(angle *PI/180);
			orient.y = sin(angle *PI/180);
			EntityPtr ent = env->LineIntersect(pos, pos + orient, MAX_DIST, &out.ray_length[i]);
			if(ent != EntityPtr())
				out.ray_detail[i] = ent->Type();
			else
				out.ray_detail[i] = ET_NONE;

			if(out.ray_detail[i] == ET_BOT) {
				out.ray_udata[i] = ((Bot*)ent.get())->team;
			}
			rays[i] = pos + orient * out.ray_length[i];
			out.ray_pos[i].x = rays[i].x;
			out.ray_pos[i].y = rays[i].y;
		}
		return out;
	}
	void UpdateBot(AgentStatus status, IEnvironment* env) {
		if(status.left) {
			shape->Rotate(-3);
		}
		if(status.right) {
			shape->Rotate(3);
		}
		float rot = shape->GetRotation();
		Vector2d vel = Vector2d(cos(rot*PI/180),sin(rot*PI/180));
		
		if(status.forward) {
			collide = false;
			EntityPtr ent = env->CircleIntersect(GetPosition() + vel, BOT_SIZE, 0);
			collide = true;
			if(ent == EntityPtr())
				shape->Move(vel);
		}
		if(status.fire) {
			uint32_t ntime = env->GetTicks();
			if(ntime - last_fire > MISSLE_COOLDOWN) {
				EntityPtr self = env->GetSmart(this);
				if(self == EntityPtr()) {
					printf("Failed to fire a missle. Missing smart pointer;");
					return;
				}
				EntityPtr missle(CreateMissle(self, GetPosition(), rot, env->GetDrawer()));
				if(self == EntityPtr()) {
					printf("Failed to fire a missle. Failed to allocate;");
					return;
				}
				env->AddEntity(missle);
				last_fire = ntime;
			}
		}
	}
	void Update(IEnvironment* env) {
		EnvirInfo info = CalculateAbstractInfo(env, GetPosition(), shape->GetRotation());
		info.team = team;
		info.id = cid;
		AgentStatus status = agent->Update(info);
		UpdateBot(status, env);
	}
	void Draw(IDrawer* dr) {
		Vector2d p = GetPosition();
		for(int i = 0; i < RAY_COUNT; ++i)
		{
			IShape* s = dr->CreateLine(p, rays[i], 240,100,100, 3);
			dr->DrawShape(s);
			delete s;
		}
		dr->DrawShape(shape);
		p.y += BOT_SIZE + 8;
		IShape* hp_bar_bg = dr->CreateLine(
			Vector2d(p.x - BOT_SIZE, p.y), Vector2d(p.x + BOT_SIZE, p.y), 0,0,0, 5);
		IShape* hp_bar = dr->CreateLine(
			Vector2d(p.x - BOT_SIZE, p.y), Vector2d(p.x - BOT_SIZE + BOT_SIZE * 2 * ((float)hp / (float)BOT_HP), p.y), 255,0,0, 5);
		dr->DrawShape(hp_bar_bg);
		dr->DrawShape(hp_bar);

		delete hp_bar_bg;
		delete hp_bar;
	}
	IShape* GetShape() {
		return shape;
	}


	IntersectionResult IntersectVsLine(Vector2d p1, Vector2d p2, int d) {
		IntersectionResult res;
		if(!collide) {
			res.success = false;
			return res;
		}
		if(p1 == shape->GetPosition()) {
			res.success = false;
			return res;
		}
		return Intersect_line_circle(p1,p2, GetPosition(), BOT_SIZE, d);
	}
	IntersectionResult IntersectVsCircle(Vector2d p , int r, int d) {
		IntersectionResult res;
		if(!collide) {
			res.success = false;
			return res;
		}
		if((p - GetPosition()).Len() < r + BOT_SIZE) {
			res.success = true;
		} else {
			res.success = false;
		}
		res.vpoint = false;
		return res;
	}
	EntityType Type() {
		return ET_BOT;
	}
	DmgAction Damage(int d) {
		hp -= d;
		return hp <= 0 ? DA_REMOVE : DA_NONE;
	}

	Bot(int c, IAgent* a, int t, IShape* s) : cid(c), agent(a), team(t), shape(s) { hp = BOT_HP; last_fire = 0; collide = true;}
	~Bot() { delete shape; }

	bool collide;
private:
	IShape* shape;
	int hp;
	IAgent* agent;
	int team;
	Vector2d rays[RAY_COUNT];
	uint32_t last_fire;
};
IEntity* CreateBot(int cid, IAgent* agent, int team, IShape* shape) {
	return new Bot(cid, agent, team, shape);
}

class Line : public IEntity {
public:
	void Update(IEnvironment* env) {

	}
	void Draw(IDrawer* c) {
		c->DrawShape(shape);
	}

	IShape* GetShape() {
		return shape;
	}

	DmgAction Damage(int) { return DA_NONE; }
	EntityType Type() { return ET_LINE; }

	IntersectionResult IntersectVsLine(Vector2d p1, Vector2d p2, int dist) {
		//printf("P1x %f, P1y %f, P2x %f, P2y %f, S1x %f, S1x %f, S2y %f, S2y %f\n", p1.x, p1.y, p2.x, p2.y, sp.x, sp.y, ep.x, ep.y);
		IntersectionResult res = Intersect_line_line(p1, p2, sp, ep, dist);
		if(!res.success) {
			return res;
		}
		if(!ClampLines(res.point, sp, ep)) {
			res.success = false;
		}
		return res;
	}
	IntersectionResult IntersectVsCircle(Vector2d p, int r, int d) {
		return Intersect_line_circle(sp, ep, p, r, (sp - ep).Len());
	}
	Line(Vector2d p1, Vector2d p2, IShape* s) : sp(p1), ep(p2), shape(s) {}
	~Line() { delete shape;}// printf("%s\n", "Deleted line"); }
private:
	Vector2d sp, ep;
	IShape* shape;
};

IEntity* CreateLine(Vector2d p1, Vector2d p2, IShape* shape) {
	return new Line(p1,p2,shape);
}


Vector2d RotateVector(Vector2d v, float r) {
	float cs = cos(r*PI/180);
	float sn = sin(r*PI/180);

	Vector2d out;
	out.x = v.x * cs - v.y * sn;
	out.y = v.x * sn + v.y * cs;
	return out;
}

class Missle : public IEntity{
public:
	void GetLines(Vector2d &lt, Vector2d &rt, Vector2d &lb, Vector2d &rb) {
		Vector2d pos = GetPosition();
		float rot = shape->GetRotation();

		lt = Vector2d(-MISSLE_WIDTH/2, MISSLE_LENGTH);
		rt = Vector2d(MISSLE_WIDTH/2,  MISSLE_LENGTH);
		lb = Vector2d(-MISSLE_WIDTH/2, 0);
		rb = Vector2d(MISSLE_WIDTH/2, 0);
		lt = RotateVector(lt, rot+1.57079633) + pos;
		rt = RotateVector(rt, rot+1.57079633) + pos;
		lb = RotateVector(lb, rot+1.57079633) + pos;
		rb = RotateVector(rb, rot+1.57079633) + pos;	
	}
	void Update(IEnvironment* env) {
		float rot = shape->GetRotation();
		Vector2d vel(cos(rot * PI/180), sin(rot * PI/180));
		vel = vel * MISSLE_SPEED;
		Vector2d pos = GetPosition();

		Vector2d lt,rt,lb,rb;
		GetLines(lt, rt, lb, rb);

		collide = false;
		((Bot*)owner.get())->collide = false;
		int dummy;
		EntityPtr ent = env->LineIntersect(pos, pos+vel, MISSLE_LENGTH, &dummy);
		collide = true;
		((Bot*)owner.get())->collide = true;

		if(ent == EntityPtr()) {
			shape->Move(vel);
		}else {
			EntityList entities = env->QueueEntities
				(pos + vel.GetNormalized() * MISSLE_LENGTH, MISSLE_AOE, ET_UNKNOWN);
			EntityList::iterator i = entities.begin();
			EntityList::iterator end = entities.end();
			while(i != end) {
				env->DamageEntity(*i, MISSLE_DMG);
				++i;
			}
		}
	}

	void Draw(IDrawer* drawer) {
		drawer->DrawShape(shape);
	}

	IShape* GetShape() {
		return shape;
	}

	DmgAction Damage(int a) {
		return DA_FREE;
	}

	EntityType Type() {
		return ET_MISSLE;
	}

	IntersectionResult IntersectVsLine(Vector2d p1, Vector2d p2, int d) {
		if(!collide) {
			IntersectionResult res;
			res.success = false;
			return res;
		}
		IntersectionResult res;
		float rot = shape->GetRotation();
		Vector2d vel(cos(rot * PI/180), sin(rot * PI/180));
		Vector2d p3 = GetPosition(), p4 = GetPosition() + vel;
		res = Intersect_line_line(p1, p2, p3, p4, d);
		if(res.success == false) {
			return res;
		}
		if(!ClampLines(res.point, p3, p4)) {
			res.success = false;
		}
		return res;
	}

	IntersectionResult IntersectVsCircle(Vector2d p, int rad, int d) {
		if(!collide) {
			IntersectionResult res;
			res.success = false;
			return res;
		}
		IntersectionResult res;
		float rot = shape->GetRotation();
		Vector2d vel(cos(rot * PI/180), sin(rot * PI/180));
		Vector2d p1 = GetPosition(), p2 = GetPosition() + vel;
		res = Intersect_line_circle(p1, p2, p, rad, 0);
		return res;
	}

	Missle(EntityPtr o, IShape* s) : owner(o), shape(s) {collide = true;}  
	~Missle() { delete shape; } //printf("%s\n", "Deleted missle"); }
private:
	IShape* shape;
	EntityPtr owner;
	bool collide;
};
IEntity* CreateMissle(EntityPtr owner, Vector2d pos, float rot, IDrawer * d) {
	IShape* shape = d->CreateLine(pos, pos + Vector2d(cos(rot*PI/180), sin(rot * PI/180)) * MISSLE_LENGTH, 100, 50, 50, MISSLE_WIDTH);
	shape->SetOrigin(Vector2d(MISSLE_WIDTH/2, 0));
	return new Missle(owner, shape);
}