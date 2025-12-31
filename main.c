#include <stdint.h>
#include <gtk/gtk.h>

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
} ST_PLAYERS;

typedef struct {
  int32_t position;
  PLAYER_TYPE player; 
} ST_POSITION;

typedef struct {
  ST_POSITION positions[9];
  ST_PLAYERS players;
  PLAYER_TYPE first_player;
  PLAYER_TYPE winner;
  int turn;
} ST_GAME;

static void create_main_window(GtkApplication *app, gpointer data);
static void initialize_game(ST_GAME *game);
static PLAYER_TYPE get_random_first_player();
static PLAYER_TYPE get_player_turn(ST_GAME *game);
static PLAYER_TYPE get_player_winner(ST_GAME *game);
static void initialize_buttons(GtkWidget *grid, ST_GAME *game);
static void process_button(GtkButton *button, gpointer data);

int main(int argc, char **argv)
{
  GtkApplication *app;

  ST_GAME game;
  initialize_game(&game);

  app = gtk_application_new("org.gtk.tictactoe", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(create_main_window), &game);

  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
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

  GtkWidget *grid = gtk_grid_new();
  gtk_grid_set_column_spacing(GTK_GRID(grid), 3);
  gtk_grid_set_row_spacing(GTK_GRID(grid), 3);
  gtk_widget_add_css_class(grid, "container");
  gtk_box_append(GTK_BOX(box), grid);
 
  initialize_buttons(grid, game);

  gtk_window_present(GTK_WINDOW(window));
}

static void initialize_game(ST_GAME *game) {
  const int32_t positions[9] = {
   0b10000000000010000000000010000000, // Row A  Column 1
   0b01000000000000001000000000000000, // Row A  Column 2
   0b00100000000000000000100000001000, // Row A  Column 3
   0b00001000000001000000000000000000, // Row B  Column 1 
   0b00000100000000000100000000000000, // Row B  Column 2
   0b00000010000000000000010000000000, // Row B  Column 3
   0b00000000100000100000000000000010, // Row C  Column 1
   0b00000000010000000010000000000000, // Row C  Column 2
   0b00000000001000000000001000100000  // Row C  Column 3
  };

  for (int i = 0; i < 9; i++) {
    game->positions[i].position = positions[i];
    game->positions[i].player = NO_PLAYER;
  }

  game->players.PlayerX = 0b00000000000000000000000000000000;
  game->players.PlayerO = 0b00000000000000000000000000000000;

  game->first_player = get_random_first_player();

  game->winner = NO_PLAYER;

  game->turn = 1;
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
  PLAYER_TYPE player = get_player_turn(game);

  if(player == PLAYER_O) {
    int32_t win = game->players.PlayerO & (game->players.PlayerO << 1) & (game->players.PlayerO >> 1);

    if(win > 0){
      return PLAYER_O;
    }
    else {
      return NO_PLAYER;
    }
  }
  else if(player == PLAYER_X) {
    int32_t win = game->players.PlayerX & (game->players.PlayerX << 1) & (game->players.PlayerX >> 1);

    if(win > 0){
      return PLAYER_X;
    }
    else {
      return NO_PLAYER;
    }
  }

  return NO_PLAYER;
}

static void initialize_buttons(GtkWidget *grid, ST_GAME *game) {
  for (int i = 0; i < 9; i++) {
    GtkWidget *button = gtk_button_new();
    gtk_widget_add_css_class(button, "button");
    
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_margin_start(box, 5);
    gtk_widget_set_margin_end(box, 5);
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
      GtkWidget *image = gtk_image_new_from_file("o_draw.png");
      gtk_widget_set_hexpand(image, true);
      gtk_widget_set_vexpand(image, true);
      gtk_box_append(GTK_BOX(box), image);

      game->players.PlayerO |= game->positions[index].position;
      game->positions[index].player = PLAYER_O;    
  }
  else if(player == PLAYER_X) {
      GtkWidget *image = gtk_image_new_from_file("x_draw.png");
      gtk_widget_set_hexpand(image, true);
      gtk_widget_set_vexpand(image, true);
      gtk_box_append(GTK_BOX(box), image);     
      
      game->players.PlayerX |= game->positions[index].position;
      game->positions[index].player = PLAYER_X;
  }
  
  game->winner = get_player_winner(game);
  game->turn = game->turn + 1;
}
