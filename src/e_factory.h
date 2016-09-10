#ifndef _E_FACTORY_h_
#define _E_FACTORY_h_

struct ECS_Manager;

u32 ECS_NewPlayer(struct ECS_Manager *manager, int x, int y);
u32 ECS_NewWall(struct ECS_Manager *manager, int x, int y);
u32 ECS_NewDot(struct ECS_Manager *manager, int x, int y);
u32 ECS_NewGhost(struct ECS_Manager *manager, int x, int y, SDL_Color color);

#endif
