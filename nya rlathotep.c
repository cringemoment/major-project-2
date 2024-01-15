/*
im not even sure where to begin on building this thing
i can compile it on my computer fine but ive probably set it up so weird itll brick
just run compileme.py ig
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define WIDTH 640
#define HEIGHT 480
#define SIZE 50
#define FPS 60

#define basecritchance 30
#define basehealth 20

struct Weapon {
  char name[50];
  char description[100];
  int type;
  int cost;
  float damage;
  float hitchance;
  float critchance;
};

struct Cats {
  char name[50];
  char description[100];
  float healthbuff;
  float damagebuff;
  float critchancebuff;
};

//20 health, 30 crit chance
struct Player {
  char name[50];
  int health;
  int damage;
  int critchance;
  struct Weapon weapons[3];
  struct Cats catpanion;
};

//just a ton of boilerplate, nothing interesting going on here
typedef struct Cats cats;
typedef struct Weapon weapons;
typedef struct Player player;

player players[2] = {{"", 20, 0, 30}, {"", 20, 0, 30}};

cats allcats[6] = {{"Alice", "Calico, cuddly and loves attention. Claws. Hard.", -4, 2, 40}, {"Elise", "Tabby, not the brightest but always a mood booster", 3, -1, 25}, {"Jane", "Tuxedo, bit feisty but will alawys be there", 1, 2, 25}, {"Mary", "Ragdoll, scared of fights but steps up when needed", 2, 1, 20}, {"Lily", "Bombay, unlucky as most black cats are, but hits when needed", 1, 4, 10}, {"Emily", "Maine Coon, majestic, but very self absorbed. Don't talk behind her back", -1, 3, 30}};

weapons allweapons[2] = {{"one", "first", 1, 50, 20, 20, 20}, {"two", "second", 2, 50, 30, 30, 30}};

cats catshop[3];
weapons weaponshop[5];

//variables that will actually be juggled by the game
int menustate = 0;
int bought = 0;
int currentplayer;

//functions!!!!!! :3:3:3:3:3
int randint (int min, int max){
  return rand() % (max - min + 1) + min;
}

void populatecatshop() {
  int catschosen[3] = {-1, -1, -1};
  int catsadded = 0;
  while(catsadded < 3) {
    int catchosen = randint(0, 5);
    if(catchosen != catschosen[0] && catchosen != catschosen[1] && catchosen != catschosen[2]) {
      catschosen[catsadded] = catchosen;
      catsadded += 1;
    }
  }
  for(int i = 0; i < 3; i++) {
    catshop[i] = allcats[catschosen[i]];
  }
}

//this is mostly rendering stuff
void drawimage(SDL_Renderer* renderer, int x, int y, const char* fileName) {//fully upfront this code was not written by me
    SDL_Surface* surface = IMG_Load(fileName);
    if (surface == NULL) {
        fprintf(stderr, "Unable to load image! SDL_image Error: %s\n", IMG_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        fprintf(stderr, "Unable to create texture! SDL_Error: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return;
    }

    SDL_FreeSurface(surface);

    SDL_Rect destRect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &destRect);
    SDL_DestroyTexture(texture);
}

void drawrect(SDL_Renderer* rend, int x, int y, int xsize, int ysize, int red, int green, int blue) {
  SDL_Rect temprect = {x, y, xsize, ysize};
  SDL_SetRenderDrawColor(rend, red, green, blue, 255);
  SDL_RenderFillRect(rend, &temprect);
}

//classes, ig; i cant do the list trickery like in python
struct MenuButton {
  int x;
  int y;
  int menustate;
};

struct GenericShop {
  int x;
  int y;
  int bought;
}; //if only i could inherit...

typedef struct MenuButton menubutton; //all of this feels very belabored but itll probably save me trouble in the long run
typedef struct GenericShop genericshop;

int menubuttonx = 100;
int menubuttony = 60;

//my idea with this is to build a lot of small functions that i know work to slowly work up to big girl buttons
void drawmenubutton(SDL_Renderer* rend, menubutton button) {
  drawrect(rend, button.x, button.y, menubuttonx, menubuttony, 255, 0, 0);
}

int width = WIDTH;

//button arrays
menubutton menubuttons[2] = {{0, 300, 0}, {0, 400, 1}};
genericshop catshopbuttons[3] = {{0, 200}, {270, 200}, {540, 200}};

//menu drawers
void drawcatshop(SDL_Renderer* rend) {
  for(int i = 0; i < 3; i++) {
    if(catshopbuttons[i].bought == true) {
      drawrect(rend, catshopbuttons[i].x, catshopbuttons[i].y, menubuttonx, menubuttony, 255, 0, 0);
    }
    else {
      drawrect(rend, catshopbuttons[i].x, catshopbuttons[i].y, menubuttonx, menubuttony, 0, 255, 0);
    }
  }
}

//whole function for checking if its within the range lol! silly silly silly
bool iswithinbutton(int mx, int my, int bx, int by, int bxs, int bys) {
  return(mx >= bx && mx <= bx + bxs && my >= by && my <= by + bys);
}

//unfortunately just a placeholder rn, not much i can do
void drawtext(SDL_Renderer* rend, char text[], int x, int y, int size, int red, int green, int blue) {
  printf("%s\n", text);
}

//a lot of the graphics code is stolen from somewhere else. problematic? maybe, but thats coding for you
int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("Error initializing SDL: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window* wind = SDL_CreateWindow("Nyaaaa~", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

  if (!wind) {
    printf("Error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

  //rend is basically the turtle
  SDL_Renderer* rend = SDL_CreateRenderer(wind, -1, render_flags);
  if (!rend) {
    printf("Error creating renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(wind);
    SDL_Quit();
    return 0;
}

  //this is where the game runs!
  bool running = true;
  int x_pos, y_pos;
  SDL_Event event;

  populatecatshop();

  while(running) {
    //always keep the clearscreen first
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderClear(rend);

    SDL_GetMouseState(&x_pos, &y_pos);

    //this event handling might look bad, but its pretty much the exact same as pygame, this time with switch statements
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          running = false;
          break;
        case SDL_MOUSEBUTTONDOWN:

          //holy guacamole is checking for button inputs annoying;

          //cat selected. maybe i should move this to a seperate function, but thats too much boilerplate even for me
          if(menustate == 0) {
            for(int i = 0; i < 3; i++) {

              if(iswithinbutton(x_pos, y_pos, catshopbuttons[i].x, catshopbuttons[i].y, menubuttonx, menubuttony) && catshopbuttons[i].bought != true) {
                catshopbuttons[i].bought = true;
                players[currentplayer].catpanion = catshop[i];

                currentplayer = (currentplayer + 1) % 2;
                bought += 1;

                if(bought == 2) {
                  printf("Selected %s and %s\n", players[0].catpanion.name, players[1].catpanion.name);
                  menustate = 1;
                }
              }
            }
          }

          if(menustate == 1) {
            for(int i = 0; i < 2; i++) {
              if(iswithinbutton(x_pos, y_pos, menubuttons[i].x, menubuttons[i].y, menubuttonx, menubuttony)) {
                menustate = menubuttons[i].menustate;
              }
            }
          }
          break;
      default:
        break;
      }
    }

    //button drawing corner!!!!!
    if(menustate == 1) {
      for(int i = 0; i < 2; i++) {
        drawmenubutton(rend, menubuttons[i]);
      }
    }

    if(menustate == 0) {
      char* selectiontext;
      sprintf(selectiontext, "Player %d, pick your cat friend", currentplayer + 1, 50);

      drawtext(rend, selectiontext, 100, 100, 64, 255, 255, 255);
      drawcatshop(rend);
    }

    /* Draw to window and loop */
    SDL_RenderPresent(rend);
    SDL_Delay(1000/FPS);
  }

  /* Release resources */
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(wind);
  SDL_Quit();
  return 0;
}
