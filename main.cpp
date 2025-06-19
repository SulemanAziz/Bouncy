#undef main //Linker nonsense, otherwise it wants me to start with SDL_main() instead of int main()

#include <iostream>
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000

#define G_a 2 //Gravity

class Circle{
    public:
        double x,y,radius; //x and y are the coordinates of the centerpoint of this circle
        double v_x,v_y; //Circle's x and y velocity component

        Circle(double x,double y,double radius,double vx,double vy){
            this->x=x;
            this->y=y;
            this->radius=radius;
            this->v_x=vx;
            this->v_y=vy;
        }

        bool atrest_y = false;
        bool atrest_x = false;
};

void FillCircle(SDL_Surface* surface,Circle* Circle){

    int border_x_low = Circle->x - Circle->radius;
    int border_y_low = Circle->y - Circle->radius;
    int border_x_high = Circle->x + Circle->radius;
    int border_y_high = Circle->y + Circle->radius;

    double distance_squared=0; //distance from centerpoint
    double radius_squared = Circle->radius*Circle->radius;

    for(int x=border_x_low; x<border_x_high; x++)
    {
        for(int y = border_y_low; y<border_y_high; y++)
        {
            //We now check if the pixels in this coordinate range fall within our circle, if so, we fill them.

            distance_squared = (((x-Circle->x)*(x-Circle->x))+((y-Circle->y)*(y-Circle->y)));

            if(distance_squared<radius_squared) //This means the pixel is within the circle!
            {
                SDL_Rect pixel = (SDL_Rect) {x,y,1,1};
                SDL_FillRect(surface, &pixel ,COLOR_WHITE);
            }

        }

    }
}

void EngineStep(Circle* Circle, SDL_Rect platform){

    //Based on the velocity components, we now want to move the circle on the screen!
    Circle->x+=Circle->v_x;
    Circle->y+=Circle->v_y;

    std::cout<<"Y Velocity: "<<Circle->v_y<<"\n";
    std::cout<<"Rest? "<<Circle->atrest_y<<"\n";
    std::cout<<"Sum of Velocity and G: "<<Circle->v_y + G_a<<"\n";

    /* if(Circle->v_y+G_a>0 && Circle->v_y+G_a<0.5){ //At what point do we declare that the Circle is at rest?
        Circle->y+=Circle->v_y;
        Circle->v_y=0;
        Circle->atrest_y=true;
    } */

    if(Circle->atrest_y==false){
        Circle->v_y+=G_a; //Falling down from gravity
    }

    //Collision detection:

    if(Circle->y+Circle->radius>HEIGHT){
        Circle->v_y*=-0.65;
        Circle->y = HEIGHT-Circle->radius+Circle->v_y;
    }

    if(Circle->y-Circle->radius<0){
        Circle->v_y*=-0.65;
        Circle->y = Circle->radius+Circle->v_y;
    }

    if(Circle->x+Circle->radius>WIDTH){
        Circle->v_x*=-0.65;
        Circle->x = WIDTH-Circle->radius+Circle->v_x;
    }

    if(Circle->x-Circle->radius<0){
        Circle->v_x*=-0.65;
        Circle->x = Circle->radius+Circle->v_x;
    }

    //How do we make it stop bouncing? It also needs to diminish v_x to simulate some sort of friction. At some point, the ball must become stationary.

    //Below is Platform Collisions:

    if(Circle->x < platform.x+platform.w && Circle->x > platform.x && Circle->y > platform.y+platform.h)
    {
        Circle->y = (platform.y+platform.h)+Circle->radius;
        Circle->v_y*=-0.65;
    }

}

SDL_Rect Spawn_Platforms(SDL_Surface* surface){

    SDL_Rect platform = (SDL_Rect) {WIDTH/2,HEIGHT/2,70,40};
    SDL_FillRect(surface, &platform, COLOR_WHITE);
    return platform;
}

int main(int argc, char* argv[]){

    std::cout<<"Starting Program\n";

    //Basic Initialization stuff:
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Bouncy",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    //Start of Rendering

    SDL_Rect erase_screen = (SDL_Rect) {0,0,WIDTH,HEIGHT};

    bool simul_on = true;
    Circle circle(100,150,35,0,0);
    SDL_Event event; //This is to see if the window has been closed or not

    while(simul_on==true)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT){
            simul_on=false;
            }

            if(event.type==SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_LEFT:
                    circle.v_x-=12;
                    break;

                    case SDLK_RIGHT:
                    circle.v_x+=12;
                    break;

                    case SDLK_UP:
                    circle.v_y-=12;
                    break;

                    case SDLK_DOWN:
                    circle.v_y+=12;
                    break;
                }
            }
        }

        SDL_FillRect(surface,&erase_screen,COLOR_BLACK); //Clears the screen each frame to draw the circle again
        FillCircle(surface, &circle);
        EngineStep(&circle,Spawn_Platforms(surface));
        SDL_UpdateWindowSurface(window);
        SDL_Delay(32);
    }
}
