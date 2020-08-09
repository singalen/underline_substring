#include <stdio.h>
#include <stdbool.h>

#ifndef MIN
#define MIN(_a,_b) ((_a)<(_b)?(_a):(_b))
#endif


static size_t
print_till_eol(const char * psz_line)
{
  int count = 0;
  while (psz_line[count] != 0 && psz_line[count] != '\n') {
    putchar(psz_line[count]);
    count++;
  }
  return count;
}

static void
print_underline(size_t offset, size_t length)
{
  printf ("%*s", (int) offset, "");
  for (int i = 0; i < length; i++) {
    putchar('^');
  }
  putchar('\n');
}


static void
print_command_underlined(char * psz_command, size_t sub_offset, size_t sub_length)
{
  // TODO: Doesn't account for \t, \r.

  long line_length = 0;

  do {
    line_length = print_till_eol(psz_command);
    putchar('\n');
    psz_command += line_length; // and a newline/null

    if (*psz_command == '\n') {
      psz_command++;
    }

    if (sub_offset < line_length) {
      size_t underline_length = MIN(sub_length, line_length - sub_offset);
      print_underline(sub_offset, underline_length);
      break;
    }

    sub_offset -= line_length;

    // Skip the newline
    if (sub_offset == 0) {
      // The offset can, in theory, start on newline.

      sub_length--;
      while (*psz_command == '\n') {
        putchar('\n');
        // If only \n-s need to be underlined, then underline the last of them.
        if (sub_length == 1) {
          puts ("^");
          psz_command++;
          break;
        }
        psz_command++;
        sub_length--;
      }

    } else {
      sub_offset--;
    }

  } while (*psz_command != '\0');

  int tail_length = printf("%s", psz_command) - 1;
  if (tail_length > 0 && psz_command[tail_length] != '\n') {
    putchar('\n');
  }

}


static void
print_command_underlined0(char * const psz_command, size_t offset, size_t length)
{
  char *psz_line = psz_command;
  bool line_matched = false;
  size_t line_length = 0;
  size_t underline_offset = offset;
  size_t underline_length = length;

  while (*psz_line) {
    if (underline_offset == line_length) {
      line_matched = true;
      break;
    }

    if (*psz_line == '\t') {
      puts("    ");
      line_length += 4;
      // TODO: Add 3 to underline_length, but only if line_matched.
    } else {
      putchar (*psz_line);
      line_length++;
    }
    psz_line++;

    // TODO: \r

    if (psz_line[-1] == '\n') {
      underline_offset -= line_length;
      line_length = 0;
      continue;
    }
  }

  // print the rest of the matched line.
  if (line_matched) {
    // unless it starts with \n, then just print them.
    while (*psz_line == '\n') {
      putchar('\n');
      psz_line++;
      underline_offset = 0;
      underline_length--;
      // If only \n-s need to be underlined, then underline the last of them.
      if (underline_length == 0) {
        puts ("^\n");
        return;
      }
    }

    int tail_length = 0;
    while (*psz_line && *psz_line != '\n') {
      putchar (*psz_line);
      psz_line++;
      tail_length++;
    }

    if (tail_length < underline_length) {
      underline_length = tail_length;
    }

    putchar ('\n');

    printf ("%*s", (int) underline_offset, "");
    for (int i = 0; i < underline_length; i++) {
      putchar('^');
    }
  }

  putchar ('\n');

  if (*psz_line == '\n') {
    psz_line++;
  }

  if (*psz_line) {
    puts (psz_line);
  }
}

void print_test(const char *psz_desc, char * const psz_command, size_t offset, size_t length) {
  puts(psz_desc);
  print_command_underlined(psz_command, offset, length);
  puts("=======================================");
}

int main() {
  setvbuf(stdout, NULL, _IONBF, 0);

  print_test("underlines \"hello\":", "echo hello", 5, 5);
  print_test("underlines \"hello\" too:", "echo a\necho hello", 12, 5);
  print_test("skips \\n, underlines second \"echo\":", "echo a\necho hello", 6, 5);
  print_test("skips \\n, underlines second \"echo\" too:", "echo a\necho hello", 7, 4);

  print_test("offset out of range", "012345", 6, 4);
  print_test("offset+len out of range", "012345", 4, 4);
  print_test("empty line, at the beginning", "", 0, 2);
  print_test("empty line, out of range", "", 1, 2);

  print_test("underline three newlines and a 3", "\n\n\n3", 2, 2);
  print_test("underline three newlines (namely, the third)", "\n\n\n", 1, 2);
  print_test("underline three newlines (namely, the third) and print fourth", "\n\n\n\n", 1, 2);

  print_test("underline 1", "0123456789", 1, 1);
  print_test("underline 23", "0123456789", 2, 2);
  print_test("underline 89", "0123456789", 8, 2);
  print_test("underline 9 and trim the remaining", "0123456789\n1234567890", 9, 3);
  print_test("skip newline and underline 12", "0123456789\n1234567890", 10, 3);
  print_test("skip newline and underline 1234", "0123456789\n123456789\n1234", 20, 5);

  print_test("tab to 4 spaces", "01234\t6", 5, 2);

  return 0;
}
