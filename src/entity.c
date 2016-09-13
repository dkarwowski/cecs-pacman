#include "entity.h"
#include "memory.h"

#include <string.h>

#define MAX_ENTITIES 512
struct ECS_Manager {
    int next_eid;

    u64 entity[MAX_ENTITIES];

#define C_ARRAY(name, num) \
    struct ECS_ ## name name[MAX_ENTITIES];
    COMPONENT_BIND(C_ARRAY)
#undef C_ARRAY
};

/**
 * Create a new ECS Manager
 *
 * @stack : where to push the manager
 */
struct ECS_Manager *
ECS_NewManager(struct Stack *stack)
{
    struct ECS_Manager *result = Z_PushStruct(stack, struct ECS_Manager, true);
    return result;
}

/**
 * Add a component to the entity
 *
 * @manager     : the ecs manager
 * @eid         : entity to add to
 * @component   : pointer to the component in memory
 * @componentid : what the component type is
 */
void
ECS_AddComponent(struct ECS_Manager *manager, u32 eid, void *component, enum ECS_ComponentMask componentid)
{
    ASSERT(eid < manager->next_eid);
    ASSERT(component);

    switch(componentid) {
#define C_SWITCH(name, num)                                                \
        case ECS_C ## name:                                                \
            memcpy(manager->name + eid, component, sizeof(struct ECS_ ## name)); \
            break;

        COMPONENT_BIND(C_SWITCH)
#undef ECS_Component

        default:
            ERR_LOG("not valid componentid");
    }

    manager->entity[eid] |= componentid;
}

/**
 * Remove component of given type from the entity
 *
 * @manager     : the ecs manager
 * @eid         : the entity to remove from
 * @componentid : what the component type is
 */
void
ECS_RemoveComponent(struct ECS_Manager *manager, u32 eid, enum ECS_ComponentMask componentid)
{
    if (eid >= manager->next_eid)
        eid = manager->entity[eid];
    ASSERT(eid < manager->next_eid);
    manager->entity[eid] &= ~componentid;
}

/**
 * Add an entity to the manager
 *
 * @manager : the ecs manager
 *
 * @return : the eid of the new entity
 */
u32
ECS_AddEntity(struct ECS_Manager *manager)
{
    ASSERT(manager->next_eid < MAX_ENTITIES);
    
    u32 result = manager->next_eid++;
    manager->entity[result] = ECS_CNone;
    return result;
}

/**
 * Remove an entity from the manager, replaces with last in array
 *
 * @manager : the ecs manager
 * @eid     : entity to remove
 */
void
ECS_RemoveEntity(struct ECS_Manager *manager, u32 eid)
{
    ASSERT(eid < MAX_ENTITIES);
    ASSERT(manager->next_eid > 0);

    if (--manager->next_eid > eid) {
#define C_COPY(name, num) \
        memcpy(manager->name + eid, manager->name + manager->next_eid, sizeof(manager->name[0]));

        COMPONENT_BIND(C_COPY)
#undef C_COPY
    }

    manager->entity[manager->next_eid] = eid;
}

/**
 * Get the next entity while iterating through the entities in the manager
 *
 * @manager : the ecs manager
 * @components : component mask starts the iteration from beginning, no components means continue
 *
 * @return : -1 if none found, or the proper eid
 */
i64
ECS_NextEntity(struct ECS_Manager *manager, struct ECS_Iter *iter)
{
    while (iter->eid < manager->next_eid 
            && (manager->entity[iter->eid] & iter->mask) != iter->mask)
        ++iter->eid;

    if (iter->eid++ < manager->next_eid)
        return iter->eid - 1;
    return -1;
}

/**
 */
void *
ECS_GetComponent(struct ECS_Manager *manager, u32 eid, enum ECS_ComponentMask componentid)
{
    if (eid >= manager->next_eid)
        eid = manager->entity[eid];
    ASSERT(eid < manager->next_eid);
    switch (componentid) {
#define C_COMPONENT(name, num) \
        case ECS_C ## name : \
            return manager->name + eid;
        COMPONENT_BIND(C_COMPONENT)
#undef C_COMPONENT
        default:
            return NULL;
    }
}

/**
 */
bool
ECS_HasComponent(struct ECS_Manager *manager, u32 eid, enum ECS_ComponentMask componentid)
{
    ASSERT(eid < manager->next_eid);
    bool result = componentid & manager->entity[eid];
    return result;
}

