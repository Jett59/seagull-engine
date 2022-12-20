#ifndef SEAGULL_RENDERER_H
#define SEAGULL_RENDERER_H

#include <seagull/gameObject.h>
#include <seagull_internal.h>

namespace seagull {
void render(const GameObjectState &gameObject, const GameContext &gameContext,
            bool bindTextures);
}

#endif