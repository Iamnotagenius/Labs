#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iso646.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "argparse.h"
#include "gof.h"

int main(int argc, char **argv) {
    char *outdir = ".", *input_bmp, *dump_filename = "%s/%04d.bmp", filename_buffer[100];
    unsigned dumpfreq = 1, maxiter = 1024, dumpcounter = 0;
    bool gui = false;
    parse_args(argc, argv, &input_bmp, &maxiter, &dumpfreq, &outdir, &gui);
    FILE *bmp = fopen(input_bmp, "rb"), *dumpfile;
    if (bmp == NULL) {
        fprintf(stderr, "%s: %s: no such file or directory\n", argv[0], input_bmp);
        exit(1);
    }
    gof_field_t *f = get_from_bmp(bmp);

#ifdef SDL
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface *surface = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event event;

    if (gui) {
        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            fprintf(stderr, "%s: SDL Error: %s\n", argv[0], SDL_GetError());
        }
        SDL_CreateWindowAndRenderer(f->width, f->height, 0, &window, &renderer);
        SDL_SetWindowTitle(window, "Game of Life");
        SDL_SetWindowAlwaysOnTop(window, SDL_FALSE);
        surface = SDL_LoadBMP(input_bmp);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }
#endif

    for (int iteration = 0; iteration < maxiter; ++iteration) {
        perform_iteration(f);
        if (iteration % dumpfreq == 0) {
            sprintf(filename_buffer, dump_filename, outdir, dumpcounter);
            dumpfile = fopen(filename_buffer, "wb");
            if (dumpfile == NULL) {
                fprintf(stderr, "%s: %s: No such file or directory\n", argv[0], filename_buffer);
                exit(1);
            }
            dump_field(f, dumpfile);
#ifdef SDL
            if (gui) {
                static SDL_RWops *bmprw = NULL;
                SDL_Delay(42);
                SDL_FreeSurface(surface);
                rewind(dumpfile);
                bmprw = SDL_RWFromFP(dumpfile, SDL_FALSE);
                surface = SDL_LoadBMP_RW(bmprw, 0);
                texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_RenderCopy(renderer, texture, NULL, NULL);
                SDL_RenderPresent(renderer);
                SDL_RWclose(bmprw);
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        goto end_main;
                    }
                }
            }
#endif
            fclose(dumpfile);
            dumpcounter++;
        }
    }
#ifdef SDL
end_main:
    SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
    SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	SDL_Quit();
#endif
    return 0;
}
