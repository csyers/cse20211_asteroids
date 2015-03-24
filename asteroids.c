#include <stdio.h>
#include <math.h>
#include "gfx3.h"
#include "asteroidsTfn.h"
#include <time.h>
#include <stdlib.h>

static int XSIZE = 800;
static int YSIZE = 800;
static int LARGE = 80;
static int MEDIUM = 40;
static int SMALL = 20;
static int L_SPEED = 5;


int main()
{
  // initializations
  srand(time(NULL));					// seed random number generator with time
  Ship ship, *pship = &ship;				// A ship objects named ship and a pointer to that ship
  Bullet *bhead = NULL, *b = NULL;			//pointers to bullet used to make a LL of bullets
  int lives=3;						//lives
  int score = 0;					// score counter. Starts at 0
  int isdrawn = 1, hasrun = 0;				// is drawn changes based on invicibility of ship, hasrun is for options at termination of program
  char c;						// char variable for choice of user
  Asteroid *head, *p;					// Pointers to Asteroid struct. head (head pointer) p (for iteration)
  int level = 1;					// numAsteroids = determined by level
  //creation of linked list of asteroids
  head = init_asteroids(level);			// initialize linked list of Asteroids
  init_ship(pship,lives);						// initialize a ship
  update_ship(pship,M_PI/10,1);				//fixes a drawing bug.
  update_ship(pship,-M_PI/10,1);

  gfx_open(XSIZE,YSIZE,"Asteroids");
  c = menu();						// displays the menu, returns either q, p, or i
  while(c!='q')
  {
    hasrun = 1;						// indicates the game has been played
    gfx_clear();
    p = head;						// resets the pointer to the beginning of the linked list

    //draw functions
    if(isdrawn==1){					//makes the ship flicker to indicate period of invincibility
      draw_ship(pship);
    }
    if(pship->inv%5==1 && pship->inv!=0)		//invincibility is an integer attribute for a ship. This code makes it flicker while invincible
    {
      if(isdrawn) isdrawn = 0;
      else isdrawn = 1;
    }
    if(pship->inv==0) isdrawn=1;			//when invincibility runs out, you still want it to draw
    draw_bullet(bhead);
    draw_asteroids(head);
    draw_score(score);
    draw_lives(pship);
    draw_level(level);
    gfx_flush();

    //update functions
    update_ship(pship,0,.993);				//update_ship() when outside of if(event_waiting) is only responsible for decelerating the ship
    update_bullets(&bhead);				//updates position and lifespan of each asteroid
    update_asteroids(head);				// updates the position of each asteroid

    usleep(20000);

    //collision detection
    head = detect_collision_ship(head,pship->xnose,pship->ynose,&score,&lives,pship);	//the ship has three collision points, the nose, left wing and right wing
    head = detect_collision_ship(head,pship->xwing1,pship->ywing1,&score,&lives,pship);
    head = detect_collision_ship(head,pship->xwing2,pship->ywing2,&score,&lives,pship);
    head = detect_collision_bullet(&bhead,head,&score);	

    if(!head)						//if all asteroids have been destroyed, move on to next level
    {
      level++;						
      if(level%3==0) lives++;				//every three levels you gain a life
      head = init_asteroids(level);			//create number of asteroids equal to level
      init_ship(pship,lives);				//initialize your ship
      //gfx_wait();
    }
    if(pship->lives==0)
    {
      end_screen(score); //ends game
    }

    if(gfx_event_waiting())
    {
      c = gfx_wait(); //wait for event

      switch(c){
	case 'R':  				//up arrow
	  pship->speed=4;  				//the ship instantaneously has a v of 3
	  update_ship(pship,0,1.1);  		//ship is updated immediately
	  break;
	case 'Q':
	  update_ship(pship,M_PI/10,1);		//left arrow
	  break;				//the ship stops accelerating forward and rotates a set angle
	case 'S':
	  update_ship(pship,-M_PI/10,1);		//right arrow
	  break;				//the ship stops accelerating forward and rotates a set angle
	case ' ':
	    bhead = init_bullet(bhead, pship);	//when the player shoots, a new node is created
	  break;
        case 'p':				// pauses when the player presses 'p'
          pause_screen();
          c = 'z';
          while(c!='p'){
            c = gfx_wait();
          }
          break;
      }
    }
  }
  if(hasrun) end_screen(score);			// displays the end screen only if the user has played for some time
  return 0;
}
