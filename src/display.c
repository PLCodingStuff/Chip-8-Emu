#include <stdio.h>
#include <stdbool.h>
#include <SDL3/SDL.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600


#define IDENTITY(NAME) NAME
#define PRESS_KEY(NAME) KEY_PRESS_SURFACE_ ## NAME

#define __BASIC_KEYS(INNER, OUTER) \
    OUTER(INNER(UP)) \
    OUTER(INNER(DOWN)) \
    OUTER(INNER(LEFT)) \
    OUTER(INNER(RIGHT)) 

#define BASIC_KEYS(X) __BASIC_KEYS(X, IDENTITY)

#define KEYS(X) \
    __BASIC_KEYS(PRESS_KEY, X) \
    X(PRESS_KEY(DEFAULT))

#define X(key) key,
    
enum KeyPressSurfaces {    
    KEYS(X)
    PRESS_KEY(TOTAL)
};

#undef X

// The window we'll be rendering.
SDL_Window *gWindow = NULL;

// The surface contained by the window.
SDL_Texture *gWindowTexture = NULL;

// Window Renderer
SDL_Renderer *gRenderer = NULL;

// The image that corresponds to a keypress
SDL_Texture *gKeyPressTexture[ PRESS_KEY(TOTAL) ];

SDL_Texture *gCurrentTexture = NULL;


// Starts up DSL and creates window
bool init() {
    bool success = true;
    if (SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        printf("SDL could not be initialized! SDL_Error : %s\n", SDL_GetError());
        return false;
    }

    gWindow = SDL_CreateWindow("Hello Texture World", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (gWindow == NULL) {
        printf("SDL Window cannot be initialized! SDL_Error: %s\n", SDL_GetError());
        success = false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, NULL);
    if(gRenderer == NULL){
        printf("SDL Renderer cannot be initialized! SDL_Error: %s\n ", SDL_GetError());
        success = false;
    }

    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    return success;
}

// Loads individual image
SDL_Texture *loadTexture(const char *path) {
    SDL_Surface *loadedSurface = SDL_LoadBMP(path);
    SDL_Texture *loadedTexture = NULL;

    if (loadedTexture == NULL)
        printf("Unable to load image %s! SDL_Error: %s\n", path, SDL_GetError());
    else {
        /* Surface 
        // Optimize blit, by predefining pixel format from 24 bits (BMP image default) to 32 bits (Display default)
        optimizedTexture = SDL_ConvertTexture(loadedTexture, gWindowTexture->format);
        if(optimizedTexture == NULL)
            printf("Unable to optimize image %s! SDL_Error: %s\n", path, SDL_GetError());

        */

        loadedTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
        if(loadedTexture == NULL)
            printf("Unable to convert surface %s to texture! SDL_Error: %s\n", path, SDL_GetError());
        SDL_DestroyTexture(loadedTexture);
    }

    return loadedTexture;
}

bool loadKeyMedia(enum KeyPressSurfaces key, const char *filename) {
    bool success = true;
    
    gKeyPressTexture[key] = loadTexture(filename);
    if (gKeyPressTexture[key] == NULL) {
        printf("Unable to load image %s! SDL_Error: %s\n", filename, SDL_GetError());
        success = false;
    }

    return success;
}

// Loads media
bool loadMedia(const char *filename) {
    bool success = true;

    const char files[ PRESS_KEY(TOTAL) ][10] = {"up.bmp", "down.bmp", "left.bmp", "right.bmp", "press.bmp"};

    #define X(key) if (!loadKeyMedia(key, files[key])) success = false;
        KEYS(X)
    #undef X

    return success;
}

// Frees media and shuts down SDL
void closeSDL() {
    SDL_DestroyTexture(gCurrentTexture);
    gCurrentTexture = NULL;

    #define X(key) { \
        SDL_DestroyTexture(gKeyPressTexture[key]); \
        gKeyPressTexture[key] = NULL; \
    }
    KEYS(X)
    #undef X

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    SDL_Quit();
}

int main(int argc, char* argv[]) {
    if (!init()) {
        printf("Failed to initialize. Exiting\n");
        return 1;
    }

    if (!loadMedia("")) {
        printf("Failed to load media.\n");
        closeSDL();
        return 1;
    }

    SDL_Event e;
    bool quit = false;
    SDL_FRect stretchRect;
    stretchRect.x = 0.0;
    stretchRect.y = 0.0;
    stretchRect.w = (float) WINDOW_WIDTH;
    stretchRect.h = (float) WINDOW_HEIGHT;

    gCurrentTexture = gKeyPressTexture[ PRESS_KEY(DEFAULT) ];
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT)
                quit = true;
            else if (e.type == SDL_EVENT_KEY_DOWN) {
                #define GENERATE_CASE(key) {\
                    case SDLK_ ## key: \
                        gCurrentTexture = gKeyPressTexture[PRESS_KEY(key)]; \
                        break; \
                }

                switch (e.key.key) {
                    BASIC_KEYS(GENERATE_CASE)
                    default:
                        gCurrentTexture = gKeyPressTexture [ PRESS_KEY(DEFAULT) ];
                        break;
                }
                #undef GENERATE_CASE
            }
        }

        SDL_RenderClear(gRenderer);
        SDL_RenderTexture(gRenderer, gCurrentTexture, NULL, &stretchRect);

        SDL_RenderPresent(gRenderer);
        /* Surfaces
        // Apply the current image
        SDL_BlitSurfaceScaled(gCurrentSurface, NULL, gWindowSurface, &stretchRect, SDL_SCALEMODE_NEAREST);
        
        // Update the surface
        SDL_UpdateWindowSurface(gWindow);
        */
    }

    // Free allocated sources and quit SDL
    closeSDL();

    return 0;
}