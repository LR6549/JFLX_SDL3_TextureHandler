//
// Created by lenny on 21.01.2026.
//

#ifndef JFLX_SDL3_TEXTUREHANDLER_HPP
#define JFLX_SDL3_TEXTUREHANDLER_HPP

// Macro guard (in addition to the #ifndef above) that can be used for
// compile-time feature detection: #ifdef JFLX_SDL3_TEXTUREHANDLER
#define JFLX_SDL3_TEXTUREHANDLER

#include <SDL3/SDL.h>                        // Core SDL3: SDL_Renderer, SDL_Texture, SDL_Color, SDL_FlipMode, …
#include <SDL3/SDL3_image/SDL_image.h>       // SDL3_image: IMG_LoadTexture() for PNG/JPG/BMP/… support

#include <unordered_map>   // O(1) average lookup: texture name  →  layer index
#include <string>          // std::string for texture names / file paths
#include <filesystem>      // std::filesystem::path / directory_iterator for folder loading
#include <vector>          // Ordered, index-accessible storage for SDL_Texture* pointers

// Convenience alias so we can write fs::path instead of std::filesystem::path throughout
namespace fs = std::filesystem;

namespace JFLX::SDL3 {

    /**
     * @brief Manages a collection of SDL_Texture* objects loaded from a folder.
     *
     * Textures are stored in a flat, ordered list ("layers"). Each layer is
     * identified both by its integer index and by the stem of the source file
     * (e.g. "player" for "player.png"). This makes sprite-sheet–style layer
     * access and name-based look-up equally convenient.
     *
     * Lifetime:  The handler does NOT own the SDL_Renderer; it only borrows it.
     *            It DOES own every SDL_Texture* it creates and destroys them in
     *            cleanup() / the destructor.
     */
    class TextureHandler {
    private:
        /// Borrowed pointer to the SDL renderer used for texture creation and rendering.
        /// Never destroyed by this class.
        SDL_Renderer* renderer = nullptr;

        /// Flat list of loaded textures. The position in this vector is the "layer index".
        /// Index 0 = first file loaded, 1 = second, etc.
        std::vector<SDL_Texture*> textures;

        /// Maps a texture's name (file stem, e.g. "background") to its layer index.
        /// Populated in lockstep with the textures vector so both stay in sync.
        std::unordered_map<std::string, int> textureLayers;

    public:
        /// Default constructor – renderer must be set later via setRenderer() before use.
        TextureHandler() = default;

        /// Preferred constructor: immediately binds a valid renderer.
        explicit TextureHandler(SDL_Renderer* renderer) : renderer(renderer) {};

        /**
         * @brief Replaces the current renderer reference.
         * @param r  New renderer pointer; silently ignored if null to prevent
         *           accidentally clearing a working renderer.
         */
        void setRenderer(SDL_Renderer* r) { if (r != nullptr) this->renderer = r; }

        /**
         * @brief Loads every image file inside @p folderPath as a separate texture layer.
         *
         * Files are loaded in directory-iteration order. The layer index assigned to each
         * texture corresponds to its position in that order. The texture name (used as the
         * key in textureLayers) is the file's stem without extension.
         *
         * @param folderPath  Path to the folder containing image files.
         * @return true  if at least one texture was loaded successfully.
         * @return false if the folder is empty, inaccessible, or no file could be loaded.
         */
        bool loadTextureFolder(const std::string& folderPath);

        /**
         * @brief Checks whether a texture with the given name has been loaded.
         * @param name  Texture name (file stem, e.g. "player").
         * @return true if the name exists in textureLayers.
         */
        bool exists(const std::string& name) const;

        /**
         * @brief Returns the total number of loaded texture layers.
         * @note [[nodiscard]] because ignoring the return value is almost always a bug.
         */
        [[nodiscard]] int layers() const {
            return static_cast<int>(textures.size());
        }

        /**
         * @brief Looks up the layer index for a named texture.
         * @param name  Texture name (file stem).
         * @return Layer index ≥ 0 on success, or -1 if the name is not found.
         */
        int getTextureLayer(const std::string& name) const;

        /**
         * @brief Direct access to the raw SDL_Texture* at the given layer.
         * @param layer  Zero-based layer index.
         * @return Pointer to the texture, or nullptr if @p layer is out of range.
         */
        SDL_Texture* getTexture(int layer) const;

        // ------------------------------------------------------------------ //
        //  Render overloads
        //  Both variants build the same SDL_FRect destination rect from
        //  (x, y, textureWidth * scaleX, textureHeight * scaleY), then call
        //  SDL_RenderTextureRotated() with the supplied transform parameters.
        // ------------------------------------------------------------------ //

        /**
         * @brief Renders the texture at the given layer index.
         *
         * @param layer     Zero-based layer index of the texture to render.
         * @param x         Destination X position in renderer coordinates.
         * @param y         Destination Y position in renderer coordinates.
         * @param scaleX    Horizontal scale factor (1.0 = original width).
         * @param scaleY    Vertical scale factor   (1.0 = original height).
         * @param rotation  Clockwise rotation in degrees around the texture centre.
         * @param flip      SDL_FLIP_NONE / SDL_FLIP_HORIZONTAL / SDL_FLIP_VERTICAL.
         * @param color     Colour & alpha modulation applied before rendering
         *                  (white + full alpha = no modulation).
         * @return true on success, false if the layer is invalid or rendering fails.
         */
        bool renderTexture(int layer,
                           float x, float y,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip     = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 0}) const;

        /**
         * @brief Renders the texture identified by name (convenience overload).
         *
         * Internally resolves the name to a layer index via textureLayers and
         * delegates to the index-based overload above.
         *
         * @param name  Texture name (file stem, e.g. "player").
         * @return true on success, false if the name is not found or rendering fails.
         */
        bool renderTexture(const std::string& name,
                           float x, float y,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip     = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 0}) const;

        /**
         * @brief Destroys all loaded textures and clears the internal containers.
         *
         * Safe to call multiple times. After cleanup() the handler is in the same
         * state as after default construction (renderer reference is kept).
         */
        void cleanup();

        /// Destructor delegates to cleanup() to ensure all SDL_Texture* are freed.
        ~TextureHandler() { cleanup(); }
    };

}

#endif