#pragma once

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../util/UsingTypes.hpp"

class RenderManager;

class RenderSystem {

public:
    void update(EManager& EM, RenderManager* RM);
    static void update_render_one_entity_shape(E& e, RenderManager* RM);
    static void update_render_one_entity_mesh(E& e, RenderManager* RM);
    static void update_render_one_entity_camera(E& e, RenderManager* RM);
    static void update_render_one_entity_light(E& e, RenderManager* RM);
    static void update_render_one_entity_image(E& e, RenderManager* RM);
    static void update_render_one_entity_text(E& e, RenderManager* RM);
    static void update_render_one_entity_particle(E& e, RenderManager* RM);

};

#endif