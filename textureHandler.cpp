//
// Created by lenny on 21.01.2026.
//

#include "textureHandler.hpp"

#include <algorithm>
#include <iostream>

// Only Supports png, jpeg, bmp and gif
bool JFLX::SDL3::TextureHandler::loadTextureFolder(const std::string& folderPath) {
    cleanup();

    if (!fs::exists(folderPath)) {
        JFLX::log("Texture folder does not exist: ", folderPath, JFLX::LOGTYPE::JFLX_INFO);
        return 0;
    }

    for (const auto& dir : fs::recursive_directory_iterator(folderPath)) {
        std::string ext = dir.path().extension().string();
        if (ext != ".png" || ext != ".jpg" || ext != ".bmp" || ext != ".gif") {
            std::string tempPath = dir.path().string();
            std::string tempNameAndExt = dir.path().stem().string();
            std::string tempName = dir.path().stem().filename().string();

            JFLX::log("Texture: ", (tempNameAndExt + " in " + tempPath), JFLX::LOGTYPE::JFLX_INFO);

            //* load Texture from file
            SDL_Texture* tex = IMG_LoadTexture(renderer, tempPath.c_str());
            if (!tex) {
                JFLX::log("Failed to load texture: ", SDL_GetError(), JFLX::LOGTYPE::JFLX_ERROR);
                continue;
            }

            textureLayers[tempName] = static_cast<int>(textures.size());
            textures.push_back(tex);
            JFLX::log("Loaded Texture: ", tempNameAndExt, JFLX::LOGTYPE::JFLX_SUCCESS);
        }
    }

    return true;
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

bool JFLX::SDL3::TextureHandler::renderTexture(int layer, float x, float y, float scaleX, float scaleY, double rotation, SDL_FlipMode flip, SDL_Color color) const {
    SDL_Texture* tex = getTexture(layer);
    if (!tex) {
        std::cerr << "renderTexture: invalid layer " << layer << "\n";
        return false;
    }

    // Apply color + alpha modulation
    SDL_SetTextureColorMod(tex, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(tex, color.a);

    // Build destination rect from texture size + scale
    SDL_FRect dstRect{
        x, y,
        static_cast<float>(tex->w) * scaleX,
        static_cast<float>(tex->h) * scaleY
    };

    SDL_RenderTextureRotated(renderer, tex, nullptr, &dstRect,
                             rotation, nullptr, flip);

    // Restore modulation to avoid affecting other textures
    SDL_SetTextureColorMod(tex, 255, 255, 255);
    SDL_SetTextureAlphaMod(tex, 255);

    return true;
}

bool JFLX::SDL3::TextureHandler::renderTexture(const std::string& name, float x, float y, float scaleX, float scaleY, double rotation, SDL_FlipMode flip, SDL_Color color) const {
    const int layer = getTextureLayer(name);
    if (layer == -1) {
        std::cerr << "renderTexture: unknown texture '" << name << "'\n";
        return false;
    }
    return renderTexture(layer, x, y, scaleX, scaleY, rotation, flip, color);
}

void JFLX::SDL3::TextureHandler::cleanup() {
    for (SDL_Texture* tex : textures) {
        SDL_DestroyTexture(tex);
    }
    textures.clear();
    textureLayers.clear();
}