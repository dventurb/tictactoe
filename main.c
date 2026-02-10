// References: https://stackoverflow.com/questions/1056316/algorithm-for-determining-tic-tac-toe-game-over

#include <stdint.h>
#include <gtk/gtk.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 600

typedef enum {
  PLAYER_X,
  PLAYER_O,
  NO_PLAYER
} PLAYER_TYPE;

typedef struct {
  int32_t PlayerX;
  int32_t PlayerO;
  int wins_x;
  int wins_o;
  int draws;
} ST_PLAYERS;

typedef struct {
  int32_t position;
  PLAYER_TYPE player; 
} ST_POSITION;

typedef struct {
  Mix_Chunk *play;
  Mix_Chunk *win;
} ST_SOUND;

typedef struct {
  ST_POSITION positions[9];
  ST_PLAYERS players;
  PLAYER_TYPE first_player;
  PLAYER_TYPE winner;
  int turn;
  ST_SOUND sounds;
} ST_GAME;

static void create_main_window(GtkApplication *app, gpointer data);
static void initialize_game(ST_GAME *game);
static void initialize_sound(ST_GAME *game);
static void play_sound(Mix_Chunk *sound);
static void create_top_score(GtkWidget *box);
static void create_display_players(GtkWidget *box);
static PLAYER_TYPE get_random_first_player();
static PLAYER_TYPE get_player_turn(ST_GAME *game);
static PLAYER_TYPE get_player_winner(ST_GAME *game);
static void initialize_buttons(GtkWidget *grid, ST_GAME *game);
static void process_button(GtkButton *button, gpointer data);
static void update_top_score(GtkWidget *box, ST_GAME *game);
static void update_display_player(GtkWidget *box, ST_GAME *game);
static void get_new_game(GtkWidget *grid, ST_GAME *game);

int main(int argc, char **argv)
{
  GtkApplication *app;

  ST_GAME game;
  initialize_game(&game);
  initialize_sound(&game);

  app = gtk_application_new("org.gtk.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(create_main_window), &game);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}

static void initialize_game(ST_GAME *game) {
  const int32_t positions[9] = {
   0x80080080,  // Row A  Column 1
   0x40008000, // Row A  Column 2
   0x20000808, // Row A  Column 3
   0x08040000, // Row B  Column 1 
   0x04004044, // Row B  Column 2
   0x02000400, // Row B  Column 3
   0x00820002, // Row C  Column 1
   0x00402000, // Row C  Column 2
   0x00200220  // Row C  Column 3
  };

  for (int i = 0; i < 9; i++) {
    game->positions[i].position = positions[i];
    game->positions[i].player = NO_PLAYER;
  }

  game->players.PlayerX = 0x00000000; 
  game->players.PlayerO = 0x00000000;

  game->players.wins_x = 0;
  game->players.wins_o = 0;
  game->players.draws = 0;

  game->first_player = get_random_first_player();

  game->winner = NO_PLAYER;

  game->turn = 1;

  game->sounds.play = NULL;
  game->sounds.win = NULL;
}

static void initialize_sound(ST_GAME *game) {
  // https://lazyfoo.net/tutorials/SDL/21_sound_effects_and_music/index.php

  // Initialize SDL
  if(SDL_Init(SDL_INIT_AUDIO) < 0)
  {
    printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
    return;
  }        

  // Initialize SDL_mixer
  if(Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0 )
  {
    printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    return;
  }

  game->sounds.play = Mix_LoadWAV("assets/sounds/play.mp3");
  game->sounds.win = Mix_LoadWAV("assets/sounds/win.mp3");

  if(!game->sounds.play || !game->sounds.win){
    printf("Failed to load sound! SDL_mixer Error: %s\n", Mix_GetError());
    return;
  }
}

static void play_sound(Mix_Chunk *sound) {
  if(sound){
    Mix_PlayChannel(-1, sound, 0);
  }
}

static void create_main_window(GtkApplication *app, gpointer data){
  ST_GAME *game = (ST_GAME *)data;

  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "Tic Tac Toe");
  gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);
  gtk_window_set_resizable(GTK_WINDOW(window), false);
  gtk_widget_add_css_class(window, "window");
  
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
  gtk_widget_add_css_class(box, "box");
  gtk_window_set_child(GTK_WINDOW(window), box);

  create_top_score(box);

  create_display_players(box);

  update_display_player(box, game);

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 0);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 0);
  gtk_widget_add_css_class(grid, "container");
  gtk_box_append(GTK_BOX(box), grid);
 
  initialize_buttons(grid, game);

  GtkCssProvider *provider = gtk_css_provider_new();
  GFile *css_file = g_file_new_for_path("assets/css/styles.css");
  gtk_css_provider_load_from_file(provider, css_file);
  gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

  gtk_window_present(GTK_WINDOW(window));
}

static void create_top_score(GtkWidget *box) {
  GtkWidget *top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 80);
  gtk_box_append(GTK_BOX(box), top_box);
  gtk_widget_set_halign(top_box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(top_box, GTK_ALIGN_CENTER);
  gtk_widget_add_css_class(top_box, "score-box");
  
  GtkWidget *x_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_append(GTK_BOX(top_box), x_box);
  gtk_widget_set_hexpand(x_box, true);

  GtkWidget *label = gtk_label_new("Player X");
  gtk_widget_add_css_class(label, "score-label");
  gtk_box_append(GTK_BOX(x_box), label);

  label = gtk_label_new("0");
  gtk_widget_add_css_class(label, "score-label-x");
  gtk_box_append(GTK_BOX(x_box), label);
  g_object_set_data(G_OBJECT(box), "X-SCORE", label);  
  
  GtkWidget *draw_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_append(GTK_BOX(top_box), draw_box);
  gtk_widget_set_hexpand(draw_box, true);

  label = gtk_label_new("Draws");
  gtk_widget_add_css_class(label, "score-label");
  gtk_box_append(GTK_BOX(draw_box), label);

  label = gtk_label_new("0");
  gtk_widget_add_css_class(label, "score-label-draws");
  gtk_box_append(GTK_BOX(draw_box), label);
  g_object_set_data(G_OBJECT(box), "DRAW-SCORE", label);
 
  GtkWidget *o_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_append(GTK_BOX(top_box), o_box);
  gtk_widget_set_hexpand(o_box, true);

  label = gtk_label_new("Player O");
  gtk_widget_add_css_class(label, "score-label");
  gtk_box_append(GTK_BOX(o_box), label);

  label = gtk_label_new("0");
  gtk_widget_add_css_class(label, "score-label-o");
  gtk_box_append(GTK_BOX(o_box), label);
  g_object_set_data(G_OBJECT(box), "O-SCORE", label);
}

static void create_display_players(GtkWidget *box){
  GtkWidget *top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
  gtk_box_append(GTK_BOX(box), top_box);
  gtk_widget_set_halign(top_box, GTK_ALIGN_CENTER);
  gtk_widget_set_valign(top_box, GTK_ALIGN_CENTER);
  gtk_widget_add_css_class(top_box, "display-box");

  GtkWidget *x_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_append(GTK_BOX(top_box), x_box);
  gtk_widget_set_hexpand(x_box, true);  
  gtk_widget_add_css_class(x_box, "current-player");
  g_object_set_data(G_OBJECT(box), "TURN-X", x_box);

  GtkWidget *label = gtk_label_new("Player X");
  gtk_box_append(GTK_BOX(x_box), label);
  gtk_widget_add_css_class(label, "current-player-label");

  GtkWidget *o_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_append(GTK_BOX(top_box), o_box);
  gtk_widget_set_hexpand(o_box, true);  
  gtk_widget_add_css_class(o_box, "current-player");
  g_object_set_data(G_OBJECT(box), "TURN-O", o_box);

  label = gtk_label_new("Player O");
  gtk_box_append(GTK_BOX(o_box), label);
  gtk_widget_add_css_class(label, "current-player-label");
}

static PLAYER_TYPE get_random_first_player() {
  srand(time(NULL));

  return rand() % 2;
}

static PLAYER_TYPE get_player_turn(ST_GAME *game) {
  if(game->first_player == PLAYER_O) {
    if(game->turn % 2 == 0) {
      return PLAYER_O;
    }else {
      return PLAYER_X;
    }
  }
  else if(game->first_player == PLAYER_X){
    if(game->turn % 2 == 0) {
      return PLAYER_X;
    }else {
      return PLAYER_O;
    }
  }
  return NO_PLAYER;
}

static PLAYER_TYPE get_player_winner(ST_GAME *game){

  const int32_t WINS[8] = {
    0xE0088888,
    0x0E044444,
    0x00E22222,
    0x888E0082,
    0x4440E044,
    0x22200E28,
    0x842842E4,
    0x2482484E
  };


  for(int i = 0; i < 8; i++) {
    if((game->players.PlayerO & WINS[i]) == WINS[i]) {

      game->players.wins_o++;

      return PLAYER_O;
    }
    
    if((game->players.PlayerX & WINS[i]) == WINS[i]) {
      game->players.wins_x++;
      return PLAYER_X;
    }
  }

  if(game->turn > 9) {
    game->players.draws++; 
  }

  return NO_PLAYER;
}

static void initialize_buttons(GtkWidget *grid, ST_GAME *game) {
  for (int i = 0; i < 9; i++) {
    GtkWidget *button = gtk_button_new();
    gtk_widget_add_css_class(button, "button");
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_button_set_child(GTK_BUTTON(button), box);
    gtk_widget_add_css_class(box, "button-box");

    gtk_widget_set_hexpand(button, true);
    gtk_widget_set_vexpand(button, true);
  
    gtk_grid_attach(GTK_GRID(grid), button, i % 3, (int)i / 3, 1, 1);
    
    g_signal_connect(button, "clicked", G_CALLBACK(process_button), game);
  }
}

static void process_button(GtkButton *button, gpointer data){
  ST_GAME *game = (ST_GAME *)data;

  GtkWidget *box = gtk_button_get_child(GTK_BUTTON(button));

  GtkWidget *grid = gtk_widget_get_parent(GTK_WIDGET(button));
  int column, row;
  gtk_grid_query_child(GTK_GRID(grid), GTK_WIDGET(button), &column, &row, NULL, NULL);

  int index = (row * 3) + column; 
  
  if(game->positions[index].player != NO_PLAYER || game->winner != NO_PLAYER) {
    return;
  }

  PLAYER_TYPE player = get_player_turn(game);

  if(player == PLAYER_O) {
      play_sound(game->sounds.play);

      GtkWidget *image = gtk_image_new_from_file("assets/images/o_draw.png");
      gtk_widget_set_hexpand(image, true);
      gtk_widget_set_vexpand(image, true);
      gtk_box_append(GTK_BOX(box), image);

      game->players.PlayerO |= game->positions[index].position;
      game->positions[index].player = PLAYER_O;    
  }
  else if(player == PLAYER_X) {
      play_sound(game->sounds.play);

      GtkWidget *image = gtk_image_new_from_file("assets/images/x_draw.png");
      gtk_widget_set_hexpand(image, true);
      gtk_widget_set_vexpand(image, true);
      gtk_box_append(GTK_BOX(box), image);     
      
      game->players.PlayerX |= game->positions[index].position;
      game->positions[index].player = PLAYER_X;
  }
  
  GtkWidget *main_box = gtk_widget_get_parent(GTK_WIDGET(grid));
  
  game->turn = game->turn + 1; 
  game->winner = get_player_winner(game);
  update_display_player(main_box, game);
  
  if(game->winner != NO_PLAYER || game->turn > 9) {     
    play_sound(game->sounds.win);
    update_top_score(main_box, game);

    get_new_game(grid, game);
  }
}

static void update_top_score(GtkWidget *box, ST_GAME *game) {
  if(game->winner == PLAYER_X) {
    GtkWidget *label = g_object_get_data(G_OBJECT(box), "X-SCORE");

    char buffer[50];
    snprintf(buffer, 50, "%d", game->players.wins_x);

    gtk_label_set_text(GTK_LABEL(label), buffer);
  }
  else if(game->winner == PLAYER_O) {
    GtkWidget *label = g_object_get_data(G_OBJECT(box), "O-SCORE");

    char buffer[50];
    snprintf(buffer, 50, "%d", game->players.wins_o);

    gtk_label_set_text(GTK_LABEL(label), buffer);
  }else if(game->winner == NO_PLAYER && game->turn >= 9) {
    GtkWidget *label = g_object_get_data(G_OBJECT(box), "DRAW-SCORE");

    char buffer[50];
    snprintf(buffer, 50, "%d", game->players.draws);

    gtk_label_set_text(GTK_LABEL(label), buffer);
  }
}

static void update_display_player(GtkWidget *box, ST_GAME *game) {
  PLAYER_TYPE player = get_player_turn(game);

  if(player == PLAYER_O) {
    GtkWidget *o_box = g_object_get_data(G_OBJECT(box), "TURN-O");
    gtk_widget_remove_css_class(o_box, "current-player");
    gtk_widget_add_css_class(o_box, "current-player-active");   

    GtkWidget *x_box = g_object_get_data(G_OBJECT(box), "TURN-X");
    gtk_widget_remove_css_class(x_box, "current-player-active");
    gtk_widget_add_css_class(x_box, "current-player");   
  }
  else if(player == PLAYER_X) {
    GtkWidget *x_box = g_object_get_data(G_OBJECT(box), "TURN-X");
    gtk_widget_remove_css_class(x_box, "current-player");
    gtk_widget_add_css_class(x_box, "current-player-active");   

    GtkWidget *o_box = g_object_get_data(G_OBJECT(box), "TURN-O");
    gtk_widget_remove_css_class(o_box, "current-player-active");
    gtk_widget_add_css_class(o_box, "current-player");   
  }
}

static void get_new_game(GtkWidget *grid, ST_GAME *game){
  GtkWidget *button;

  while((button = gtk_widget_get_first_child(grid))) {
    gtk_grid_remove(GTK_GRID(grid), button);
  }

  initialize_buttons(grid, game);  

  for (int i = 0; i < 9; i++) {
    game->positions[i].player = NO_PLAYER;
  }

  game->players.PlayerX = 0x00000000; 
  game->players.PlayerO = 0x00000000;

  game->first_player = get_random_first_player();

  game->winner = NO_PLAYER;

  game->turn = 1;
}

