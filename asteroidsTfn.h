typedef struct Ships
{
  float xpos;
  float ypos;
  float xnose;
  float ynose;
  float xwing1;
  float xwing2;
  float ywing1;
  float ywing2;
  float speed;
  float angle;
  int lives;
  int inv;
} Ship;

typedef struct Lazorz
{
  int index;
  float xpos;
  float ypos;
  float speed;
  float angle;
  int lifespan; 			//how long the bullet has to live. muahaha
  float xend;
  float yend;
  struct Lazorz *nextb;
} Bullet;

typedef struct Asteroids			// asteroid struct
{
  int index;
  float xpos;
  float ypos;
  int r;
  float speed;
  float angle; 
  struct Asteroids *next;
}Asteroid;					// type def'd to Asteroid

void init_ship(Ship *s, int);
void draw_ship(Ship *s);
void update_ship(Ship *s, float angle, float speed);
Bullet * init_bullet(Bullet *bhead, Ship *s);
void draw_bullet(Bullet *b);
void update_bullets(Bullet **bhead);
void delete_bullet(Bullet **bhead, Bullet *b);
void update_asteroids(Asteroid *);
Asteroid * destroy_asteroid(Asteroid *,int,int *);
void remove_link(Asteroid **,int,float *, float *, int *);
Asteroid * add_medium(Asteroid *,float,float,int,int);
Asteroid * add_small(Asteroid *,float,float,int,int);
void draw_menu();
void draw_asteroids(Asteroid *);
Asteroid * detect_collision_ship(Asteroid *,int,int,int *, int *, Ship *);
Asteroid * detect_collision_bullet(Bullet **,Asteroid *,int *);
Asteroid * init_asteroids(int);
char menu();
void draw_score(int);
char instructions();
void draw_lives(Ship *);
void end_screen(int);
void pause_screen();
void draw_level(int level);


