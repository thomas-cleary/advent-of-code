/*
Day 21: Keypad Conundrum
https://adventofcode.com/2024/day/21
*/

#define _DEFAULT_SOURCE
#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/aoc.h"
#include "../lib/txt.h"

// #define USE_EXAMPLE

#ifdef USE_EXAMPLE
#define INPUT_FILENAME "example-input.txt"
#define PART1_ANSWER 126384
#define PART2_ANSWER 2
#else
#define INPUT_FILENAME "puzzle-input.txt"
#define PART1_ANSWER 1
// 148460 - too high
// 142688 - too high
#define PART2_ANSWER 2
#endif

typedef enum {
  NUMPAD_BUTTON_0 = 0,
  NUMPAD_BUTTON_1 = 1,
  NUMPAD_BUTTON_2 = 2,
  NUMPAD_BUTTON_3 = 3,
  NUMPAD_BUTTON_4 = 4,
  NUMPAD_BUTTON_5 = 5,
  NUMPAD_BUTTON_6 = 6,
  NUMPAD_BUTTON_7 = 7,
  NUMPAD_BUTTON_8 = 8,
  NUMPAD_BUTTON_9 = 9,
  NUMPAD_BUTTON_A,
} NumPadButton;

typedef enum {
  DIRPAD_BUTTON_UP,
  DIRPAD_BUTTON_DOWN,
  DIRPAD_BUTTON_LEFT,
  DIRPAD_BUTTON_RIGHT,
  DIRPAD_BUTTON_A,
} DirPadButton;

typedef struct {
  int8_t row, col;
} Coord;

typedef struct {
  NumPadButton *buttons;
  size_t length;
  uint64_t value;
} Code;

typedef struct {
  Code *value;
  size_t length;
} DoorCodes;

typedef struct {
  DirPadButton *buttons;
  size_t length;
} Directions;

/*
[7][8][9]
[4][5][6]
[1][2][3]
   [0][A]
*/
static const Coord NUMPAD_BUTTON_COORDS[] = {
    [NUMPAD_BUTTON_7] = {.row = 0, .col = 0},
    [NUMPAD_BUTTON_8] = {.row = 0, .col = 1},
    [NUMPAD_BUTTON_9] = {.row = 0, .col = 2},
    [NUMPAD_BUTTON_4] = {.row = 1, .col = 0},
    [NUMPAD_BUTTON_5] = {.row = 1, .col = 1},
    [NUMPAD_BUTTON_6] = {.row = 1, .col = 2},
    [NUMPAD_BUTTON_1] = {.row = 2, .col = 0},
    [NUMPAD_BUTTON_2] = {.row = 2, .col = 1},
    [NUMPAD_BUTTON_3] = {.row = 2, .col = 2},
    // No button
    [NUMPAD_BUTTON_0] = {.row = 3, .col = 1},
    [NUMPAD_BUTTON_A] = {.row = 3, .col = 2},
};

// Answer
// 379A
// ^A              <<^^A                          >>AvvvA
// <A>A            v<<AA>^AA>A                    vAA^A<vAAA>^A
// <v<A>>^AvA^A    <vA<AA>>^AAvA<^A>AAvA^A        <vA>^AA<A>A<v<A>A>^AAAvA<^A>A

// Mine
// 379A
// ^A              ^^<<A                          >>AvvvA
// <A>A            <AAv<AA>>^A                    vAA^Av<AAA>^A
// v<<A>>^AvA^A    v<<A>>^AAv<A<A>>^AAvAA^<A>A    v<A>^AA<A>Av<A<A>>^AAAvA^<A>A

/*
   [^][A]
[<][v][>]
*/
static const Coord DIRPAD_BUTTON_COORDS[] = {
    // No button
    [DIRPAD_BUTTON_UP] = {.row = 0, .col = 1},
    [DIRPAD_BUTTON_A] = {.row = 0, .col = 2},
    [DIRPAD_BUTTON_LEFT] = {.row = 1, .col = 0},
    [DIRPAD_BUTTON_DOWN] = {.row = 1, .col = 1},
    [DIRPAD_BUTTON_RIGHT] = {.row = 1, .col = 2},
};

static const char DIRPAD_SYMBOLS[] = {
    [DIRPAD_BUTTON_UP] = '^',   [DIRPAD_BUTTON_DOWN] = 'v',
    [DIRPAD_BUTTON_LEFT] = '<', [DIRPAD_BUTTON_RIGHT] = '>',
    [DIRPAD_BUTTON_A] = 'A',
};

Coord coord_diff(Coord a, Coord b) {
  return (Coord){.row = a.row - b.row, .col = a.col - b.col};
}

void door_codes_free(DoorCodes *door_codes) {
  for (size_t i = 0; i < door_codes->length; i++) {
    free(door_codes->value[i].buttons);
  }
  free(door_codes->value);
}

DoorCodes door_codes_parse(Txt *txt) {
  assert(txt->num_lines > 0 && "invalid puzzle input");

  DoorCodes door_codes = {
      .value = malloc(sizeof(*door_codes.value) * txt->num_lines),
      .length = txt->num_lines};
  assert(door_codes.value != NULL && "malloc failed");

  for (size_t i = 0; i < door_codes.length; i++) {
    char *line = txt->lines[i];
    size_t line_len = strlen(line);

    Code code = {.buttons = malloc(sizeof(*code.buttons) * line_len),
                 .length = line_len};
    assert(code.buttons != NULL && "malloc failed");

    for (size_t j = 0; j < code.length; j++) {
      char button = line[j];
      if (button == 'A') {
        code.buttons[j] = NUMPAD_BUTTON_A;
      } else {
        assert(button >= '0' && button <= '9' && "invalid numpad button");
        code.buttons[j] = (NumPadButton)(button - '0');
      }
    }

    errno = 0;
    code.value = strtoul(line, NULL, 10);
    assert(errno == 0 && "strotoul failed");

    door_codes.value[i] = code;
  }

  return door_codes;
}

void directions_free(Directions *directions) {
  free(directions->buttons);
}

Directions numpad_directions(Code *code) {
  const size_t MAX_MOVE_LENGTH = 5; // e.g. [A] -> [7]
  size_t directions_size = (code->length * MAX_MOVE_LENGTH) +
                           code->length; // 1 'A' for each button press

  Directions directions = {
      .buttons = malloc(sizeof(*directions.buttons) * directions_size),
      .length = 0};
  assert(directions.buttons != NULL && "malloc failed");

  NumPadButton curr_button = NUMPAD_BUTTON_A;
  Coord curr_coord = NUMPAD_BUTTON_COORDS[curr_button];

  for (size_t i = 0; i < code->length; i++) {
    NumPadButton next_button = code->buttons[i];
    Coord next_coord = NUMPAD_BUTTON_COORDS[next_button];
    Coord diff = coord_diff(next_coord, curr_coord);

    if (curr_coord.row == 3) {
      if (diff.row < 0) {
        for (int8_t r = 0; r > diff.row; r--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_UP;
        }
      }

      if (diff.col < 0) {
        for (int8_t c = 0; c > diff.col; c--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_LEFT;
        }
      }

      if (diff.col > 0) {
        for (int8_t c = 0; c < diff.col; c++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_RIGHT;
        }
      }
    } else if (curr_coord.col == 2) {
      if (diff.col < 0) {
        for (int8_t c = 0; c > diff.col; c--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_LEFT;
        }
      }

      if (diff.row > 0) {
        for (int8_t r = 0; r < diff.row; r++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_DOWN;
        }
      }

      if (diff.row < 0) {
        for (int8_t r = 0; r > diff.row; r--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_UP;
        }
      }
    } else if (curr_coord.col == 0) {
      if (diff.col > 0) {
        for (int8_t c = 0; c < diff.col; c++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_RIGHT;
        }
      }

      if (diff.row < 0) {
        for (int8_t r = 0; r > diff.row; r--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_UP;
        }
      }

      if (diff.row > 0) {
        for (int8_t r = 0; r < diff.row; r++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_DOWN;
        }
      }
    } else if (curr_coord.row == 0) {
      if (diff.col < 0) {
        for (int8_t c = 0; c > diff.col; c--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_LEFT;
        }
      }

      if (diff.row > 0) {
        for (int8_t r = 0; r < diff.row; r++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_DOWN;
        }
      }

      if (diff.col > 0) {
        for (int8_t c = 0; c < diff.col; c++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_RIGHT;
        }
      }
    } else {
      if (diff.col < 0) {
        for (int8_t c = 0; c > diff.col; c--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_LEFT;
        }
      }

      if (diff.row > 0) {
        for (int8_t r = 0; r < diff.row; r++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_DOWN;
        }
      }

      if (diff.col > 0) {
        for (int8_t c = 0; c < diff.col; c++) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_RIGHT;
        }
      }

      if (diff.row < 0) {
        for (int8_t r = 0; r > diff.row; r--) {
          directions.buttons[directions.length++] = DIRPAD_BUTTON_UP;
        }
      }
    }

    directions.buttons[directions.length++] = DIRPAD_BUTTON_A;

    curr_button = next_button;
    curr_coord = next_coord;
  }

  return directions;
}

// TODO: this is pretty much the same algo as code_directions
// But with preference for DOWN before UP (might not matter)
// The COORDS array is only difference?
// And the MAX_MOVE_LENGTH?
Directions direction_directions(Directions *directions) {
  const size_t MAX_MOVE_LENGTH = 3; // e.g. [A] -> [<]
  size_t dir_dirs_size = (directions->length * MAX_MOVE_LENGTH) +
                         directions->length; // 1 'A' for each button press

  Directions dir_dirs = {.buttons =
                             malloc(sizeof(*dir_dirs.buttons) * dir_dirs_size),
                         .length = 0};
  assert(dir_dirs.buttons != NULL && "malloc failed");

  DirPadButton curr_button = DIRPAD_BUTTON_A;
  Coord curr_coord = DIRPAD_BUTTON_COORDS[curr_button];

  for (size_t i = 0; i < directions->length; i++) {
    DirPadButton next_dir = directions->buttons[i];
    Coord next_coord = DIRPAD_BUTTON_COORDS[next_dir];
    Coord diff = coord_diff(next_coord, curr_coord);

    // Prioritise moving DOWN/RIGHT first
    // That way you'll never move over the empty space on the dirpad
    // If all we care about is the length of the moveset, I'm not sure that this
    // matters...

    if (diff.row > 0) {
      for (int8_t r = 0; r < diff.row; r++) {
        dir_dirs.buttons[dir_dirs.length++] = DIRPAD_BUTTON_DOWN;
      }
    }

    if (diff.col > 0) {
      for (int8_t c = 0; c < diff.col; c++) {
        dir_dirs.buttons[dir_dirs.length++] = DIRPAD_BUTTON_RIGHT;
      }
    }

    if (diff.row < 0) {
      for (int8_t r = 0; r > diff.row; r--) {
        dir_dirs.buttons[dir_dirs.length++] = DIRPAD_BUTTON_UP;
      }
    }

    if (diff.col < 0) {
      for (int8_t c = 0; c > diff.col; c--) {
        dir_dirs.buttons[dir_dirs.length++] = DIRPAD_BUTTON_LEFT;
      }
    }

    dir_dirs.buttons[dir_dirs.length++] = DIRPAD_BUTTON_A;

    curr_button = next_dir;
    curr_coord = next_coord;
  }

  return dir_dirs;
}

// <v<A>>^AvA^A<vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A
// v<<A>>^AvA^Av<<A>>^AAv<A<A>>^AAvAA^<A>Av<A>^AA<A>Av<A<A>>^AAAvA^<A>A

void directions_print(Directions dirs) {
  for (size_t i = 0; i < dirs.length; i++) {
    putchar(DIRPAD_SYMBOLS[dirs.buttons[i]]);
  }
  putchar('\n');
}

uint64_t door_code_complexity(Code *door_code, uint8_t num_robots) {
  Directions directions = numpad_directions(door_code);
  uint8_t directors_remaining = num_robots - 2 + 1; // +1 for yourself

  printf("Code value: %u\n", (unsigned)door_code->value);
  while (directors_remaining > 0) {
    directions_print(directions);
    Directions next_directions = direction_directions(&directions);
    directions_free(&directions);
    directions = next_directions;
    directors_remaining--;
  }

  directions_print(directions);
  printf("Directions length: %zu\n\n", directions.length);

  uint64_t complexity = door_code->value * directions.length;

  directions_free(&directions);

  return complexity;
}

int main(void) {
  Txt *txt = txt_read_file(INPUT_FILENAME);
  DoorCodes door_codes = door_codes_parse(txt);

  const uint8_t num_robots = 3;
  uint64_t code_complexity = 0;
  for (size_t i = 0; i < door_codes.length; i++) {
    code_complexity += door_code_complexity(&door_codes.value[i], num_robots);
  }

  door_codes_free(&door_codes);
  txt_free(txt);

  print_day(21, "Keypad Conundrum");
  print_part_uint64(1, code_complexity, PART1_ANSWER);
  // print_part_uint64(2, 0, PART2_ANSWER);

  return 0;
}
