#undef main //Linker nonsense, otherwise it wants me to start with SDL_main() instead of int main()

#include <iostream>
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480

struct Circle{

    int x,y,radius; //x and y are the coordinates of the centerpoint of this circle
};

void FillCircle(SDL_Surface* surface,struct Circle Circle){

    int border_x_low = Circle.x - Circle.radius;
    int border_y_low = Circle.y - Circle.radius;
    int border_x_high = Circle.x + Circle.radius;
    int border_y_high = Circle.y + Circle.radius;
}

int main(int argc, char* argv[]){

    std::cout<<"Starting Program\n";

    //Basic Initialization stuff:
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Bouncy",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    //Start of Rendering
    SDL_Rect rect = (SDL_Rect) {250,200,200,200};
    SDL_FillRect(surface, &rect, 0xffffffff);

    //Updates window with the rendered object:
    SDL_UpdateWindowSurface(window);

    //Wait 3 seconds then close the window:
    SDL_Delay(3000);

    return 0;
}
