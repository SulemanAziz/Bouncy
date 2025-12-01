#undef main //Linker nonsense, otherwise it wants me to start with SDL_main() instead of int main()

#include <iostream>
#include <vector>
#include<map>
#include <SDL2/SDL.h>

#define WIDTH 640
#define HEIGHT 480
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000

#define G_a 1.2 //Gravity

std::map<int, bool> x_Map;
std::map<int, bool> y_Map; //Coordinate Map for level

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

void EngineStep(Circle* Circle){

    //Based on the velocity components, we now want to move the circle on the screen!
    Circle->x+=Circle->v_x;
    Circle->y+=Circle->v_y;

    Circle->v_y+=G_a; //Falling down from gravity

    //Boundary Collision detection:

    if(Circle->y+Circle->radius>HEIGHT){
        Circle->v_y*=-0.75;
        Circle->y = HEIGHT-Circle->radius+Circle->v_y;
    }

    if(Circle->y-Circle->radius<0){
        Circle->v_y*=-0.75;
        Circle->y = Circle->radius+Circle->v_y;
    }

    if(Circle->x+Circle->radius>WIDTH){
        Circle->v_x*=-0.75;
        Circle->x = WIDTH-Circle->radius+Circle->v_x;
    }

    if(Circle->x-Circle->radius<0){
        Circle->v_x*=-0.75;
        Circle->x = Circle->radius+Circle->v_x;
    }

    //Platform Collisions:

    if((x_Map[Circle->x-Circle->radius]==true || x_Map[Circle->x+Circle->radius]==true) && (y_Map[Circle->y-Circle->radius]==true || y_Map[Circle->y+Circle->radius]==true)){

       if(x_Map[Circle->x-Circle->radius]==true){
            Circle->v_x*=-0.75;
            Circle->x = Circle->x + 0.75;
       }
       else if(x_Map[Circle->x+Circle->radius]==true){
            Circle->v_x*=-0.75;
            Circle->x = Circle->x - 0.75;
       }

       if(y_Map[Circle->y-Circle->radius]==true){
            Circle->v_y*=-0.75;
            Circle->y = Circle->y + 0.75;
       }
       else if(y_Map[Circle->y+Circle->radius]==true){
            Circle->v_y*=-0.75;
            Circle->y = Circle->y - 0.75;
       }
    }
}

void CreateMap(){

    int Width = WIDTH;
    int Height = HEIGHT;

    for(Width; Width>0; Width--){
        x_Map.insert({Width, false});
    }

    for(Height; Height>0; Height--){
        y_Map.insert({Height, false});
    }
}

void CreateCollisions(SDL_Rect (&Obstacle)[]){

    for(int i=0; i<sizeof(&Obstacle)/4; i++){

        int start_x = Obstacle[i].x;
        int start_y = Obstacle[i].y;

        int end_x = Obstacle[i].x + Obstacle[i].w;
        int end_y = Obstacle[i].y + Obstacle[i].h;

        for(start_x; start_x<=end_x; start_x++){
            x_Map[start_x] = true;
        }

        for(start_y; start_y<=end_y; start_y++){
            y_Map[start_y] = true;
        }
    }

}

void RenderObstacle(SDL_Surface* surface, SDL_Rect (&Obstacle)[]){
    for(int i=0;i<sizeof(&Obstacle)/4;i++){
        SDL_FillRect(surface, &Obstacle[i], COLOR_WHITE);
    }
}

int main(int argc, char* argv[]){


    std::cout<<"Starting Program\n";
    std::cout<<"Generating Map...\n";

    CreateMap();

    std::cout<<"Map Generated\n";
    std::cout<<"Generating Collision Map...\n";

    SDL_Rect Obstacle[] = { (SDL_Rect) {200,400,70,80}, (SDL_Rect) {300,300,70,80} };

    CreateCollisions(Obstacle);

    std::cout<<"Collision Map Generated\n";

    //Basic Initialization stuff:
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Bouncy",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIDTH,HEIGHT,0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);

    //Start of Rendering
    SDL_Rect erase_screen = (SDL_Rect) {0,0,WIDTH,HEIGHT};

    bool simul_on = true;
    Circle circle(10,50,35,0,0);
    SDL_Event event; //This is to see if the window has been closed or not

    while(simul_on==true)
    {
        while(SDL_PollEvent(&event))
        {
            if(event.type==SDL_QUIT){
                simul_on=false;
                break;
            }

            if(event.type==SDL_KEYDOWN)
            {
                switch(event.key.keysym.sym)
                {
                    case SDLK_LEFT:
                    circle.v_x-=7;
                    break;

                    case SDLK_RIGHT:
                    circle.v_x+=7;
                    break;

                    case SDLK_UP:
                    circle.v_y-=7;
                    break;

                    case SDLK_DOWN:
                    circle.v_y+=7;
                    break;
                }
            }
        }

        SDL_FillRect(surface,&erase_screen,COLOR_BLACK); //Clears the screen each frame to draw the circle again
        RenderObstacle(surface, Obstacle);

        FillCircle(surface, &circle);

        EngineStep(&circle);

        SDL_UpdateWindowSurface(window);
        SDL_Delay(32); //30 FPS
    }
}
