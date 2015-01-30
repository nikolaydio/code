#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "drawer.h"
#include "utility.h"
#include "environment.h"
#include <memory>

class IEntity;
class IAgent;
typedef std::shared_ptr<IEntity> EntityPtr;
typedef std::vector<EntityPtr> EntityList;
enum DmgAction {
	DA_NONE,
	DA_FREE,
	DA_REMOVE
};
class IEnvironment {
public:
	virtual void AddEntity(EntityPtr entity) = 0;
	virtual void AddLine(Vector2d a, Vector2d b) = 0;
	virtual void AddAgent(IAgent* agent, int team, int num) = 0;


	virtual void RenderWorld() = 0;
	virtual void Update() = 0;
	virtual EntityPtr LineIntersect(Vector2d, Vector2d, int dist, int* len) = 0;
	virtual EntityPtr CircleIntersect(Vector2d, float, int) = 0;
	
	virtual void DamageEntity(EntityPtr ent, int) = 0;
	virtual EntityList QueueEntities(Vector2d pos, float radius, EntityType type) = 0;

	virtual void SetDrawer(IDrawer* d) = 0;
	virtual IDrawer* GetDrawer() = 0;

	virtual uint32_t GetTicks() = 0;
	virtual EntityPtr GetSmart(IEntity* ent) = 0;
};
class IEntity{
public:
	IEntity() {}
	virtual ~IEntity() {}

	virtual void Update(IEnvironment* env) = 0;
	virtual void Draw(IDrawer*) = 0;
	virtual IShape* GetShape() = 0;

	virtual DmgAction Damage(int) = 0;
	virtual EntityType Type() = 0;

	virtual IntersectionResult IntersectVsLine(Vector2d, Vector2d, int) = 0;
	virtual IntersectionResult IntersectVsCircle(Vector2d, int, int) = 0;

	//shortcuts
	Vector2d GetPosition() { return GetShape()->GetPosition(); }
	void SetPosition(Vector2d p) { GetShape()->SetPosition(p); }
};

#endif
