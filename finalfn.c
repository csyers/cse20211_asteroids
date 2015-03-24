#include <stdio.h>
#include <math.h>
#include "gfx3.h"
#include "asteroidsTfn.h"
#include <time.h>
#include <stdlib.h>

// size of graphics window
static int XSIZE = 800;
static int YSIZE = 800;

// radius of asteroids
static int LARGE = 80;
static int MEDIUM = 40;
static int SMALL = 20;

// different size asteroid speeds
static float L_SPEED = .5;		
static float M_SPEED = 1;	
static float S_SPEED = 2;		


void init_ship(Ship *p, int lives)
{
  p->xpos = XSIZE/2;
  p->ypos = YSIZE/2;
  p->speed = .1;
  p->angle = M_PI/2;
  p->lives = lives;						//lives are important
  p->xnose = p->xpos+15*cos(p->angle);			//nose and wing positions are calculated for collisions
  p->ynose = p->ypos-15*sin(p->angle);
  p->xwing1 = p->xpos+10*cos((3*M_PI/4)+(p->angle));
  p->ywing1 = p->ypos-10*sin((3*M_PI/4)+(p->angle));
  p->xwing2 = p->xpos+10*cos((5*M_PI/4)+(p->angle));
  p->ywing2 = p->ypos-10*sin((5*M_PI/4)+(p->angle));
  p->inv = 300;
}

void draw_ship(Ship *p)
{
  gfx_line(p->xpos,p->ypos,p->xwing1,p->ywing1);	//the function really does its job well
  gfx_line(p->xpos,p->ypos,p->xwing2,p->ywing2);
  gfx_line(p->xnose,p->ynose,p->xwing1,p->ywing1);
  gfx_line(p->xnose,p->ynose,p->xwing2,p->ywing2);
}

void update_ship(Ship *p, float angle1, float speed1)
{
  p->angle += angle1;

  if(p->speed>10)		//set a max speed
    p->speed=10;
  else
    p->speed *= speed1;

  p->xpos += p->speed*cos(p->angle);		
  p->ypos -= p->speed*sin(p->angle);

  if(p->xpos>XSIZE+15) p->xpos=0-15;		//if the ship reaches the edge of the screen
  if(p->xpos<0-15) p->xpos=XSIZE+15;
  if(p->ypos>YSIZE+15) p->ypos=0-15;
  if(p->ypos<0-15) p->ypos=YSIZE+15;		//come back around

  p->xnose = p->xpos+15*cos(p->angle);
  p->ynose = p->ypos-15*sin(p->angle);
  p->xwing1 = p->xpos+10*cos((3*M_PI/4)+(p->angle));
  p->ywing1 = p->ypos-10*sin((3*M_PI/4)+(p->angle));
  p->xwing2 = p->xpos+10*cos((5*M_PI/4)+(p->angle));
  p->ywing2 = p->ypos-10*sin((5*M_PI/4)+(p->angle));
  p->inv--;					//decreases invincibility
  if(p->inv<0) p->inv=0;
}

Bullet * init_bullet(Bullet *bhead, Ship *s)	//create a bullet and add it to the linked list
{
  Bullet *b = (Bullet *) malloc(sizeof(Bullet));
  Bullet *curr = bhead;				//curr will help us add bullets to the list
  b->xpos = s->xnose;				//the position/angle will be the same as the ship
  b->ypos = s->ynose;
  b->speed = 20;
  b->angle = s->angle;
  b->lifespan = 30;
  b->xend = b->xpos+(10*cos(b->angle));	
  b->yend = b->ypos-(10*sin(b->angle));
  b->nextb = NULL;


  if(!bhead) return b; 				//if bhead is pointing to NULL i.e. we are initializing the first bullet, we assign bhead to the bullet

  if(bhead->nextb == NULL){			//Case where we are initializing the second bullet
    bhead->nextb = b;
    return bhead;
  }

  while(curr){					//curr traverses the linked list until it reaches the end, then adds b to the end
    if(curr->nextb==NULL){
      curr->nextb = b;
      break;
    }
    curr = curr->nextb;
  }
  return bhead;					//making the function return bhead makes it a little easier in dealing with adding the first bullet
}

void draw_bullet(Bullet *bhead)			//traverses the Linked List and draws each bullet in their new positions
{
  while(bhead){
    gfx_line(bhead->xpos,bhead->ypos,bhead->xpos+(10*cos(bhead->angle)),bhead->ypos-(10*sin(bhead->angle)));
    bhead = bhead->nextb;
  }
}

void delete_bullet(Bullet **bhead, Bullet *target)		//deletes a bullet from the list
{
  Bullet *curr, *prev;
  prev = NULL;
  for(curr=*bhead; curr!=NULL; prev=curr,curr=curr->nextb){	//keeps going until we reach our target (the bullet we want to destroy)
    if(*bhead==target){						//if the bullet we want to destroy is the first in the List, just move the head pointer
      *bhead = curr->nextb;
      free(curr);						//then free the node
      return;
    }
    if(curr->nextb==target){					//if the target is not the next bullet, and the next bullet is our target
      prev = curr;						//we move prev and current so prev is pointing to the node before our target
      curr = curr->nextb;					//and curr is now pointing to the target
      if(prev!=*bhead){						//Only execute the following code when we are not deleting the first node
      prev->nextb=curr->nextb;					//Draws link from previous node around the node we want to delete
      }
      free(curr);
      prev->nextb = NULL;				// NEW LINE BECAUSE OF SEG FAULT
      return;
    }
  }
}

void update_bullets(Bullet **bhead)				//updates position and deletes any bullets that have exceeded their lifespan
{
  Bullet *b=*bhead;
  while(b){
    b->xpos += b->speed*cos(b->angle);
    b->ypos -= b->speed*sin(b->angle);
    b->xend = b->xpos+(10*cos(b->angle));
    b->yend = b->ypos-(10*sin(b->angle));
    if(b->xpos>XSIZE+15) b->xpos=0-15;				//if the bullet goes off the edge, it comes back around
    if(b->xpos<0-15) b->xpos=XSIZE+15;
    if(b->ypos>YSIZE+15) b->ypos=0-15;
    if(b->ypos<0-15) b->ypos=YSIZE+15;

    b->lifespan--;						//reduces the lifespan

    if(b->lifespan<1){						//deletes the bullet by passing in the pointer to the one that needs to die.
      delete_bullet(bhead,b);
    }
    b = b->nextb;
  }
}

// function that uses the speed, current location, and angle of each asteroid in the linked list to move it to its next location
void update_asteroids(Asteroid *p)
{
  while(p)						// traverses the linked list
  {
    p->xpos = p->xpos + p->speed * sin(p->angle);	// new xpos is its old xpos plus the x component of its speed
    p->ypos = p->ypos - p->speed * cos(p->angle);	// new ypos is its old ypos minus the y component of its speed
// the next 4 if statements sees if the asteroid has gone off the screen (more than a radius off in any direction). If it has, move it to the other side
    if(p->xpos>XSIZE+p->r) p->xpos=0-p->r;
    if(p->xpos<0-p->r) p->xpos=XSIZE+p->r;
    if(p->ypos>YSIZE+p->r) p->ypos=0-p->r;
    if(p->ypos<0-p->r) p->ypos=YSIZE+p->r;

    p=p->next;						// advance pointer in linked list
  }
}

// function that removes the Asteroid with index "index" and increases the score
Asteroid * destroy_asteroid(Asteroid *head, int index, int *score)
{ 
  Asteroid *p = head;
  float xpos=0,ypos=0;				// used to keep track of the xpos and ypos of the asteroid that is destroyed
  int i=0;					// used to keep track of the index of the asteroid that is destroyed
  while(p)					
  {
    if(p->index==index)				// searches through the linked list until it finds the asteroid with the passed in index
    {
      if(p->r==LARGE)				// if it is a large asteroid being destroyed
      {
        remove_link(&head,index,&xpos,&ypos,&i);	// remove the large asteroid
	head = add_medium(head,xpos,ypos,index,1);	// add 2 medium asteroids in place of the large one
	head = add_medium(head,xpos,ypos,index,2);
	(*score)+=20;					// add 20 to the score
	return head; 					// returns the new head of the linked list
      }
      else if(p->r==MEDIUM)			// similar to above, but adds two small asteroids, adds 50 points to score
      {
        remove_link(&head,index,&xpos,&ypos,&i);
	head = add_small(head,xpos,ypos,index,1);
	head = add_small(head,xpos,ypos,index,2);
	(*score)+=50;
	return head;
      }
      else if(p->r==SMALL)			// removes the small asteroid, increases score by 100
      {
        remove_link(&head,index,&xpos,&ypos,&i);
	(*score)+=100;
	return head;
      }
    }
    p=p->next;
  }
  return head;
}

// function to remove a node in the linked list with value "value". Stores its xpos, ypos, and index before it gets destroyed
void remove_link(Asteroid **head, int value,float *xpos, float *ypos, int *index)
{
  if((*head)->next==0)						// if the head is the only link in the list
  {
    *xpos = (*head)->xpos;					// get the heads's characteristics
    *ypos = (*head)->ypos;
    *index = (*head)->index;
    free(*head);						// free the head
    *head = (*head)->next;					// make the head point to NULL
    return;
  }
  Asteroid *curr, *prev;					// Asteroid pointers to traverse the list while keeping track of the previous node
  prev=NULL;
  for(curr=*head;curr!=NULL;prev=curr,curr=curr->next){		// traverse the list
    if(curr->index==value){
      *xpos = (curr)->xpos;					// store the necessary information
      *ypos = (curr)->ypos;
      *index = (curr)->index;
      if(prev==NULL){						// case deleting the first node
        *head=curr->next;
      }
      else{
        prev->next=curr->next;					// make the new link
      }
      free(curr);						// delete the node
      return;
    }
  }
}

// function that adds one medium asteroid at xpos,ypos
Asteroid * add_medium(Asteroid *head, float xpos, float ypos, int index, int num)
{
  Asteroid *new = (Asteroid *)malloc(sizeof(Asteroid));		// new is a pointer to the new Asteroid
  new->xpos=xpos;						// sets the members of the new Asteroid (xpos, ypos same as its parent asteroid
  new->ypos=ypos;						// radius and speed constant, index is related to its parent's index
  new->r=MEDIUM;						// new random angle
  new->speed=M_SPEED;
  new->index=10*index+num;
  new->angle = ((float)rand()/(float)(RAND_MAX)*2*M_PI);
  new->next=NULL;
  if(head==NULL)						// if there is nothing in the list
  {
    head = new;
    return head;
  }
  if(head->next==NULL){						// code to add the link to the list (adds at end)
    head->next=new;
  }
  else{
    Asteroid *curr = head;
    while(curr){
      if(curr->next==NULL){
        curr->next=new;
	break;
      }
      curr = curr->next;
    }
  }
  return head;
}

// identical to add_medium but with small asteroid characteristics
Asteroid * add_small(Asteroid *head, float xpos, float ypos, int index, int num)
{
  Asteroid *new = (Asteroid *)malloc(sizeof(Asteroid));		
  new->xpos=xpos;
  new->ypos=ypos;
  new->r=SMALL;
  new->speed=S_SPEED;
  new->index=10*index+num;
  new->angle = ((float)rand()/(float)(RAND_MAX)*2*M_PI);
  new->next=NULL;
  if(head==NULL)						
  {
    head = new;
    return head;
  }
  if(head->next==NULL){
    head->next=new;
  }
  else{
    Asteroid *curr = head;
    while(curr){
      if(curr->next==NULL){
        curr->next=new;
        break;
      }
      curr = curr->next;
    }
  }
  return head;
}

// function to display the initial menu
void draw_menu()
{
  gfx_text(.3*XSIZE/2,.3*YSIZE/2,"ASTEROIDS");					// game title
  gfx_text(.9*XSIZE/2,.9*YSIZE/2+YSIZE/40,"Press p to start");			// instruction 
  gfx_text(.9*XSIZE/2,.9*YSIZE/2+2*YSIZE/40,"Press q to quit");			// instruction
  gfx_text(.9*XSIZE/2,.9*YSIZE/2+3*YSIZE/40,"Press i for instructions");	// instruction
  gfx_text(.9*XSIZE/2,YSIZE-100,"Written by Tim Chang and Chris Syers     CSE20211");	// credits
}

// function that detects a collision and destroys the new Asteroid with the collision
Asteroid * detect_collision_ship(Asteroid *head, int x, int y, int *score,int *lives,Ship *pship )
{
  Asteroid *p = head, *newhead=head;
  while(p)						// runs through each element in the linked list of asteroids
  {
    if(fabsf(p->xpos-x)<p->r && fabsf(p->ypos-y)<p->r && pship->inv==0)	// if the distance between the point of interest (x,y) and the asteroid's centr is less than the asteroid's radius
    {
      newhead = destroy_asteroid(head,p->index,score);	// destroy that asteroid
      (*lives)--;
      init_ship(pship,*lives);
      update_ship(pship,M_PI/10,1);			//fixes bug where bullets don't initialize straight
      update_ship(pship,-M_PI/10,1);
    }
    p=p->next;						// advance through the linked list
  }
  return newhead;					// adjust head in main
}

// function to draw the asteroids in the linked list on graphics screen
void draw_asteroids(Asteroid *head)
{
  while(head)						// draws all asteroids in linked list on graphics window
  {
    gfx_circle(head->xpos,head->ypos,head->r);
    head=head->next;
  }
}

// function to initialize the linked list of asteroids
Asteroid * init_asteroids(int numAsteroids)
{
  int i;
  Asteroid *p=NULL,*head=NULL,*tail=NULL;
  for(i=1;i<=numAsteroids;i++)				// creates the linked list of numAsteroid asteroids
  {
    p = (Asteroid *) malloc(sizeof(Asteroid));		// p points to a new ASteroid struct
    if(!head) head = tail = p;				// case where it is the first link
    p->index=i;						// the next few lines set the properties of the asteroid
    p->r = LARGE;
    p->xpos=XSIZE/2;					// starts in the middle, but it will regenerate 3 lines down
    p->ypos=YSIZE/2;					// starts in the middle, but it will regenerate 3 lines down
    while(p->xpos<XSIZE/2+2*p->r && p->xpos>XSIZE/2-2*p->r && p->ypos<YSIZE/2+2*p->r && p->ypos>YSIZE/2-2*p->r)		// check to see if the asteroid is too close to middle (don't want to die instantly) will always be true the first time
    {
      p->xpos = rand()%(XSIZE+1);			// random (x,y) coordinate that is at least 2 radii away from the middle and is between 0 and XSIZE
      p->ypos = rand()%(YSIZE+1);
    }
    p->speed = L_SPEED;
    p->angle = ((float)rand()/(float)(RAND_MAX)*2*M_PI);	// starts at a random angle
tail->next = p;
    tail = p;
    tail->next = NULL;					// last node points to null
  }
  return head;
}

// draws the menu and waits for the user to click specific things
char menu()
{
  gfx_clear();
  char c = 'z';
  while(1)
  {					
    if(c=='q' || c=='p') return c;			// return p or q if the user presses p or q
    if(c=='i') c = instructions();			// display the instructions if the user presses i
    draw_menu();					// displays the menu
    c = gfx_wait();					// waits for an event
  }
}

// draws the score to the top left corner of the screen
void draw_score(int score)
{
  char score_str[15];
  sprintf(score_str,"Score: %d",score);			// prints the int score to a string
  gfx_text(5,20,score_str);				// displays the score in the top left corner
}

// displays the instruction menu
char instructions()
{
  gfx_clear();
  char c ='z';
  while(1)
  {
    gfx_text(300,300,"Asteroids Instructions:");
    gfx_text(300,320,"Press the up arrow to accelerate your ship");
    gfx_text(300,340,"Press the left arrow key to turn your ship anticlockwise");
    gfx_text(300,360,"Press the right arrow key to turn your ship clockwise");
    gfx_text(300,380,"Press the space bar to fire a bullet!");
    gfx_text(300,400,"While playing, press 'p' to puase the game");
    gfx_text(300,420,"While playing, press 'q' to quit playing");
    gfx_text(100,450,"Shoot all the asteroids to clear the level. Avoid being hit by the flying asteroids! You only have 3 lives.");
    gfx_text(100,470,"You are invicible for a short period of time at the beginning of each level and when yor ship is destroyed.");
    gfx_text(300,490,"Press 'm' to return to the main menu");
    c = gfx_wait();
    if(c=='m'){ gfx_clear(); return c;}
  }
}

// draws the number of lives in the top left corner
void draw_lives(Ship *p)
{
  char lives_str[15];
  sprintf(lives_str,"Lives: %d",p->lives);
  gfx_text(5,40,lives_str);
}

// displays the final score when the player runs out of lives. Exits the program once the user presses any key
void end_screen(int score)
{
  gfx_clear();
  char c, score_str[15];
  sprintf(score_str,"Your final score is: %d",score);			// prints the int score to a string
  gfx_text(XSIZE/2-100,YSIZE/2,score_str);					// displays the score in the middle of the screen
  gfx_text(XSIZE/2-100,YSIZE/2+20,"Please play again!");
  gfx_text(XSIZE/2-100,YSIZE/2+40,"Press any key to quit the program");
  c = gfx_wait();							// waits for the user to press any key
  exit(1);								// closes whole program
}

// runs when the user presses 'p' in the middle of playing
void pause_screen()
{
  gfx_text(XSIZE/2-50,YSIZE/2,"Game Paused.");				
  gfx_text(XSIZE/2-50,YSIZE/2+20,"Press 'p' to resume.");
}

// detects collision between the bullets and the asteroids
Asteroid * detect_collision_bullet(Bullet **bhead, Asteroid *ahead, int *score)
{
  Bullet *b;
  Asteroid *a, *newhead = ahead;
  int flag;
  a = ahead;
  while(a){								// nested whiles compare each bullet in LL to each asteroid in LL
    b = *bhead;
    while(b){
      if(fabsf(b->xpos-a->xpos)<a->r && fabsf(b->ypos-a->ypos)<a->r){	// if the bullet's start pos is within the radius of an asteroid
        delete_bullet(bhead,b);						// delete the bullet
        newhead = destroy_asteroid(ahead,a->index,score);		// delete the asteroid
     }
      if(fabsf(b->xend-a->xpos)<a->r && fabsf(b->yend-a->ypos)<a->r){	// if the bullet's end pos is within the radius of an asteroid
        delete_bullet(bhead,b);						// delete the bullet
        newhead = destroy_asteroid(ahead,a->index,score);		// delete the asteroid
      }
     b = b->nextb;							// advances the bullet
    }
   a = a->next;								// advances the asteroid
  }
 return newhead;
}

// function to display the level in the top left corner while playing the game
void draw_level(int level)
{
  char level_str[15];
  sprintf(level_str,"Level: %d",level);
  gfx_text(5,60,level_str);	
}
