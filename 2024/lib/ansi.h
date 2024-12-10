#ifndef ANSI_H
#define ANSI_H

typedef enum AnsiCode {
  ANSI_CODE_BOLD = 1,
  ANSI_CODE_FAINT,
  ANSI_CODE_ITALIC,
  ANSI_CODE_UNDERLINE,
  ANSI_CODE_SLOW_BLINK,
  ANSI_CODE_RAPID_BLINK,
  ANSI_CODE_STRIKE = 9,
  ANSI_CODE_FG_BLACK = 30,
  ANSI_CODE_FG_RED,
  ANSI_CODE_FG_GREEN,
  ANSI_CODE_FG_YELLOW,
  ANSI_CODE_FG_BLUE,
  ANSI_CODE_FG_MAGENTA,
  ANSI_CODE_FG_CYAN,
  ANSI_CODE_FG_WHITE,
  ANSI_CODE_BG_BLACK = 40,
  ANSI_CODE_BG_RED,
  ANSI_CODE_BG_GREEN,
  ANSI_CODE_BG_YELLOW,
  ANSI_CODE_BG_BLUE,
  ANSI_CODE_BG_MAGENTA,
  ANSI_CODE_BG_CYAN,
  ANSI_CODE_BG_WHITE,
  ANSI_CODE_FG_BRIGHT_BLACK = 90,
  ANSI_CODE_FG_BRIGHT_RED,
  ANSI_CODE_FG_BRIGHT_GREEN,
  ANSI_CODE_FG_BRIGHT_YELLOW,
  ANSI_CODE_FG_BRIGHT_BLUE,
  ANSI_CODE_FG_BRIGHT_MAGENTA,
  ANSI_CODE_FG_BRIGHT_CYAN,
  ANSI_CODE_FG_BRIGHT_WHITE,
  ANSI_CODE_BG_BRIGHT_BLACK = 100,
  ANSI_CODE_BG_BRIGHT_RED,
  ANSI_CODE_BG_BRIGHT_GREEN,
  ANSI_CODE_BG_BRIGHT_YELLOW,
  ANSI_CODE_BG_BRIGHT_BLUE,
  ANSI_CODE_BG_BRIGHT_MAGENTA,
  ANSI_CODE_BG_BRIGHT_CYAN,
  ANSI_CODE_BG_BRIGHT_WHITE,
} AnsiCode;

void ansi_esc(AnsiCode code);
void ansi_reset(void);

#endif
