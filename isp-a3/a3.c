//----------------------------------------------------------------------------------------------------------------------
///
/// This program implements a basic interactive text editor that allows the user to
/// modify a single text stored on the heap using a small set of predefined commands.
///
/// Author: 11902993
//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define ERROR_MEMORY "[ERROR] Memory allocation failed!\n"
#define ERROR_EMPTY "[ERROR] String can't be empty!\n"
#define ERROR_NOTFOUND "[ERROR] Substring not found!\n"
#define ERROR_NOWORDS "[ERROR] No words to sort!\n"
#define ERROR_UNIQUE "[ERROR] All words are already unique\n"
#define ERROR_UNKNOWN "[ERROR] Command unknown!\n"

#define INITIAL_READLINE_SIZE 8
#define WORD_LIST_START_SIZE 4

#define COMMAND_APPEND 'a'
#define COMMAND_REPLACE 'r'
#define COMMAND_SPLIT_SORT 's'
#define COMMAND_UNIQUE 'u'
#define COMMAND_QUIT 'q'

//----------------------------------------------------------------------------------------------------------------------
/// @brief it is used to read a full line of input and returns it as a dynamically allocated string.
/// @return pointer to the allocated string, or NULL on memory error.
//----------------------------------------------------------------------------------------------------------------------
char *readLine(void);

//----------------------------------------------------------------------------------------------------------------------
/// @brief checks whether the user enter the word "quit".
/// @param input_text user input string.
/// @return 1 if the input is "quit", otherwise 0.
//----------------------------------------------------------------------------------------------------------------------
int isQuitInput(char *input_text);

//----------------------------------------------------------------------------------------------------------------------
/// @brief used to free a dynamically allocated list of words.
/// @param word_list an array of word pointers.
/// @param number_of_words number of elements in the list.
//----------------------------------------------------------------------------------------------------------------------
void freeWordList(char **word_list, int number_of_words);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Splits a text into separate words by whitespace.
/// @param input_text text to split.
/// @param output_word_list the output pointer for the allocated word array.
/// @param output_word_count the output pointer for the number of words.
/// @return 0 if success, -1 if memory allocation failed.
//----------------------------------------------------------------------------------------------------------------------
int splitWords(char *input_text, char ***output_word_list, int *output_word_count);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it sorts a list of words dependent what used typed as input.
/// @param word_list an array of word strings.
/// @param number_of_words Number of elements to sort.
//----------------------------------------------------------------------------------------------------------------------
void sortWords(char **word_list, int number_of_words);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it Counts how many times a word appears inside a text.
/// @param input_text user input full text.
/// @param text_to_replace The word to search for.
/// @return Number of occurrences found.
//----------------------------------------------------------------------------------------------------------------------
int countOccurrences(char *input_text, char *text_to_replace);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Creates new text by replacing all occurence of a given substring with another substring
/// @param original_text the full input text.
/// @param text_to_replace Substring to replace.
/// @param replacement_text the replacement substring.
/// @param number_of_matches Number of occurrences counted.
/// @return a newly allocated string with replacements, otherwise NULL on error.
//----------------------------------------------------------------------------------------------------------------------
char *buildReplacedText(char *original_text, char *text_to_replace, char *replacement_text, int number_of_matches);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it compares two words independet if they are lower or uppercase, if word is the same than it will match.
/// @param first_word first string.
/// @param second_word second string.
/// @return 1 if equal ignoring case, 0 otherwise.
//----------------------------------------------------------------------------------------------------------------------
int matchStringIgnoreCase(char *first_word, char *second_word);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it marks which words in a list should remain that way the first appearance of a duplicate word is kept.
/// @param word_list Array of words.
/// @param number_of_words total number of words in word_list.
/// @param keep_word_flags Output array flag to mark the kept words: 1 = keep, 0 = remove.
/// @return 1 if duplicates existed, 0 if everything was already unique.
//----------------------------------------------------------------------------------------------------------------------
int markUniqueWords(char **word_list, int number_of_words, int *keep_word_flags);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it appends user entered text to the current text buffer.
/// @param current_text_pointer Pointer to the current text.
/// @param abort_flag flag is set to 1 on memory error, 2 if user typed "quit".
/// @return 1 on success, 0 otherwise.
//----------------------------------------------------------------------------------------------------------------------
int appendCommand(char **current_text_pointer, int *abort_flag);

//----------------------------------------------------------------------------------------------------------------------
/// @brief helper that reads and checks user input for commands.
/// @param abort_flag Set to 1 on memory error, 2 if "quit".
/// @return allocated input string or NULL on invalid input.
//----------------------------------------------------------------------------------------------------------------------
char *checkValidInput(int *abort_flag);

//----------------------------------------------------------------------------------------------------------------------
/// @brief used to searches for a substring and replace all occurrences with another.
/// @param current_text_pointer Pointer to the editable text.
/// @param abort_flag flag is set to 1 on memory error, 2 if user typed "quit"
/// @return 1 if replacement succeeded, 0 otherwise.
//----------------------------------------------------------------------------------------------------------------------
int searchReplaceCommand(char **current_text_pointer, int *abort_flag);

//----------------------------------------------------------------------------------------------------------------------
/// @brief splits the current text into words, sorts them, and rebuilds the text.
/// @param current_text_pointer pointer to current text.
/// @param abort_flag flag is set to 1 on memory error, 2 if user typed "quit".
/// @return 1 on success, 0 on error.
//----------------------------------------------------------------------------------------------------------------------
int splitSortCommand(char **current_text_pointer, int *abort_flag);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it determines which words in a list should be kept to remove duplicates.
/// @param word_array array of word strings.
/// @param word_count Number of words.
/// @param abort_flag flag is set to 1 on memory error, 2 if user typed "quit".
/// @return allocated keep flag array or NULL.
//----------------------------------------------------------------------------------------------------------------------
int *markWordsToKeep(char **word_array, int word_count, int *abort_flag);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it calculates how many characters are needed to build the final text
///        that contains only the unique words.
/// @param word_array Array of words.
/// @param word_count Number of words.
/// @param keep_word_flags array flag used to mark the kept words, 1 for keep 0 for remove.
/// @return required character count, or -1 on error.
//----------------------------------------------------------------------------------------------------------------------
int getUniqueTextLength(char **word_array, int word_count, int *keep_word_flags);

//----------------------------------------------------------------------------------------------------------------------
/// @brief Builds the final text string that contains only unique words.
/// @param word_array List of words.
/// @param word_count Number of words.
/// @param keep_word_flags array flag used to mark the kept words, 1 for keep 0 for remove.
/// @param output_length Length of the final string.
/// @return Newly allocated unique text string.
//----------------------------------------------------------------------------------------------------------------------
char *buildUniqueText(char **word_array, int word_count, int *keep_word_flags, int output_length);

//----------------------------------------------------------------------------------------------------------------------
/// @brief it runs the unique words command and it updates the current text.
/// @param current_text_pointer Pointer to current text.
/// @param abort_flag flag is set to 1 on memory error, 2 if user typed "quit".
/// @return 1 on success, 0 otherwise.
//----------------------------------------------------------------------------------------------------------------------
int uniqueCommand(char **current_text_pointer, int *abort_flag);

//----------------------------------------------------------------------------------------------------------------------
/// @brief used to interpret and execute a command entered by the user.
/// @param enter_command The entered command string.
/// @param current_text Pointer to the current text.
/// @param command_successful Set to 1 if the command modified the text.
/// @return 0 = continue, 1 = memory error, 2 = quit.
//----------------------------------------------------------------------------------------------------------------------
int handleCommand(char *enter_command, char **current_text, int *command_successful);

//----------------------------------------------------------------------------------------------------------------------
/// @brief handles all inputs from the user: reading initial text, showing
///        the command menu, processing commands, and exiting.
/// @return 0 = normal exit, 1 = error.
//----------------------------------------------------------------------------------------------------------------------
int main(void);

char *readLine(void)
{
  int current_buffer_size = INITIAL_READLINE_SIZE;
  int current_text_length = 0;
  char *input_buffer = malloc(current_buffer_size);

  if (input_buffer == NULL)
  {
    return NULL;
  }

  int current_character = getchar();

  while (current_character != '\n' && current_character != EOF)
  {
    if (current_text_length + 1 >= current_buffer_size)
    {
      int new_buffer_size = current_buffer_size * 2;
      char *resized_buffer = realloc(input_buffer, new_buffer_size);

      if (resized_buffer == NULL)
      {
        free(input_buffer);
        return NULL;
      }

      input_buffer = resized_buffer;
      current_buffer_size = new_buffer_size;
    }

    input_buffer[current_text_length] = current_character;
    current_text_length++;

    current_character = getchar();
  }

  input_buffer[current_text_length] = '\0';
  return input_buffer;
}

int isQuitInput(char *input_text)
{
  return strcmp(input_text, "quit") == 0;
}

void freeWordList(char **word_list, int number_of_words)
{
  int current_word_index;

  for (current_word_index = 0; current_word_index < number_of_words; current_word_index++)
  {
    free(word_list[current_word_index]);
  }

  free(word_list);
}

int splitWords(char *input_text, char ***output_word_list, int *output_word_count)
{
  int text_position = 0;
  int number_of_words = 0;
  int current_allocation_capacity = WORD_LIST_START_SIZE;

  char **word_list = malloc(current_allocation_capacity * sizeof(char *));

  if (word_list == NULL)
  {
    return -1;
  }

  while (input_text[text_position] != '\0')
  {
    while (input_text[text_position] != '\0' && isspace(input_text[text_position]))
    {
      text_position++;
    }

    if (input_text[text_position] == '\0')
    {
      break;
    }

    int word_start_position = text_position;

    while (input_text[text_position] != '\0' && !isspace(input_text[text_position]))
    {
      text_position++;
    }

    int current_word_length = text_position - word_start_position;

    char *new_word = malloc(current_word_length + 1);

    if (new_word == NULL)
    {
      freeWordList(word_list, number_of_words);
      return -1;
    }

    memcpy(new_word, input_text + word_start_position, current_word_length);
    new_word[current_word_length] = '\0';

    if (number_of_words == current_allocation_capacity)
    {
      int new_capacity = current_allocation_capacity * 2;
      char **resized_word_list = realloc(word_list, new_capacity * sizeof(char *));

      if (resized_word_list == NULL)
      {
        free(new_word);
        freeWordList(word_list, number_of_words);
        return -1;
      }

      word_list = resized_word_list;
      current_allocation_capacity = new_capacity;
    }

    word_list[number_of_words] = new_word;
    number_of_words++;
  }

  *output_word_list = word_list;
  *output_word_count = number_of_words;
  return 0;
}

void sortWords(char **word_list, int number_of_words)
{
  int current_word_index;
  int compare_word_index;

  for (current_word_index = 0; current_word_index < number_of_words - 1; current_word_index++)
  {
    for (compare_word_index = 0; compare_word_index < number_of_words - current_word_index - 1; compare_word_index++)
    {
      if (strcmp(word_list[compare_word_index], word_list[compare_word_index + 1]) > 0)
      {
        char *temporary_word = word_list[compare_word_index];
        word_list[compare_word_index] = word_list[compare_word_index + 1];
        word_list[compare_word_index + 1] = temporary_word;
      }
    }
  }
}

int countOccurrences(char *input_text, char *text_to_replace)
{
  int number_of_matches = 0;
  int text_to_replace_length = strlen(text_to_replace);
  char *search_position = input_text;

  if (text_to_replace_length == 0)
  {
    return 0;
  }

  char *word_found_at = strstr(search_position, text_to_replace);

  while (word_found_at != NULL)
  {
    number_of_matches++;
    search_position = word_found_at + text_to_replace_length;
    word_found_at = strstr(search_position, text_to_replace);
  }

  return number_of_matches;
}

char *buildReplacedText(char *original_text, char *text_to_replace, char *replacement_text, int number_of_matches)
{
  int text_to_replace_length = strlen(text_to_replace);
  int replacement_text_length = strlen(replacement_text);
  int original_text_length = strlen(original_text);

  int new_text_length = original_text_length + number_of_matches * (replacement_text_length - text_to_replace_length);

  char *new_text = malloc(new_text_length + 1);

  if (new_text == NULL)
  {
    return NULL;
  }

  char *text_position = original_text;
  char *destination_position = new_text;

  char *word_found_at = strstr(text_position, text_to_replace);

  while (word_found_at != NULL)
  {
    int length_until_match = (word_found_at - text_position);
    memcpy(destination_position, text_position, length_until_match);
    destination_position += length_until_match;

    memcpy(destination_position, replacement_text, replacement_text_length);
    destination_position += replacement_text_length;

    text_position = word_found_at + text_to_replace_length;
    word_found_at = strstr(text_position, text_to_replace);
  }

  strcpy(destination_position, text_position);
  return new_text;
}

int matchStringIgnoreCase(char *first_word, char *second_word)
{
  while (*first_word != '\0' && *second_word != '\0')
  {
    if (tolower(*first_word) != tolower(*second_word))
    {
      return 0;
    }

    first_word++;
    second_word++;
  }

  return *first_word == '\0' && *second_word == '\0';
}

int markUniqueWords(char **word_list, int number_of_words, int *keep_word_flags)
{
  int duplicates_found = 0;

  for (int current_word_index = 0; current_word_index < number_of_words; current_word_index++)
  {
    keep_word_flags[current_word_index] = 1;
  }

  for (int current_word_index = 0; current_word_index < number_of_words; current_word_index++)
  {
    if (!keep_word_flags[current_word_index])
    {
      continue;
    }

    for (int compare_word_index = current_word_index + 1; compare_word_index < number_of_words; compare_word_index++)
    {
      if (keep_word_flags[compare_word_index] && matchStringIgnoreCase(word_list[current_word_index], word_list[compare_word_index]))
      {
        keep_word_flags[compare_word_index] = 0;
        duplicates_found = 1;
      }
    }
  }

  return duplicates_found;
}

int appendCommand(char **current_text_pointer, int *abort_flag)
{
  printf("\nPlease enter the string that should be appended:\n");
  printf(" > ");

  char *append_text = readLine();

  if (append_text == NULL)
  {
    printf(ERROR_MEMORY);
    *abort_flag = 1;
    return 0;
  }

  if (isQuitInput(append_text))
  {
    free(append_text);
    *abort_flag = 2;
    return 0;
  }

  if (append_text[0] == '\0')
  {
    printf(ERROR_EMPTY);
    free(append_text);
    return 0;
  }

  int current_text_length = strlen(*current_text_pointer);
  int append_text_length = strlen(append_text);
  int new_text_length = current_text_length + append_text_length;

  char *final_text = malloc(new_text_length + 1);

  if (final_text == NULL)
  {
    printf(ERROR_MEMORY);
    free(append_text);
    *abort_flag = 1;
    return 0;
  }

  strcpy(final_text, *current_text_pointer);
  strcat(final_text, append_text);

  free(*current_text_pointer);
  free(append_text);

  *current_text_pointer = final_text;
  return 1;
}

char *checkValidInput(int *abort_flag)
{
  char *input = readLine();

  if (input == NULL)
  {
    printf(ERROR_MEMORY);
    *abort_flag = 1;
    return NULL;
  }

  if (isQuitInput(input))
  {
    free(input);
    *abort_flag = 2;
    return NULL;
  }

  if (input[0] == '\0')
  {
    printf(ERROR_EMPTY);
    free(input);
    return NULL;
  }

  return input;
}

int searchReplaceCommand(char **current_text_pointer, int *abort_flag)
{
  printf("\nPlease enter the substring to search for:\n > ");

  char *text_to_replace = checkValidInput(abort_flag);
  if (text_to_replace == NULL)
  {
    return 0;
  }

  if (strstr(*current_text_pointer, text_to_replace) == NULL)
  {
    printf(ERROR_NOTFOUND);
    free(text_to_replace);
    return 0;
  }

  printf("\nPlease enter the new substring:\n > ");

  char *replacement_text = readLine();

  if (replacement_text == NULL)
  {
    printf(ERROR_MEMORY);
    free(text_to_replace);
    *abort_flag = 1;
    return 0;
  }

  if (isQuitInput(replacement_text))
  {
    free(text_to_replace);
    free(replacement_text);
    *abort_flag = 2;
    return 0;
  }

  int number_of_matches = countOccurrences(*current_text_pointer, text_to_replace);

  char *replaced_text = buildReplacedText(
      *current_text_pointer,
      text_to_replace,
      replacement_text,
      number_of_matches);

  if (replaced_text == NULL)
  {
    printf(ERROR_MEMORY);
    free(text_to_replace);
    free(replacement_text);
    *abort_flag = 1;
    return 0;
  }

  free(text_to_replace);
  free(replacement_text);
  free(*current_text_pointer);

  *current_text_pointer = replaced_text;
  return 1;
}

int splitSortCommand(char **current_text_pointer, int *abort_flag)
{
  char **word_list;
  int number_of_words;

  int split_result = splitWords(*current_text_pointer, &word_list, &number_of_words);

  if (split_result == -1)
  {
    printf(ERROR_MEMORY);
    *abort_flag = 1;
    return 0;
  }

  if (number_of_words == 0)
  {
    printf(ERROR_NOWORDS);
    freeWordList(word_list, number_of_words);
    return 0;
  }

  sortWords(word_list, number_of_words);

  int total_character_count = 0;

  for (int current_word_index = 0; current_word_index < number_of_words; current_word_index++)
  {
    total_character_count += strlen(word_list[current_word_index]);
  }

  total_character_count += number_of_words - 1;

  char *sorted_text = malloc(total_character_count + 1);

  if (sorted_text == NULL)
  {
    printf(ERROR_MEMORY);
    freeWordList(word_list, number_of_words);
    *abort_flag = 1;
    return 0;
  }

  char *output_position = sorted_text;

  for (int current_word_index = 0; current_word_index < number_of_words; current_word_index++)
  {
    int current_word_length = strlen(word_list[current_word_index]);
    memcpy(output_position, word_list[current_word_index], current_word_length);
    output_position += current_word_length;

    if (current_word_index < number_of_words - 1)
    {
      *output_position = ' ';
      output_position++;
    }
  }

  *output_position = '\0';

  freeWordList(word_list, number_of_words);
  free(*current_text_pointer);

  *current_text_pointer = sorted_text;
  return 1;
}

int *markWordsToKeep(char **word_array, int word_count, int *abort_flag)
{
  int *keep_word_flags = malloc(word_count * sizeof(int));

  if (keep_word_flags == NULL)
  {
    printf(ERROR_MEMORY);
    *abort_flag = 1;
    return NULL;
  }

  int duplicates_exist = markUniqueWords(word_array, word_count, keep_word_flags);

  if (!duplicates_exist)
  {
    printf(ERROR_UNIQUE);
    free(keep_word_flags);
    return NULL;
  }

  return keep_word_flags;
}

int getUniqueTextLength(char **word_array, int word_count, int *keep_word_flags)
{
  int kept_word_count = 0;
  int total_characters = 0;

  for (int i = 0; i < word_count; i++)
  {
    if (keep_word_flags[i])
    {
      kept_word_count++;
      total_characters += strlen(word_array[i]);
    }
  }

  if (kept_word_count == 0)
  {
    return -1;
  }

  total_characters += kept_word_count - 1;

  return total_characters;
}

char *buildUniqueText(char **word_array, int word_count, int *keep_word_flags, int output_length)
{
  char *output_text = malloc(output_length + 1);

  if (output_text == NULL)
  {
    return NULL;
  }

  char *write_position = output_text;
  int is_first_word = 1;

  for (int word_index = 0; word_index < word_count; word_index++)
  {
    if (!keep_word_flags[word_index])
    {
      continue;
    }

    if (!is_first_word)
    {
      *write_position = ' ';
      write_position++;
    }

    int word_length = strlen(word_array[word_index]);
    memcpy(write_position, word_array[word_index], word_length);
    write_position += word_length;

    is_first_word = 0;
  }

  *write_position = '\0';
  return output_text;
}

int uniqueCommand(char **current_text_pointer, int *abort_flag)
{
  char **word_array;
  int word_count;

  if (splitWords(*current_text_pointer, &word_array, &word_count) == -1)
  {
    printf(ERROR_MEMORY);
    *abort_flag = 1;
    return 0;
  }

  if (word_count == 0)
  {
    printf(ERROR_UNIQUE);
    freeWordList(word_array, word_count);
    return 0;
  }

  int *keep_word_flags = markWordsToKeep(word_array, word_count, abort_flag);

  if (keep_word_flags == NULL)
  {
    freeWordList(word_array, word_count);
    return 0;
  }

  int output_length = getUniqueTextLength(word_array, word_count, keep_word_flags);

  if (output_length == -1)
  {
    free(keep_word_flags);
    freeWordList(word_array, word_count);
    *abort_flag = 1;
    return 0;
  }

  char *unique_text = buildUniqueText(word_array, word_count, keep_word_flags, output_length);

  if (unique_text == NULL)
  {
    printf(ERROR_MEMORY);
    free(keep_word_flags);
    freeWordList(word_array, word_count);
    *abort_flag = 1;
    return 0;
  }

  free(keep_word_flags);
  freeWordList(word_array, word_count);
  free(*current_text_pointer);

  *current_text_pointer = unique_text;

  return 1;
}

int handleCommand(char *enter_command, char **current_text, int *command_successful)
{
  int abort_flag = 0;

  if (enter_command[0] == COMMAND_APPEND && enter_command[1] == '\0')
  {
    *command_successful = appendCommand(current_text, &abort_flag);
  }
  else if (enter_command[0] == COMMAND_REPLACE && enter_command[1] == '\0')
  {
    *command_successful = searchReplaceCommand(current_text, &abort_flag);
  }
  else if (enter_command[0] == COMMAND_SPLIT_SORT && enter_command[1] == '\0')
  {
    *command_successful = splitSortCommand(current_text, &abort_flag);
  }
  else if (enter_command[0] == COMMAND_UNIQUE && enter_command[1] == '\0')
  {
    *command_successful = uniqueCommand(current_text, &abort_flag);
  }
  else if (enter_command[0] == COMMAND_QUIT && enter_command[1] == '\0')
  {
    printf("\nCurrent text:\n%s\n", *current_text);
    abort_flag = 2;
  }
  else
  {
    printf(ERROR_UNKNOWN);
  }

  return abort_flag;
}

int main(void)
{
  printf("\nWelcome to the\nStringtango Text Editor!\n\n");
  printf("Enter a text:\n > ");

  char *current_text = readLine();

  if (current_text == NULL)
  {
    printf(ERROR_MEMORY);
    return 1;
  }

  if (isQuitInput(current_text))
  {
    free(current_text);
    return 0;
  }

  while (1)
  {
    printf("\nChoose a command:\n");
    printf(" a: append text\n");
    printf(" r: search and replace\n");
    printf(" s: split and sort\n");
    printf(" u: unique\n");
    printf(" q: quit\n\n > ");

    char *enter_command = readLine();

    if (enter_command == NULL)
    {
      printf(ERROR_MEMORY);
      free(current_text);
      return 1;
    }

    if (isQuitInput(enter_command))
    {
      free(enter_command);
      free(current_text);
      return 0;
    }

    int command_successful = 0;
    int abort_flag = handleCommand(enter_command, &current_text, &command_successful);

    free(enter_command);

    if (abort_flag == 1)
    {
      free(current_text);
      return 1;
    }

    if (abort_flag == 2)
    {
      free(current_text);
      return 0;
    }

    if (command_successful)
    {
      printf("\nCurrent text:\n%s\n", current_text);
    }
  }

  return 0;
}
