//
// Created by lenny on 21.01.2026.
//

#include "textureHandler.hpp"

#include <algorithm>
#include <iostream>

// Only Supports png, jpeg, bmp and gif
bool JFLX::SDL3::TextureHandler::loadTextureFolder(const std::string& folderPath, SDL_ScaleMode scaling) {
    cleanup();

    if (!fs::exists(folderPath)) {
        JFLX::log("Texture folder does not exist: ", folderPath, JFLX::LOGTYPE::JFLX_INFO);
        return false;
    }

    for (const auto& dir : fs::recursive_directory_iterator(folderPath)) {
        std::string ext = dir.path().extension().string();

        // Bug fix: was using || instead of &&, which caused every file to pass the filter
        if (ext != ".png" && ext != ".jpg" && ext != ".bmp" && ext != ".gif") {
            continue;
        }

        std::string tempPath    = dir.path().string();
        std::string tempNameExt = dir.path().filename().string();   // "player.png"
        std::string tempName    = dir.path().stem().string();       // "player"

        JFLX::log("Texture: ", (tempNameExt + " in " + tempPath), JFLX::LOGTYPE::JFLX_INFO);

        SDL_Texture* tex = IMG_LoadTexture(renderer, tempPath.c_str());

        SDL_SetTextureScaleMode(tex, scaling);
        if (!tex) {
            JFLX::log("Failed to load texture: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
            continue;
        }

        textureLayers[tempName] = static_cast<int>(textures.size());
        textures.push_back(tex);
        JFLX::log("Loaded Texture: ", tempName, JFLX::LOGTYPE::JFLX_SUCCESS);
    }

    return !textures.empty();
}

bool JFLX::SDL3::TextureHandler::exists(const std::string& name) const {
    return textureLayers.contains(name);
}

int JFLX::SDL3::TextureHandler::getTextureLayer(const std::string& name) const {
    auto it = textureLayers.find(name);
    return (it == textureLayers.end()) ? -1 : it->second;
}

SDL_Texture* JFLX::SDL3::TextureHandler::getTexture(int layer) const {
    if (layer < 0 || layer >= static_cast<int>(textures.size())) {
        return nullptr;
    }
    return textures[layer];
}

bool JFLX::SDL3::TextureHandler::renderTexture(int layer, float x, float y, renderMode mode, float scaleX, float scaleY, double rotation, SDL_FlipMode flip, SDL_Color color) const {
    SDL_Texture* tex = getTexture(layer);
    if (!tex) {
        std::cerr << "renderTexture: invalid layer " << layer << "\n";
        return false;
    }

    const float w = static_cast<float>(tex->w) * scaleX;
    const float h = static_cast<float>(tex->h) * scaleY;

    // --------------------------------------------------------
    // Compute top-left corner of dstRect from the anchor point.
    // anchorOffX / anchorOffY describe the anchor's position
    // *within* the texture rect (0,0 = top-left, w,h = bottom-right).
    // --------------------------------------------------------
    float anchorOffX = 0.f;
    float anchorOffY = 0.f;

    switch (mode) {
        case renderMode::JFLX_RENDER_TOPLEFT:
            anchorOffX = 0.f;   anchorOffY = 0.f;   break;
        case renderMode::JFLX_RENDER_TOPRIGHT:
            anchorOffX = w;     anchorOffY = 0.f;   break;
        case renderMode::JFLX_RENDER_BOTTOMLEFT:
            anchorOffX = 0.f;   anchorOffY = h;     break;
        case renderMode::JFLX_RENDER_BOTTOMRIGHT:
            anchorOffX = w;     anchorOffY = h;     break;
        case renderMode::JFLX_RENDER_CENTERED:
            anchorOffX = w / 2.f; anchorOffY = h / 2.f; break;
    }

    SDL_FRect dstRect {
        x - anchorOffX,
        y - anchorOffY,
        w,
        h
    };

    // The SDL centre point is relative to dstRect's top-left,
    // so it always equals the anchor offset computed above.
    SDL_FPoint center { anchorOffX, anchorOffY };

    // Apply colour + alpha modulation
    SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(tex, color.a);

    SDL_RenderTextureRotated(renderer, tex, nullptr, &dstRect,
                             rotation, &center, flip);

    // Restore neutral modulation so other textures are unaffected
    SDL_SetTextureColorMod(tex, 255, 255, 255);
    SDL_SetTextureAlphaMod(tex, 255);

    return true;
}

bool JFLX::SDL3::TextureHandler::renderTexture(const std::string& name, float x, float y, renderMode mode, float scaleX, float scaleY, double rotation, SDL_FlipMode flip, SDL_Color color) const {
    const int layer = getTextureLayer(name);
    if (layer == -1) {
        std::cerr << "renderTexture: unknown texture '" << name << "'\n";
        return false;
    }
    return renderTexture(layer, x, y, mode, scaleX, scaleY, rotation, flip, color);
}

void JFLX::SDL3::TextureHandler::cleanup() {
    for (SDL_Texture* tex : textures) {
        SDL_DestroyTexture(tex);
    }
    textures.clear();
    textureLayers.clear();
}