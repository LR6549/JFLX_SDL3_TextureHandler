//
// Created by lenny on 21.01.2026.
//

#ifndef JFLX_SDL3_TEXTUREHANDLER_HPP
#define JFLX_SDL3_TEXTUREHANDLER_HPP

#define JFLX_SDL3_TEXTUREHANDLER

#include <SDL3/SDL.h>
#include <SDL3/SDL3_image/SDL_image.h>

#include <unordered_map>
#include <string>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace JFLX::SDL3 {

    class textureHandler {
    private:
        SDL_Renderer* renderer = nullptr;

        std::vector<SDL_Texture*> textures;
        std::unordered_map<std::string, int> textureLayers;

    public:
        explicit textureHandler(SDL_Renderer* renderer) : renderer(renderer) {}

        bool loadTextureFolder(const std::string& folderPath);

        [[nodiscard]] int layers() const {
            return static_cast<int>(textures.size());
        }

        int getTextureLayer(const std::string& name) const;

        SDL_Texture* getTexture(int layer) const;

        // Render by layer index
        bool renderTexture(int layer,
                           float x, float y,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip     = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 255}) const;

        // Render by name
        bool renderTexture(const std::string& name,
                           float x, float y,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip     = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 255}) const;

        void cleanup();

        ~textureHandler() { cleanup(); }
    };

}

#endif