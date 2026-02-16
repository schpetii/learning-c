//----------------------------------------------------------------------------------------------------------------------
///
/// This program implements a basic command-line based image editor
///
/// Author: 11902993
//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "bmp.h"

#define ALLOWED_ARGUMENT_COUNT 3
#define MINIMUM_CANVAS_DIMENSION 1
#define INTEGER_BASE 10
#define TOKEN_COUNT 6
#define PIXEL_SIZE 4u
#define WHITE_COLOR 255
#define INCLUSIVE_RANGE_PIXEL 1
#define EXCLUSIVE_RANGE_PIXEL 1
#define BMP_HEADER_SIZE 54
#define MAGIC_B 'B'
#define MAGIC_M 'M'

typedef enum _ErrorCode_
{
  ERR_OK = 0,
  ERR_MEM_ALLOC = 1,
  ERR_INVALID_ARGS = 2,
  ERR_INVALID_CVS_SIZE = 3,
  ERR_UNK_COMMAND,
  ERR_INVALID_NR_ARGS,
  ERR_BMPID_NOT_FOUND,
  ERR_CANT_OPEN_FILE,
  ERR_INVALID_FILE,
  ERR_COORD_OUTSITE,
  ERR_NON_VALID_RECT,
  ERR_INVALID_COORD,
  ERR_BMP_NO_FIT,
  ERR_INVALID_BLEND_MODE,
  ERR_ALREADY_ROOT,
  ERR_LAYER_ID_NOT_FOUND,
  ERR_INVALID_FILEPATH,
} ErrorCode;

typedef enum _BlendMode_
{
  BLEND_NORMAL,
  BLEND_MULTIPLY,
  BLEND_SUBTRACT

} BlendMode;

typedef struct _CommandParameters_
{
  int input_argument_count_;
  char *string_parameters_[TOKEN_COUNT];
} CommandParameters;

typedef struct _Bmp_Node_
{
  int id_;
  size_t pixel_offset_;
  size_t width_;
  size_t height_;
  unsigned char *pixels_;
  struct _Bmp_Node_ *next_;
} BmpNode;

typedef struct _Layer_
{
  int id_;
  int x_coord_;
  int y_coord_;

  BmpNode *active_bmp_;
  BlendMode blend_mode_;

  struct _Layer_ *parent_;
  struct _Layer_ *first_child_;
  struct _Layer_ *sibling_;
} Layer;

typedef struct _Application_
{
  size_t canvas_width_;
  size_t canvas_height_;
  Layer *root_layer_;
  Layer *active_layer_;
  BmpNode *root_bmp_;
  unsigned char *canvas_;
  int last_assigned_layer_id_;
  int last_assigned_bmp_id_;

  const char *messages_[ERR_INVALID_FILEPATH + 1];

} Application;

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints error message corresponding to an error code
/// @param messages array of message strings
/// @param error error with message to print.
//----------------------------------------------------------------------------------------------------------------------
void printMessage(const char *messages[], ErrorCode error);

//----------------------------------------------------------------------------------------------------------------------
/// @brief It initializes all error messages used in the assigment
/// @param app Pointer to the application
//----------------------------------------------------------------------------------------------------------------------
static void initializeErrorMessages(Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief It converts a string to an integer
/// @param input_string String to convert
/// @param parsed_number Output value
/// @return true  only if conversion worked, otherwise false
//----------------------------------------------------------------------------------------------------------------------
bool parseInteger(const char *input_string, long *parsed_number);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Checks command line arguments and it also reads canvas size
/// @param argc Number of arguments
/// @param argv Argument values
/// @param app Pointer to the application
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int parameterCheck(int argc, char **argv, Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Fills the canvas with white pixels
/// @param app pointer to the application
//----------------------------------------------------------------------------------------------------------------------
void resetCanvas(Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Initializes canvas, root layer and the program state
/// @param argc Number of arguments
/// @param argv argument values
/// @param app Pointer to the application.
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int initializeCanvas(int argc, char **argv, Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Reads a full line from user input.
/// @param app Pointer to the application.
/// @param input the output pointer to allocated input string.
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int readUserInput(Application *app, char **input);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Splits input into tokens
/// @param input Input string
/// @param input_parameters Output structure with tokens
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int splitString(char *input, CommandParameters *input_parameters);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Adds a BMP to the BMP list
/// @param app Pointer to the application
/// @param loaded_bmp_file BMP to be added
//----------------------------------------------------------------------------------------------------------------------
void addBmpToList(Application *app, BmpNode *loaded_bmp_file);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it reads pixel data from BMP file.
/// @param file Opened BMP file
/// @param loaded_bmp BMP structure
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int loadBmpPixels(FILE *file, BmpNode *loaded_bmp);

//----------------------------------------------------------------------------------------------------------------------
/// @brief used to load a BMP file from disk
/// @param app Pointer to the application
/// @param file_path path to BMP file
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int loadBmp(Application *app, const char *file_path);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints all loaded BMPs
/// @param app Pointer to the application
//----------------------------------------------------------------------------------------------------------------------
void printBmps(Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief used to find a BMP by its ID
/// @param app Pointer to the application
/// @param id BMP ID.
/// @return either ointer to BMP or NULL
//----------------------------------------------------------------------------------------------------------------------
BmpNode *returnSelectedBmp(Application *app, int id);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Checks if a coordinate is valid
/// @param input_string coordinate as string
/// @param coordinate output value
/// @param higher_bound max allowed value
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int checkBoundary(const char *input_string, long *coordinate, size_t higher_bound);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Validates all crop command coordinates
/// @param input_parameters command parameters
/// @param top_x_coordinate top_x output value
/// @param top_y_coordinate top_y output value
/// @param bottom_x_coordinate bottom_x output value
/// @param bottom_y_coordinate bottom_y output value
/// @param selected_bmp BMP to crop
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int validateInputCropCommand(CommandParameters input_parameters,
                             long *top_x_coordinate,
                             long *top_y_coordinate,
                             long *bottom_x_coordinate,
                             long *bottom_y_coordinate,
                             BmpNode *selected_bmp);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it is used to Copy a cropped area from one BMP to another
/// @param selected_bmp source BMP
/// @param created_new_bmp destination BMP
/// @param top_x_coordinate crop start X
/// @param top_y_coordinate crop start Y
//----------------------------------------------------------------------------------------------------------------------
void copyContentFromSelectedBmp(BmpNode *selected_bmp,
                                BmpNode *created_new_bmp,
                                long top_x_coordinate,
                                long top_y_coordinate);

//----------------------------------------------------------------------------------------------------------------------
/// @brief appends a BMP to the end of the BMP list
/// @param current_iterated_node start of the list
/// @param new_bmp BMP to append
//----------------------------------------------------------------------------------------------------------------------
void getLastlyCreatedBmp(BmpNode *current_iterated_node, BmpNode *new_bmp);

//----------------------------------------------------------------------------------------------------------------------
/// @brief It creates a new cropped BMP
/// @param app Pointer to the application
/// @param input_parameters command parameter
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int crop(Application *app, CommandParameters input_parameters);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Validates coordinates for placing a BMP
/// @param input_string coordinate as string
/// @param coordinate output value
/// @param boundary canvas limit
/// @param bmp_dimension BMP size
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int validateInputPlaceCommand(const char *input_string,
                              long *coordinate,
                              long boundary,
                              long bmp_dimension);

//----------------------------------------------------------------------------------------------------------------------
/// @brief to Convert blend mode input to enum
/// @param input_parameter Blend mode string
/// @param blend_mode Output value
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int extractBlendMode(const char *input_parameter, BlendMode *blend_mode);

//----------------------------------------------------------------------------------------------------------------------
/// @brief It Creates and inserts a new layer
/// @param app Pointer to the application
/// @param new_layer Layer to initialize
/// @param selected_bmp BMP for the layer
/// @param x_coordinate x position
/// @param y_coordinate y position
/// @param blend_mode Blend mode
//----------------------------------------------------------------------------------------------------------------------
void createNewLayer(Application *app,
                    Layer *new_layer,
                    BmpNode *selected_bmp,
                    long x_coordinate,
                    long y_coordinate,
                    BlendMode blend_mode);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it places a BMP on the canvas
/// @param app Pointer to the application
/// @param input_parameters command parameter
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int place(Application *app, CommandParameters input_parameters);

//----------------------------------------------------------------------------------------------------------------------
/// @brief undo, it moves back to the parent layer
/// @param app Pointer to the application
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int undo(Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the layer tree
/// @param current_layer current layer
/// @param depth Tree depth.
//----------------------------------------------------------------------------------------------------------------------
void showTree(Layer *current_layer, int depth);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it switches active layer by ID
/// @param app Pointer to the application
/// @param head current tree node
/// @param layer_id layer ID
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int switchLayer(Application *app, Layer *head, int layer_id);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Blends one color channel using normal alpha blending
/// @param transparency alpha value between 0 and 1
/// @param bmp_color_channel Color from the BMP
/// @param canvas_color_channel Color already on the canvas
/// @return the result blended color
//----------------------------------------------------------------------------------------------------------------------
int blendNormal(double transparency, int bmp_color_channel, int canvas_color_channel);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Blends one color channel using multiply blending
/// @param bmp_color_channel Color from the BMP
/// @param canvas_color_channel Color already on the canvas
/// @return the result blended color
//----------------------------------------------------------------------------------------------------------------------
int blendMultiply(int bmp_color_channel, int canvas_color_channel);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Blends one color channel using subtract blending
/// @param bmp_color_channel Color from the BMP
/// @param canvas_color_channel Color already on the canvas
/// @return the result blended color
//----------------------------------------------------------------------------------------------------------------------
int blendSubtract(int bmp_color_channel, int canvas_color_channel);

//----------------------------------------------------------------------------------------------------------------------
/// @brief  limit a color value to the valid range.
/// @param color Input color value
/// @return clamped color value between 0 and 255.
//----------------------------------------------------------------------------------------------------------------------
int clampColor(int color);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it draws all layers onto the canvas
/// @param app Pointer to the application.
/// @param current_active_layer active layer
//----------------------------------------------------------------------------------------------------------------------
void fillCanvas(Application *app, Layer *current_active_layer);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Prints the canvas to the terminal
/// @param app Pointer to the application
//----------------------------------------------------------------------------------------------------------------------
void printCanvas(Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Saves the canvas as a BMP file
/// @param app Pointer to the application
/// @param out_path output file path
/// @return Error code.
//----------------------------------------------------------------------------------------------------------------------
int saveCanvas(Application *app, const char *out_path);

//----------------------------------------------------------------------------------------------------------------------
/// @brief here we execute the selected comands
/// @param app Pointer to the application
/// @param input_parameters Ccmmand parameters
/// @param capture_user_input controls program loop
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int executeCommand(Application *app, CommandParameters input_parameters, bool *capture_user_input);

//----------------------------------------------------------------------------------------------------------------------
/// @brief based on the command string it executes a known command
/// @param app Pointer to the application
/// @param command command name as string
/// @param input_parameters parsed command parameters
/// @param capture_user_input checks whether the program keeps running
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
static int executeKnownCommand(Application *app,
                               const char *command,
                               CommandParameters input_parameters,
                               bool *capture_user_input);

//----------------------------------------------------------------------------------------------------------------------
/// @brief reads input and executes one command
/// @param app Pointer to the application
/// @param capture_user_input controls program loop
/// @return Error code
//----------------------------------------------------------------------------------------------------------------------
int commandLoop(Application *app, bool *capture_user_input);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Frees a layer and all its children
/// @param current_layer Layer to free
//----------------------------------------------------------------------------------------------------------------------
void freeLayer(Layer *current_layer);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Frees all allocated memory
/// @param app Pointer to the application
//----------------------------------------------------------------------------------------------------------------------
void freeHeap(Application *app);

//----------------------------------------------------------------------------------------------------------------------
/// @brief entry point of the program
/// @param argc Number of arguments
/// @param argv Argument values
/// @return programs exit code
//----------------------------------------------------------------------------------------------------
int main(int argc, char **argv);

void printMessage(const char *messages[], ErrorCode error)
{
  printf("%s", messages[error]);
}

static void initializeErrorMessages(Application *app)
{
  app->messages_[ERR_OK] = "";
  app->messages_[ERR_MEM_ALLOC] = "[ERROR] Memory allocation failed!\n";
  app->messages_[ERR_INVALID_ARGS] = "[ERROR] Invalid amount of command line parameters!\n";
  app->messages_[ERR_INVALID_CVS_SIZE] = "[ERROR] Invalid canvas size specified!\n";
  app->messages_[ERR_UNK_COMMAND] = "[ERROR] Command unknown!\n";
  app->messages_[ERR_INVALID_NR_ARGS] = "[ERROR] Wrong number of arguments!\n";
  app->messages_[ERR_BMPID_NOT_FOUND] = "[ERROR] BMP ID not found!\n";
  app->messages_[ERR_CANT_OPEN_FILE] = "[ERROR] Cannot open file!\n";
  app->messages_[ERR_INVALID_FILE] = "[ERROR] Invalid file!\n";
  app->messages_[ERR_COORD_OUTSITE] = "[ERROR] Crop coordinates are outside the BMP!\n";
  app->messages_[ERR_NON_VALID_RECT] = "[ERROR] Crop coordinates do not form a valid rectangle!\n";
  app->messages_[ERR_INVALID_COORD] = "[ERROR] Canvas coordinates are invalid!\n";
  app->messages_[ERR_BMP_NO_FIT] = "[ERROR] BMP does not fit on the canvas!\n";
  app->messages_[ERR_INVALID_BLEND_MODE] = "[ERROR] Invalid blend mode!\n";
  app->messages_[ERR_ALREADY_ROOT] = "[ERROR] Already at root layer!\n";
  app->messages_[ERR_LAYER_ID_NOT_FOUND] = "[ERROR] Layer ID not found!\n";
  app->messages_[ERR_INVALID_FILEPATH] = "[ERROR] Invalid file path!\n";
}

bool parseInteger(const char *input_string, long *parsed_number)
{
  char *end_pointer;
  long input_value;
  input_value = strtol(input_string, &end_pointer, INTEGER_BASE);

  if (end_pointer == input_string || *end_pointer != '\0')
  {
    return false;
  }

  *parsed_number = input_value;

  return true;
}

int parameterCheck(int argc, char **argv, Application *app)
{
  if (argc != ALLOWED_ARGUMENT_COUNT)
  {
    return ERR_INVALID_ARGS;
  }

  long temporary_width = 0;
  const char *input_argument_first = argv[1];

  if (parseInteger(input_argument_first, &temporary_width) == false)
  {
    return ERR_INVALID_CVS_SIZE;
  }

  long temporary_height = 0;
  const char *input_argument_second = argv[2];

  if (parseInteger(input_argument_second, &temporary_height) == false)
  {
    return ERR_INVALID_CVS_SIZE;
  }

  if (temporary_width < MINIMUM_CANVAS_DIMENSION || temporary_height < MINIMUM_CANVAS_DIMENSION)
  {
    return ERR_INVALID_CVS_SIZE;
  }

  app->canvas_width_ = (size_t)temporary_width;
  app->canvas_height_ = (size_t)temporary_height;

  return ERR_OK;
}

void resetCanvas(Application *app)
{
  int offset = 4;
  for (size_t index = 0; index < (app->canvas_width_ * app->canvas_height_); index++)
  {
    app->canvas_[index * offset + 0] = WHITE_COLOR;
    app->canvas_[index * offset + 1] = WHITE_COLOR;
    app->canvas_[index * offset + 2] = WHITE_COLOR;
    app->canvas_[index * offset + 3] = WHITE_COLOR;
  }
}

int initializeCanvas(int argc, char **argv, Application *app)
{
  int return_value = parameterCheck(argc, argv, app);

  if (return_value != ERR_OK)
  {
    return return_value;
  }

  app->root_layer_ = calloc(1, sizeof(*app->root_layer_));

  if (app->root_layer_ == NULL)
  {
    return ERR_MEM_ALLOC;
  }

  size_t canvas_size = app->canvas_width_ * app->canvas_height_;
  size_t canvas_pixels_size = canvas_size * PIXEL_SIZE; // 4 wegen RGB jeweils 1 und für transparent

  app->canvas_ = malloc(canvas_pixels_size);

  if (app->canvas_ == NULL)
  {
    return ERR_MEM_ALLOC;
  }

  app->root_layer_->active_bmp_ = NULL;
  app->root_layer_->id_ = 0;

  app->root_bmp_ = NULL;
  app->last_assigned_layer_id_ = 0;
  app->last_assigned_bmp_id_ = 0;

  app->active_layer_ = app->root_layer_;

  resetCanvas(app);

  return ERR_OK;
}

int readUserInput(Application *app, char **input)
{
  size_t input_length = 0;
  size_t capacity = 64;
  int read_character = 0;

  char *buffer = malloc(capacity);
  if (buffer == NULL)
  {
    printMessage(app->messages_, ERR_MEM_ALLOC);
    return ERR_MEM_ALLOC;
  }

  while ((read_character = getchar()) != '\n' && read_character != EOF)
  {
    if (input_length + 1 >= capacity)
    {
      size_t new_capacity = capacity * 2;
      char *temporary_character_storange = realloc(buffer, new_capacity);
      if (temporary_character_storange == NULL)
      {
        free(buffer);
        printMessage(app->messages_, ERR_MEM_ALLOC);
        return ERR_MEM_ALLOC;
      }
      buffer = temporary_character_storange;
      capacity = new_capacity;
    }

    buffer[input_length] = (char)read_character;
    input_length++;
  }

  buffer[input_length] = '\0';

  *input = buffer;

  return ERR_OK;
}

int splitString(char *input, CommandParameters *input_parameters)
{
  char *token = strtok(input, " ");

  while (token != NULL)
  {
    if (input_parameters->input_argument_count_ >= TOKEN_COUNT)
    {
      return ERR_OK;
    }

    input_parameters->string_parameters_[input_parameters->input_argument_count_++] = token;
    token = strtok(NULL, " ");
  }

  return ERR_OK;
}

void addBmpToList(Application *app, BmpNode *loaded_bmp_file)
{
  if (app->root_bmp_ == NULL)
  {
    app->root_bmp_ = loaded_bmp_file;
  }
  else
  {
    BmpNode *current_iterated_node = app->root_bmp_;

    while (current_iterated_node->next_ != NULL)
    {
      current_iterated_node = current_iterated_node->next_;
    }

    current_iterated_node->next_ = loaded_bmp_file;
  }
}

int loadBmpPixels(FILE *file, BmpNode *loaded_bmp)
{
  int start_position = 1;

  for (size_t index = 0; index < loaded_bmp->height_; index++)
  {
    size_t end_position = PIXEL_SIZE * loaded_bmp->width_;
    size_t pixel_row_offset = (loaded_bmp->height_ - 1u - index) * end_position;

    unsigned char *row = loaded_bmp->pixels_ + pixel_row_offset;

    if (fread(row, start_position, end_position, file) != end_position)
    {
      free(loaded_bmp->pixels_);
      free(loaded_bmp);
      fclose(file);
      return ERR_INVALID_FILE;
    }
  }

  return ERR_OK;
}

int loadBmp(Application *app, const char *file_path)
{
  FILE *file = fopen(file_path, "rb");

  if (file == NULL)
  {
    return ERR_CANT_OPEN_FILE;
  }

  unsigned int start_position = 1;
  unsigned char header[BMP_HEADER_SIZE];

  if (fread(header, start_position, BMP_HEADER_SIZE, file) != BMP_HEADER_SIZE)
  {
    fclose(file);
    return ERR_INVALID_FILE;
  }

  unsigned char magic_number_first = header[0];
  unsigned char magic_number_second = header[1];

  if (magic_number_first != MAGIC_B || magic_number_second != MAGIC_M)
  {
    fclose(file);
    return ERR_INVALID_FILE;
  }

  BmpNode *loaded_bmp = malloc(sizeof(BmpNode));

  if (loaded_bmp == NULL)
  {
    fclose(file);
    return ERR_MEM_ALLOC;
  }

  unsigned char *read_offset_position = &header[10];
  unsigned char *read_width_position = &header[18];
  unsigned char *read_height_position = &header[22];

  uint32_t offset;
  int32_t bmp_width;
  int32_t bmp_height;

  memcpy(&offset, read_offset_position, sizeof(offset));
  memcpy(&bmp_width, read_width_position, sizeof(bmp_width));
  memcpy(&bmp_height, read_height_position, sizeof(bmp_height));

  loaded_bmp->width_ = (size_t)bmp_width;
  loaded_bmp->height_ = (size_t)bmp_height;

  loaded_bmp->next_ = NULL;

  fseek(file, offset, SEEK_SET);

  size_t bmp_size = loaded_bmp->width_ * loaded_bmp->height_ * PIXEL_SIZE;
  loaded_bmp->pixels_ = malloc(bmp_size);

  if (loaded_bmp->pixels_ == NULL)
  {
    free(loaded_bmp);
    fclose(file);
    return ERR_MEM_ALLOC;
  }

  loadBmpPixels(file, loaded_bmp);
  addBmpToList(app, loaded_bmp);

  loaded_bmp->id_ = app->last_assigned_bmp_id_++;
  fclose(file);
  printf("Loaded %s with ID %d and dimensions %ld %ld\n", file_path, loaded_bmp->id_, loaded_bmp->width_, loaded_bmp->height_);

  return ERR_OK;
}

void printBmps(Application *app)
{
  BmpNode *current_iterated_node = app->root_bmp_;

  while (current_iterated_node != NULL)
  {
    printf("BMP %d has dimensions %ld x %ld\n", current_iterated_node->id_, current_iterated_node->width_, current_iterated_node->height_);
    current_iterated_node = current_iterated_node->next_;
  }
}

BmpNode *returnSelectedBmp(Application *app, int id)
{
  BmpNode *current_iterated_node = app->root_bmp_;

  while (current_iterated_node != NULL)
  {
    if (current_iterated_node->id_ == id)
    {
      return current_iterated_node;
    }

    current_iterated_node = current_iterated_node->next_;
  }

  return NULL;
}

int checkBoundary(const char *input_string, long *coordinate, size_t higher_bound)
{

  if (parseInteger(input_string, coordinate) == false)
  {
    return ERR_COORD_OUTSITE;
  }

  if (*coordinate < MINIMUM_CANVAS_DIMENSION || *coordinate > (long)higher_bound)
  {
    return ERR_COORD_OUTSITE;
  }

  return ERR_OK;
}

int validateInputCropCommand(CommandParameters input_parameters, long *top_x_coordinate, long *top_y_coordinate,
                             long *bottom_x_coordinate, long *bottom_y_coordinate, BmpNode *selected_bmp)
{

  const char *input_coordinate_first = input_parameters.string_parameters_[2];
  int error_code = checkBoundary(input_coordinate_first, top_x_coordinate, selected_bmp->width_);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  const char *input_coordinate_second = input_parameters.string_parameters_[3];
  error_code = checkBoundary(input_coordinate_second, top_y_coordinate, selected_bmp->height_);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  const char *input_coordinate_third = input_parameters.string_parameters_[4];
  error_code = checkBoundary(input_coordinate_third, bottom_x_coordinate, selected_bmp->width_);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  const char *input_coordinate_fourth = input_parameters.string_parameters_[5];
  error_code = checkBoundary(input_coordinate_fourth, bottom_y_coordinate, selected_bmp->height_);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  if (*bottom_x_coordinate < *top_x_coordinate || *bottom_y_coordinate < *top_y_coordinate)
  {
    return ERR_NON_VALID_RECT;
  }

  return error_code;
}

void copyContentFromSelectedBmp(BmpNode *selected_bmp, BmpNode *created_new_bmp, long top_x_coordinate, long top_y_coordinate)
{
  for (size_t y = 0; y < created_new_bmp->height_; y++)
  {

    size_t offset_y = (size_t)(top_y_coordinate - EXCLUSIVE_RANGE_PIXEL + y);
    size_t offset_x = (size_t)(top_x_coordinate - EXCLUSIVE_RANGE_PIXEL) * PIXEL_SIZE;

    size_t row_length_selected_bmp = selected_bmp->width_ * PIXEL_SIZE;

    unsigned char *position_selected_bmp = (unsigned char *)selected_bmp->pixels_ + offset_y * row_length_selected_bmp + offset_x;
    unsigned char *position_created_bmp = (unsigned char *)created_new_bmp->pixels_ + (size_t)y * created_new_bmp->width_ * PIXEL_SIZE;

    memcpy(position_created_bmp, position_selected_bmp, created_new_bmp->width_ * PIXEL_SIZE);
  }
}

void getLastlyCreatedBmp(BmpNode *current_iterated_node, BmpNode *new_bmp)
{
  while (current_iterated_node->next_ != NULL)
  {
    current_iterated_node = current_iterated_node->next_;
  }

  current_iterated_node->next_ = new_bmp;
}

int crop(Application *app, CommandParameters input_parameters)
{
  long selected_bmp_id = 0;

  if (parseInteger(input_parameters.string_parameters_[1], &selected_bmp_id) == false)
  {
    return ERR_BMPID_NOT_FOUND;
  }

  BmpNode *selected_bmp = returnSelectedBmp(app, selected_bmp_id);

  if (selected_bmp == NULL)
  {
    return ERR_BMPID_NOT_FOUND;
  }

  long top_x_coordinate = 0;
  long top_y_coordinate = 0;
  long bottom_x_coordinate = 0;
  long bottom_y_coordinate = 0;

  int error_code = validateInputCropCommand(input_parameters, &top_x_coordinate, &top_y_coordinate, &bottom_x_coordinate, &bottom_y_coordinate, selected_bmp);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  BmpNode *new_bmp = malloc(sizeof(BmpNode));

  if (new_bmp == NULL)
  {
    return ERR_MEM_ALLOC;
  }

  new_bmp->width_ = (size_t)(bottom_x_coordinate - top_x_coordinate + INCLUSIVE_RANGE_PIXEL);
  new_bmp->height_ = (size_t)(bottom_y_coordinate - top_y_coordinate + INCLUSIVE_RANGE_PIXEL);
  new_bmp->next_ = NULL;

  size_t size = new_bmp->width_ * new_bmp->height_ * PIXEL_SIZE;

  new_bmp->pixels_ = malloc(size);

  if (new_bmp->pixels_ == NULL)
  {
    free(new_bmp);
    return ERR_MEM_ALLOC;
  }

  new_bmp->id_ = app->last_assigned_bmp_id_++;

  copyContentFromSelectedBmp(selected_bmp, new_bmp, top_x_coordinate, top_y_coordinate);

  BmpNode *bmp_root = app->root_bmp_;

  getLastlyCreatedBmp(bmp_root, new_bmp);

  printf("Cropped ID %ld to new ID %d with dimensions %ld x %ld\n", selected_bmp_id, new_bmp->id_,
         new_bmp->width_, new_bmp->height_);

  return ERR_OK;
}

int validateInputPlaceCommand(const char *input_string, long *coordinate, long boundary, long bmp_dimension)
{

  if (parseInteger(input_string, coordinate) == false)
  {
    return ERR_INVALID_COORD;
  }

  if (*coordinate < MINIMUM_CANVAS_DIMENSION || *coordinate > boundary)
  {
    return ERR_INVALID_COORD;
  }

  if (*coordinate + bmp_dimension - 1 > boundary)
  {
    return ERR_BMP_NO_FIT;
  }

  return ERR_OK;
}

int extractBlendMode(const char *input_parameter, BlendMode *blend_mode)
{
  if (strcmp(input_parameter, "n") == 0)
  {
    *blend_mode = BLEND_NORMAL;
  }
  else if (strcmp(input_parameter, "m") == 0)
  {
    *blend_mode = BLEND_MULTIPLY;
  }
  else if (strcmp(input_parameter, "s") == 0)
  {
    *blend_mode = BLEND_SUBTRACT;
  }
  else
  {
    return ERR_INVALID_BLEND_MODE;
  }

  return ERR_OK;
}

void createNewLayer(Application *app, Layer *new_layer, BmpNode *selected_bmp, long x_coordinate, long y_coordinate, BlendMode blend_mode)
{
  new_layer->id_ = ++app->last_assigned_layer_id_;
  new_layer->x_coord_ = x_coordinate;
  new_layer->y_coord_ = y_coordinate;
  new_layer->active_bmp_ = selected_bmp;
  new_layer->blend_mode_ = blend_mode;

  Layer *active_selected_layer = app->active_layer_;
  new_layer->parent_ = active_selected_layer;
  new_layer->first_child_ = NULL;
  new_layer->sibling_ = NULL;

  if (active_selected_layer->first_child_ == NULL)
  {
    new_layer->parent_->first_child_ = new_layer;
  }
  else
  {
    Layer *current_layer = new_layer->parent_->first_child_;

    while (current_layer->sibling_ != NULL)
    {
      current_layer = current_layer->sibling_;
    }

    current_layer->sibling_ = new_layer;
  }

  app->active_layer_ = new_layer;
}

int place(Application *app, CommandParameters input_parameters)
{
  long selected_bmp_id = 0;

  if (parseInteger(input_parameters.string_parameters_[1], &selected_bmp_id) == false)
  {
    return ERR_BMPID_NOT_FOUND;
  }

  BmpNode *selected_bmp = returnSelectedBmp(app, selected_bmp_id);

  if (selected_bmp == NULL)
  {
    return ERR_BMPID_NOT_FOUND;
  }

  long x_coordinate = 0;
  long y_coordinate = 0;

  const char *input_coordinate_first = input_parameters.string_parameters_[2];
  int error_code = validateInputPlaceCommand(input_coordinate_first, &x_coordinate, (long)app->canvas_width_, (long)selected_bmp->width_);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  const char *input_coordinate_second = input_parameters.string_parameters_[3];
  error_code = validateInputPlaceCommand(input_coordinate_second, &y_coordinate, (long)app->canvas_height_, (long)selected_bmp->height_);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  BlendMode blend_mode;

  error_code = extractBlendMode(input_parameters.string_parameters_[4], &blend_mode);

  if (error_code != ERR_OK)
  {
    return error_code;
  }

  Layer *new_layer = malloc(sizeof(Layer));

  if (new_layer == NULL)
  {
    return ERR_MEM_ALLOC;
  }

  createNewLayer(app, new_layer, selected_bmp, x_coordinate, y_coordinate, blend_mode);

  printf("Switched to layer %d\n", new_layer->id_);

  return ERR_OK;
}

int undo(Application *app)
{
  if (app->active_layer_->parent_ == NULL)
  {
    return ERR_ALREADY_ROOT;
  }

  app->active_layer_ = app->active_layer_->parent_;
  printf("Switched to layer %d\n", app->active_layer_->id_);

  return ERR_OK;
}

void showTree(Layer *current_layer, int depth)
{

  if (current_layer == NULL)
    return;

  if (current_layer->id_ == 0)
  {
    printf("Layer 0\n");
  }
  else
  {
    for (int depth_index = 0; depth_index < depth; depth_index++)
    {
      printf("   ");
    }

    printf("Layer %d renders BMP %d at %d %d\n", current_layer->id_, current_layer->active_bmp_->id_, current_layer->x_coord_, current_layer->y_coord_);
  }

  showTree(current_layer->first_child_, depth + 1);

  showTree(current_layer->sibling_, depth);
}

int switchLayer(Application *app, Layer *head, int layer_id)
{
  if (head == NULL)
  {
    return ERR_LAYER_ID_NOT_FOUND;
  }

  if (head->id_ == layer_id)
  {
    app->active_layer_ = head;
    printf("Switched to layer %d\n", layer_id);
    return ERR_OK;
  }

  int return_value = switchLayer(app, head->first_child_, layer_id);

  if (return_value == ERR_OK)
  {
    return ERR_OK;
  }

  return switchLayer(app, head->sibling_, layer_id);
}

int blendNormal(double transparency, int bmp_color_channel, int canvas_color_channel)
{
  int color = (int)(transparency * (double)bmp_color_channel + (1.0 - transparency) * (double)canvas_color_channel);
  return color;
}

int blendMultiply(int bmp_color_channel, int canvas_color_channel)
{
  int color = (canvas_color_channel * bmp_color_channel) / WHITE_COLOR;
  return color;
}

int blendSubtract(int bmp_color_channel, int canvas_color_channel)
{
  int color = 0;

  if (canvas_color_channel > bmp_color_channel)
  {
    color = canvas_color_channel - bmp_color_channel;
  }
  else
  {
    color = bmp_color_channel - canvas_color_channel;
  }

  return color;
}

int clampColor(int color)
{
  if (color < 0)
  {
    color = 0;
  }
  else if (color > WHITE_COLOR)
  {
    color = WHITE_COLOR;
  }

  return color;
}

void fillCanvas(Application *app, Layer *current_active_layer)
{

  if (current_active_layer == app->root_layer_)
  {
    return;
  }

  fillCanvas(app, current_active_layer->parent_);

  BmpNode *active_bmp = current_active_layer->active_bmp_;
  BlendMode blend_mode = current_active_layer->blend_mode_;

  size_t start_coordinate_y = current_active_layer->y_coord_ - EXCLUSIVE_RANGE_PIXEL;
  size_t end_coordinate_y = start_coordinate_y + active_bmp->height_;

  for (size_t canvas_coordinate_y = start_coordinate_y; canvas_coordinate_y < end_coordinate_y; canvas_coordinate_y++)
  {
    size_t start_coordinate_x = current_active_layer->x_coord_ - EXCLUSIVE_RANGE_PIXEL;
    size_t end_coordinate_x = start_coordinate_x + active_bmp->width_;

    for (size_t canvas_coordinate_x = start_coordinate_x; canvas_coordinate_x < end_coordinate_x; canvas_coordinate_x++)
    {

      int bmp_x_coordinate = canvas_coordinate_x - (current_active_layer->x_coord_ - EXCLUSIVE_RANGE_PIXEL);
      int bmp_y_coordinate = canvas_coordinate_y - (current_active_layer->y_coord_ - EXCLUSIVE_RANGE_PIXEL);

      size_t pixel_postion_bmp = (size_t)(bmp_y_coordinate * active_bmp->width_ + bmp_x_coordinate) * PIXEL_SIZE;
      size_t pixel_postion_canvas = (size_t)(canvas_coordinate_y * app->canvas_width_ + canvas_coordinate_x) * PIXEL_SIZE;

      int bmp_pixel_blue_channel = (int)active_bmp->pixels_[pixel_postion_bmp + 0];
      int bmp_pixel_green_channel = (int)active_bmp->pixels_[pixel_postion_bmp + 1];
      int bmp_pixel_red_channel = (int)active_bmp->pixels_[pixel_postion_bmp + 2];
      int bmp_pixel_transparent = (int)active_bmp->pixels_[pixel_postion_bmp + 3];

      int canvas_pixel_blue_channel = (int)app->canvas_[pixel_postion_canvas + 0];
      int canvas_pixel_green_channel = (int)app->canvas_[pixel_postion_canvas + 1];
      int canvas_pixel_red_channel = (int)app->canvas_[pixel_postion_canvas + 2];

      int output_blue_channel = 0;
      int output_green_channel = 0;
      int output_red_channel = 0;

      if (blend_mode == BLEND_NORMAL)
      {
        double transparency = (double)bmp_pixel_transparent / 255.0;
        output_blue_channel = blendNormal(transparency, bmp_pixel_blue_channel, canvas_pixel_blue_channel);
        output_green_channel = blendNormal(transparency, bmp_pixel_green_channel, canvas_pixel_green_channel);
        output_red_channel = blendNormal(transparency, bmp_pixel_red_channel, canvas_pixel_red_channel);
      }
      else if (blend_mode == BLEND_MULTIPLY)
      {
        output_blue_channel = blendMultiply(bmp_pixel_blue_channel, canvas_pixel_blue_channel);
        output_green_channel = blendMultiply(bmp_pixel_green_channel, canvas_pixel_green_channel);
        output_red_channel = blendMultiply(bmp_pixel_red_channel, canvas_pixel_red_channel);
      }
      else if (blend_mode == BLEND_SUBTRACT)
      {
        output_blue_channel = blendSubtract(bmp_pixel_blue_channel, canvas_pixel_blue_channel);
        output_green_channel = blendSubtract(bmp_pixel_green_channel, canvas_pixel_green_channel);
        output_red_channel = blendSubtract(bmp_pixel_red_channel, canvas_pixel_red_channel);
      }

      output_blue_channel = clampColor(output_blue_channel);
      output_green_channel = clampColor(output_green_channel);
      output_red_channel = clampColor(output_red_channel);

      app->canvas_[pixel_postion_canvas + 0] = (unsigned char)output_blue_channel;
      app->canvas_[pixel_postion_canvas + 1] = (unsigned char)output_green_channel;
      app->canvas_[pixel_postion_canvas + 2] = (unsigned char)output_red_channel;
      app->canvas_[pixel_postion_canvas + 3] = WHITE_COLOR;
    }
  }

  return;
}

void printCanvas(Application *app)
{

  fillCanvas(app, app->active_layer_);

  printf("   ");

  for (size_t index = INCLUSIVE_RANGE_PIXEL; index < app->canvas_width_ + INCLUSIVE_RANGE_PIXEL; index++)
  {
    printf(" %02ld", index);
  }

  printf("\n");

  for (size_t canvas_coordinate_y = 0; canvas_coordinate_y < app->canvas_height_; canvas_coordinate_y++)
  {
    printf("%02ld|", canvas_coordinate_y + INCLUSIVE_RANGE_PIXEL);

    for (size_t canvas_coordinate_x = 0; canvas_coordinate_x < app->canvas_width_; canvas_coordinate_x++)
    {

      size_t index = (canvas_coordinate_y * app->canvas_width_ + canvas_coordinate_x) * PIXEL_SIZE;

      unsigned char pixel_blue_channel = app->canvas_[index + 0];
      unsigned char pixel_green_channel = app->canvas_[index + 1];
      unsigned char pixel_red_channel = app->canvas_[index + 2];

      printf("\033[38;2;%u;%u;%um███\033[0m", pixel_red_channel, pixel_green_channel, pixel_blue_channel);
    }

    printf("|\n");
  }

  printf("  ");

  size_t line_width = app->canvas_width_ * 3 + 2;

  for (size_t endline = 0; endline < line_width; endline++)
  {
    printf("-");
  }
  printf("\n");

  resetCanvas(app);
}

int saveCanvas(Application *app, const char *out_path)
{
  int start_file_position = 1;
  FILE *file = fopen(out_path, "wb");
  if (file == NULL)
  {
    return ERR_INVALID_FILEPATH;
  }

  BmpHeader header;
  fillBmpHeaderDefaultValues(&header, app->canvas_width_, app->canvas_height_);

  if (fwrite(&header, start_file_position, sizeof(BmpHeader), file) != sizeof(BmpHeader))
  {
    fclose(file);
    return ERR_INVALID_FILE;
  }

  fillCanvas(app, app->active_layer_);

  const size_t row_size = app->canvas_width_ * PIXEL_SIZE;

  for (long canvas_coordinate_y = (long)app->canvas_height_ - EXCLUSIVE_RANGE_PIXEL; canvas_coordinate_y >= 0; --canvas_coordinate_y) // - gerechnet. weil bmp von bottom to top geschrieben muss
  {
    unsigned char *row = app->canvas_ + canvas_coordinate_y * row_size;

    if (fwrite(row, start_file_position, row_size, file) != row_size)
    {
      fclose(file);
      return ERR_INVALID_FILEPATH;
    }
  }

  fclose(file);
  printf("Successfully saved image to %s\n", out_path);

  resetCanvas(app);
  return ERR_OK;
}

int helpCommand(Application *app, int argument_count)
{
  if (argument_count != 1)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }
  printf("\nAvailable commands:\n help\n load <PATH>\n");
  printf(" crop <BMP_ID> <TOP_X> <TOP_Y> <BOTTOM_X> <BOTTOM_Y>\n place <BMP_ID> <CANVAS_X> <CANVAS_Y> <BLEND_MODE>\n");
  printf(" undo\n print\n switch <LAYER_ID>\n tree\n bmps\n save <FILE_PATH>\n quit\n\n");
  return ERR_OK;
}

int loadCommand(Application *app, CommandParameters input_parameters)
{
  if (input_parameters.input_argument_count_ != 2)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  int return_value = loadBmp(app, input_parameters.string_parameters_[1]);

  if (return_value != ERR_OK)
  {
    printMessage(app->messages_, return_value);
    if (return_value == ERR_MEM_ALLOC)
    {
      return return_value;
    }
  }

  return ERR_OK;
}

int bmpsCommand(Application *app, int argument_count)
{
  if (argument_count != 1)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  printBmps(app);

  return ERR_OK;
}

int cropCommand(Application *app, CommandParameters input_parameters)
{
  if (input_parameters.input_argument_count_ != TOKEN_COUNT)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  int return_value = crop(app, input_parameters);

  if (return_value != ERR_OK)
  {
    printMessage(app->messages_, return_value);

    if (return_value == ERR_MEM_ALLOC)
    {
      return return_value;
    }
  }

  return ERR_OK;
}

int placeCommand(Application *app, CommandParameters input_parameters)
{
  if (input_parameters.input_argument_count_ != 5)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  int return_value = place(app, input_parameters);

  if (return_value != ERR_OK)
  {
    printMessage(app->messages_, return_value);

    if (return_value == ERR_MEM_ALLOC)
    {
      return return_value;
    }
  }

  return ERR_OK;
}

int undoCommand(Application *app, int argument_count)
{
  if (argument_count != 1)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_OK;
  }

  int return_value = undo(app);

  if (return_value != ERR_OK)
  {
    printMessage(app->messages_, return_value);
  }

  return ERR_OK;
}
int treeCommand(Application *app, int argument_count)
{
  if (argument_count != 1)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  showTree(app->root_layer_, 0);

  return ERR_OK;
}

int switchCommand(Application *app, CommandParameters input_parameters)
{
  if (input_parameters.input_argument_count_ != 2)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  long layer_id = 0;

  if (parseInteger(input_parameters.string_parameters_[1], &layer_id) == false)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_LAYER_ID_NOT_FOUND;
  }

  int return_value = switchLayer(app, app->root_layer_, layer_id);

  if (return_value != ERR_OK)
  {
    printMessage(app->messages_, return_value);
  }

  return ERR_OK;
}

int printCommand(Application *app, int argument_count)
{
  if (argument_count != 1)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  printCanvas(app);

  return ERR_OK;
}

int saveCommand(Application *app, CommandParameters input_parameters)
{
  if (input_parameters.input_argument_count_ != 2)
  {
    printMessage(app->messages_, ERR_INVALID_NR_ARGS);
    return ERR_INVALID_NR_ARGS;
  }

  int return_value = saveCanvas(app, input_parameters.string_parameters_[1]);
  if (return_value != ERR_OK)
  {
    printMessage(app->messages_, return_value);
  }

  return ERR_OK;
}

static int executeKnownCommand(Application *app, const char *command, CommandParameters input_parameters, bool *capture_user_input)
{
  if (strcmp(command, "quit") == 0)
  {
    if (input_parameters.input_argument_count_ != 1)
    {
      printMessage(app->messages_, ERR_INVALID_NR_ARGS);
      return ERR_INVALID_NR_ARGS;
    }

    *capture_user_input = false;
    return ERR_OK;
  }
  else if (strcmp(command, "help") == 0)
  {
    return helpCommand(app, input_parameters.input_argument_count_);
  }
  else if (strcmp(command, "load") == 0)
  {
    return loadCommand(app, input_parameters);
  }
  else if (strcmp(command, "bmps") == 0)
  {
    return bmpsCommand(app, input_parameters.input_argument_count_);
  }
  else if (strcmp(command, "crop") == 0)
  {
    return cropCommand(app, input_parameters);
  }
  else if (strcmp(command, "place") == 0)
  {
    return placeCommand(app, input_parameters);
  }
  else if (strcmp(command, "undo") == 0)
  {
    return undoCommand(app, input_parameters.input_argument_count_);
  }
  else if (strcmp(command, "tree") == 0)
  {
    return treeCommand(app, input_parameters.input_argument_count_);
  }
  else if (strcmp(command, "switch") == 0)
  {
    return switchCommand(app, input_parameters);
  }
  else if (strcmp(command, "print") == 0)
  {
    return printCommand(app, input_parameters.input_argument_count_);
  }
  else if (strcmp(command, "save") == 0)
  {
    return saveCommand(app, input_parameters);
  }

  printMessage(app->messages_, ERR_UNK_COMMAND);
  return ERR_OK;
}

int executeCommand(Application *app, CommandParameters input_parameters, bool *capture_user_input)
{
  if (input_parameters.input_argument_count_ == 0)
  {
    printMessage(app->messages_, ERR_UNK_COMMAND);
    return ERR_UNK_COMMAND;
  }

  const char *command = input_parameters.string_parameters_[0];
  return executeKnownCommand(app, command, input_parameters, capture_user_input);
}

int commandLoop(Application *app, bool *capture_user_input)
{
  char *user_input = NULL;

  printf(" > ");
  int return_value = readUserInput(app, &user_input);

  if (return_value != ERR_OK)
  {
    free(user_input);
    return return_value;
  }

  CommandParameters user_input_parameters = {0};
  splitString(user_input, &user_input_parameters);

  return_value = executeCommand(app, user_input_parameters, capture_user_input);

  free(user_input);
  return return_value;
}

void freeLayer(Layer *current_layer)
{
  if (current_layer == NULL)
  {
    return;
  }

  freeLayer(current_layer->first_child_);

  freeLayer(current_layer->sibling_);

  free(current_layer);
}

void freeHeap(Application *app)
{
  BmpNode *current_iterated_node = app->root_bmp_;
  while (current_iterated_node != NULL)
  {
    BmpNode *next = current_iterated_node->next_;
    free(current_iterated_node->pixels_);
    free(current_iterated_node);

    current_iterated_node = next;
  }

  free(app->canvas_);
  freeLayer(app->root_layer_);
  free(app);
}

int main(int argc, char **argv)
{
  Application *app = malloc(sizeof(Application));

  if (app == NULL)
  {
    printf("[ERROR] Memory allocation failed!\n");
    return ERR_MEM_ALLOC;
  }

  initializeErrorMessages(app);

  int return_value = initializeCanvas(argc, argv, app);

  if (return_value != ERR_OK)
  {

    printMessage(app->messages_, return_value);
    free(app);
    return return_value;
  }

  printf("\nWelcome to Image Structuring Program!\nThe canvas is %ld x %ld pixels.\n\n", app->canvas_width_, app->canvas_height_);

  bool capture_user_input = true;

  while (capture_user_input == true)
  {
    return_value = commandLoop(app, &capture_user_input);

    if (return_value != ERR_OK)
    {
      capture_user_input = false;
    }
  }

  freeHeap(app);

  return return_value;
}
