//
// Created by lenny on 21.01.2026.
//

// Standard include guards — prevent this header from being processed more than once
// per translation unit, avoiding duplicate definition errors.
#ifndef JFLX_SDL3_TEXTUREHANDLER_HPP
#define JFLX_SDL3_TEXTUREHANDLER_HPP

// Additional macro definition — can be used elsewhere to check if this header
// has been included (e.g. #ifdef JFLX_SDL3_TEXTUREHANDLER).
#define JFLX_SDL3_TEXTUREHANDLER

// SDL3 core and image extension headers
#include <SDL3/SDL.h>
#include <SDL3/SDL3_image/SDL_image.h>

// STL includes
#include <unordered_map>  // For O(1) average-case name → layer index lookups
#include <string>         // For texture name keys
#include <filesystem>     // For directory iteration when loading a texture folder
#include <vector>         // For the ordered list of loaded SDL_Texture pointers

// Convenience alias — lets us write fs::path, fs::directory_iterator, etc.
namespace fs = std::filesystem;

namespace JFLX::SDL3 {

    /**
     * @class textureHandler
     * @brief Loads, stores, and renders a collection of SDL3 textures.
     *
     * Textures are loaded from a folder and stored in a flat vector (the "layer"
     * system). Each texture gets a zero-based integer index called a *layer*,
     * which corresponds to the alphabetical order of the source files.
     * Textures can be looked up either by that index or by their filename stem
     * (filename without extension), e.g. "player_idle".
     *
     * Ownership: this class owns every SDL_Texture* it creates. Call cleanup()
     * (or just let the destructor run) to free GPU memory.
     */
    class textureHandler {
    private:
        /// The SDL renderer used to create and draw all textures.
        /// Not owned by this class — must outlive the textureHandler instance.
        SDL_Renderer* renderer = nullptr;

        /// Flat list of loaded textures. Index == layer number.
        /// textures[0] is the first texture loaded (alphabetically), etc.
        std::vector<SDL_Texture*> textures;

        /// Maps a texture's filename stem (e.g. "background") to its layer index.
        /// Populated by loadTextureFolder() alongside the textures vector.
        std::unordered_map<std::string, int> textureLayers;

    public:
        /**
         * @brief Constructs the handler and binds it to an existing SDL_Renderer.
         * @param renderer  A valid, non-null SDL_Renderer*. Must remain alive for
         *                  the entire lifetime of this textureHandler.
         */
        explicit textureHandler(SDL_Renderer* renderer) : renderer(renderer) {}

        /**
         * @brief Loads every image file found in @p folderPath as a texture.
         *
         * Files are iterated via std::filesystem and loaded with SDL_image.
         * Each successfully loaded texture is appended to the textures vector
         * and registered in textureLayers under its filename stem.
         *
         * @param folderPath  Absolute or relative path to the image folder.
         * @return true  if at least one texture was loaded without errors,
         *         false on I/O or SDL errors.
         */
        bool loadTextureFolder(const std::string& folderPath);

        /**
         * @brief Returns the total number of loaded textures (= highest layer + 1).
         * @note  [[nodiscard]] — the compiler will warn if the return value is ignored.
         */
        [[nodiscard]] int layers() const {
            return static_cast<int>(textures.size());
        }

        /**
         * @brief Looks up the layer index for a texture by its filename stem.
         * @param name  Filename without extension, e.g. "player_run".
         * @return The layer index, or -1 if no texture with that name was loaded.
         */
        int getTextureLayer(const std::string& name) const;

        /**
         * @brief Direct access to a raw SDL_Texture* by layer index.
         * @param layer  Zero-based index into the textures vector.
         * @return Pointer to the SDL_Texture, or nullptr if @p layer is out of range.
         */
        SDL_Texture* getTexture(int layer) const;

        // ------------------------------------------------------------------ //
        //  Render overloads                                                    //
        //  Both variants share the same visual parameters; they differ only    //
        //  in how the target texture is identified.                            //
        // ------------------------------------------------------------------ //

        /**
         * @brief Renders a texture identified by its layer index.
         *
         * Positions the texture so that its top-left corner is at (x, y),
         * scales it, rotates it around its centre, optionally flips it, and
         * applies a colour/alpha modulation before drawing.
         *
         * @param layer     Zero-based layer index.
         * @param x         Destination X coordinate in renderer logical pixels.
         * @param y         Destination Y coordinate in renderer logical pixels.
         * @param scaleX    Horizontal scale factor (1.0 = original width).
         * @param scaleY    Vertical scale factor   (1.0 = original height).
         * @param rotation  Clockwise rotation in degrees around the texture centre.
         * @param flip      SDL_FLIP_NONE / SDL_FLIP_HORIZONTAL / SDL_FLIP_VERTICAL.
         * @param color     RGBA colour modulation; {255,255,255,255} = no change.
         * @return true on success, false if the layer is invalid or SDL reports an error.
         */
        bool renderTexture(int layer,
                           float x, float y,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip     = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 255}) const;

        /**
         * @brief Renders a texture identified by its filename stem.
         *
         * Convenience overload — internally resolves the name to a layer index
         * via textureLayers and then delegates to the index-based overload.
         *
         * @param name  Filename stem, e.g. "background".
         *              All other parameters are identical to the layer overload.
         * @return true on success, false if the name is not found or SDL errors out.
         */
        bool renderTexture(const std::string& name,
                           float x, float y,
                           float scaleX         = 1.f,
                           float scaleY         = 1.f,
                           double rotation      = 0.0,
                           SDL_FlipMode flip     = SDL_FLIP_NONE,
                           SDL_Color color      = {255, 255, 255, 255}) const;

        /**
         * @brief Destroys all loaded SDL_Textures and clears the internal containers.
         *
         * Safe to call multiple times (SDL_DestroyTexture on a null pointer is a no-op,
         * and the containers are cleared after destruction so nothing is double-freed).
         * Called automatically by the destructor.
         */
        void cleanup();

        /// Destructor — guarantees GPU memory is released when the object goes out of scope.
        ~textureHandler() { cleanup(); }
    };

} // namespace JFLX::SDL3

#endif // JFLX_SDL3_TEXTUREHANDLER_HPP
