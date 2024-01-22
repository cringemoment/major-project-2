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

struct Weapon {
  int type;
  char name[50];
  float damage;
  int cooldown;
  int cost;
  float hitchance;
  float critchance;
};

struct Armor {
  int type;
  char name[50];
  float damage;
  float armorpoints;
  int cost;
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
  int gold;
  int weaponsowned[10];
  int weaponcooldowns[10];
  struct Cats catpanion;
  struct Armor armorowned;
};

//just a ton of boilerplate, nothing interesting going on here
typedef struct Cats cats;
typedef struct Weapon weapon;
typedef struct Player player;
typedef struct Armor armor;

player players[2] = {{"", 20, 0, 30, 500, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}, {"", 20, 0, 30, 550, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}};

cats allcats[6] = {{"Alice", "Calico, cuddly and loves attention. Claws. Hard.", -4, 3, 40}, {"Elise", "Tabby, not the brightest but always a mood booster", 3, -1, 25}, {"Jane", "Tuxedo, bit feisty but will alawys be there", 1, 2, 25}, {"Mary", "Ragdoll, scared of fights but steps up when needed", 2, 1, 20}, {"Lily", "Bombay, unlucky as most black cats are, but hits when needed", 1, 4, 10}, {"Emily", "Maine Coon, majestic, but very self absorbed. Don't talk behind her back", -1, 2, 30}};

//0 = energy, 1 = kinetic, 2 = melee, 3 = magic
weapon armory[10] =    {{0, "Tachyon Lance", 30, 20, 1500, 3},
                        {0, "Plasma Launcher", 4, 2, 700, 6},
                        {0, "Zip Zap", 1, 1, 500, 6},
                        {1, "Gauss Canon", 10, 5, 1500, 3},
                        {1, "Carbine Gun", 3, 1, 1000, 6},
                        {1, "Hand Gun", 1, 2, 500, 6},
                        {2, "Dark Saber", 20, 5, 1000, 8},
                        {2, "Plasma Knife", 3, 2, 500, 8},
                        {2, "Reaper's Scythe", 15, 5, 800, 9},
                        {3, "Aetherophasic Engine", 20, 1, 800, 15}
};

armor allarmor[10] = {{0, "Dark Matter Shield", 4000, 1, 1100},
                      {0, "Energy Deflector", 2000, 1, 800},
                      {0, "Basic Shield", 700, 1, 500},
                      {1, "Gravitational Refractor", 1000, 0.5, 1100},
                      {1, "Hyper Array", 1000, 0.8, 800},
                      {1, "Pulsing Array", 500, 0.9, 500},
                      {2, "Zero-point Armor", 300, 0.3, 1100},
                      {2, "Power Armor", 300, 0.4, 800},
                      {2, "Space Armor", 200, 0.6, 500},
                      {3, "Dimensional Lens", 500, 0.5, 1000},
};

cats catshop[3];
weapon weaponshop[5];

//variables that will actually be juggled by the game
int menustate = 0;
int shopstate = 0;
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

void nextturn() {
  for(int i = 0; i < 10; i++) {
    if(players[currentplayer].weaponcooldowns[i] > 0) {
      players[currentplayer].weaponcooldowns[i] -= 1;
    }
  }
  currentplayer = (currentplayer + 1) % 2;
}

//this is mostly rendering stuff
//unfortunately just a placeholder rn, not much i can do
void drawtext(SDL_Renderer* rend, char text[], int x, int y, int size, int red, int green, int blue) {
  printf("%s\n", text);
}

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
void drawmenubutton(SDL_Renderer* rend, menubutton button, int red, int green, int blue) {
  drawrect(rend, button.x, button.y, menubuttonx, menubuttony, red, green, blue);
}

void drawshopbutton(SDL_Renderer* rend, genericshop button) {
  if(button.bought == 1) {
    drawrect(rend, button.x, button.y, menubuttonx, menubuttony, 255, 0, 0);
  } else {
    drawrect(rend, button.x, button.y, menubuttonx, menubuttony, 0, 255, 0);
  }
}

//button arrays
menubutton menubuttons[5] = {{0, 360, 2}, {0, 420, 3}, {540, 420, 1}, {100, 360, 4}, {100, 420, 5}};
menubutton shopstateswitcher[2] = {{0, 120, 0}, {540, 120, 0}};
genericshop catshopbuttons[3] = {{0, 200}, {270, 200}, {540, 200}};

//menu drawers
void drawcatshop(SDL_Renderer* rend) {
  for(int i = 0; i < 3; i++) {
    drawshopbutton(rend, catshopbuttons[i]);
  }
}

void drawweapon(SDL_Renderer* rend) {
  weapon weapontorender = armory[shopstate];
  //drawtext(rend, weapontorender.name, 200, 50, 50, 255, 255, 255);
  genericshop menubutton = {270, 350};

  if(players[currentplayer].weaponsowned[shopstate] == 1) {
    menubutton.bought = 1;
  }

  drawshopbutton(rend, menubutton);
}

//duplicate code because armor is ever so slightly the same but also different
void drawarmor(SDL_Renderer* rend) {
  armor weapontorender = allarmor[shopstate];
  drawtext(rend, weapontorender.name, 200, 50, 50, 255, 255, 255);
  genericshop menubutton = {270, 350};

  drawshopbutton(rend, menubutton);
}

void drawstats (SDL_Renderer* rend) {
  char currentplayerdisplay[100];
  sprintf(currentplayerdisplay, "Player %d's turn\nHealth:%i\nGold:%i\n", currentplayer + 1, players[currentplayer].health, players[currentplayer].gold, 50);
  drawtext(rend, currentplayerdisplay, 300, 300, 64, 255, 255, 255);
}

//choosing what weapon to use
void drawweaponchoices (SDL_Renderer* rend) {
  int counter = 0;
  for(int y = 100; y < 200; y += 60) {
    for(int x = 0; x < 540; x += 100) {
      genericshop menubutton;
      menubutton.x = x;
      menubutton.y = y;

      drawshopbutton(rend, menubutton);
    }
  }
}

//whole function for checking if its within the range lol! silly silly silly
bool iswithinbutton(int mx, int my, int bx, int by, int bxs, int bys) {
  return(mx >= bx && mx <= bx + bxs && my >= by && my <= by + bys);
}

//i have to write logic for the code??? urghhh cant a girl catch a break
void applycat(cats catchosen) {
  players[currentplayer].health += catchosen.healthbuff;
  players[currentplayer].damage += catchosen.damagebuff;
  players[currentplayer].critchance += catchosen.critchancebuff;
}

int consc = 0;
void rest() {
  players[currentplayer].gold += 150 + (consc * 50);
  consc += 1;
  nextturn();
}

//a lot of the graphics code is stolen from somewhere else. problematic? maybe, but thats coding for you
int main(int argc, char* argv[]) {
  //darn classes

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
                applycat(catshop[i]);

                currentplayer = (currentplayer + 1) % 2;
                bought += 1;

                if(bought == 2) {
                  printf("Selected %s and %s\n", players[0].catpanion.name, players[1].catpanion.name);
                  menustate = 1;
                }
              }
            }
          }

          if(menustate > 0) {
            for(int i = 0; i < 3; i++) {
              if(iswithinbutton(x_pos, y_pos, menubuttons[i].x, menubuttons[i].y, menubuttonx, menubuttony)) {
                menustate = menubuttons[i].menustate;
                shopstate = 0;
              }
            }

            //action buttons, again; i wish the clicking code could be more tied to the button code but whatevsss
            if(menustate == 1) {
              for(int i = 3; i < 4; i++) {
                if(iswithinbutton(x_pos, y_pos, menubuttons[i].x, menubuttons[i].y, menubuttonx, menubuttony)) {
                  menustate = menubuttons[i].menustate;
                  shopstate = 0;
                }
              }

              if(iswithinbutton(x_pos, y_pos, menubuttons[4].x, menubuttons[4].y, menubuttonx, menubuttony)) {
                rest();
              }

            }

            if(menustate == 2 || menustate == 3) {
              //sliding around the items in the shop menu
              for(int i = 0; i < 2; i++) {
                if(iswithinbutton(x_pos, y_pos, shopstateswitcher[i].x, shopstateswitcher[i].y, menubuttonx, menubuttony)) {
                  //super super sloppy, whatever lol
                  if(i == 1) {
                    shopstate = (shopstate + 1) % 10;
                  } else {
                    shopstate = (shopstate - 1) % 10;
                    if(shopstate < 0) {shopstate += 10;}
                  }
                }
              }

              //buying stuff
              if(menustate == 2) {
                if(iswithinbutton(x_pos, y_pos, 270, 350, menubuttonx, menubuttony) && players[currentplayer].weaponsowned[shopstate] != 1 && players[currentplayer].gold > armory[shopstate].cost) {
                  players[currentplayer].gold -= armory[shopstate].cost;
                  players[currentplayer].weaponsowned[shopstate] = 1;
                }
              }

              //i dont love how sloppy this code is and im sure theres some hacky workaround but thatd probably be worse
              if(menustate == 3) {
                if(iswithinbutton(x_pos, y_pos, 270, 350, menubuttonx, menubuttony) && players[currentplayer].gold > allarmor[shopstate].cost) {
                  players[currentplayer].gold -= allarmor[shopstate].cost;
                  players[currentplayer].armorowned = allarmor[shopstate];
                }
              }

            }
          }
          drawstats(rend);
          break;
      default:
        break;
      }
    }

    //button drawing corner!!!!!
    if(menustate > 0) {
      for(int i = 0; i < 2; i++) {
        drawmenubutton(rend, menubuttons[i], 255, 0, 0);
      }

      //action buttons; probably only want to show them on the first menu
      if(menustate == 1) {
        drawmenubutton(rend, menubuttons[3], 0, 0, 255);
        drawmenubutton(rend, menubuttons[4], 255, 0, 180);
      }

      //back button and shop switch buttons
      if(menustate > 1) {
        drawmenubutton(rend, menubuttons[2], 0, 0, 255);
        if(menustate == 2 || menustate == 3) {
          for(int i = 0; i < 2; i++) {
            drawmenubutton(rend, shopstateswitcher[i], 255, 150, 0);
          }
        }
      }

      //weapons!!!!
      if(menustate == 2) {
        drawweapon(rend);
      }

      if(menustate == 3) {
        drawarmor(rend);
      }

    }

    if(menustate == 4) {
      drawweaponchoices(rend);
    }

    if(menustate == 0) {
      char selectiontext[100];
      sprintf(selectiontext, "Player %d, pick your cat friend", currentplayer + 1, 50);
      //drawtext(rend, selectiontext, 100, 100, 64, 255, 255, 255);

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
