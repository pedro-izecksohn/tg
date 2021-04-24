// tg.c - old tg.py - A simple Turtle Graphics that exports a PGM.
// By: Pedro Izecksohn
// Version: 2021/April/24 16:44
// License:
// 1) This software while in source code format may be freely distributed.
//    To be able to distribute this software in executable format you must buy a commercial license from Pedro Izecksohn.
//    You may modify this software if you distribute it under this same license.
// 2) This software may damage your hardware and or overwrite your file. So be carefull. Read the source code for more details.

#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Point
{
  double x, y;
} Point;

typedef struct PGM
{
  unsigned int width, height;
  unsigned char *ba;
  const char *comment;
} PGM;

const char * const default_comment = "";

PGM *new_PGM (const unsigned int width, const unsigned int height)
{
  PGM *ret = malloc (sizeof(PGM));
  if (!ret) return ret;
    ret->width = width;
    ret->height = height;
    if (!(ret->ba = malloc (((size_t)width)*height))) {free (ret); return NULL;}
    unsigned int y = 0;
    for (; y<height; ++y)
    {
      unsigned int x=0;
      for (; x<width; ++x)
      {
        ret->ba [x+(y*width)] = 0;
      }
    }
    ret->comment = default_comment;
    return ret;
}

void PGM_turn (PGM * const self, const Point point, const unsigned char value)
{
  unsigned long int x = point.x, y = point.y;
  //fprintf (stderr, "PGM_turn (%p, (%lu,%lu), %hhu);\n", self, x, y, value);
  if (x >= self->width) return;
  if (y >= self->height) return;
  self->ba [x+(y*self->width)] = value;
}

void PGM_write (const PGM * const self, FILE * const f)
{
  assert (self->comment);
  fprintf (f, "P5\n# %s\n%u %u\n255\n", self->comment, self->width, self->height);
  fwrite (self->ba, 1, self->width*self->height, f);
}

typedef struct Screen
{
  unsigned int width, height;
  PGM *pgm;
} Screen;

Screen *new_Screen (unsigned int width, unsigned int height)
{
  Screen *ret = malloc (sizeof(Screen));
  if (!ret) return ret;
  ret->width = width;
  ret->height = height;
  ret->pgm = new_PGM (width, height);
  if (!ret->pgm) {free (ret); return NULL;}
  return ret;
}

void Screen_turn (Screen * const self, const Point point, const unsigned char color)
{
  //fprintf (stderr, "Screen_turn (%p, ..., %hhu):\n", self, color);
  Point real;
  real.x = point.x*self->width;
  real.y = point.y*self->height;
  //fprintf (stderr, "%lf,%lf = %d,%d\n", point.x, point.y, (int)real.x, (int)real.y);
  PGM_turn (self->pgm, real, color);
}

typedef struct Command
{
  char c;
  double d;
  char *aux;
} Command;

char *new_String (const char * const old)
{
  if (old)
  {
    size_t len = strlen (old);
    char *ret = malloc (len+1);
    strcpy (ret, old);
    return ret;
  }
  char *ret = malloc (1);
  *ret = 0;
  return ret;
}

char *String_append (char *self, const char c)
{
  size_t len = strlen (self);
  self = realloc (self, len+2);
  self[len] = c;
  ++len;
  self[len] = 0;
  return self;
}

Command *new_Command (const char *line)
{
  //fprintf (stderr, "new_Command (%p):\n", line);
  Command *self = malloc (sizeof(Command));
  if (!self) return self;
  self->c = 0;
  self->d = NAN;
  self->aux = NULL;
  if (!line) return self;
  if (*line)
  {
    while (*line && isspace(*line)) ++line;
    self->c = *line;
    if (!*line) return self;
    ++line;
    while (*line && isspace(*line)) ++line;
    if (!*line) return self;
    char * endptr;
    self->d = strtod (line, &endptr);
    if (line==endptr)
    {
      self->d = NAN, self->aux = new_String (line);
      char * const p = strchr (self->aux, '\n');
      if (p) *p = 0;
    }
  }
  return self;
}

void Command_print (const Command * const self, FILE * const f)
{
  fprintf (f, "Command %c", self->c);
  if (!isnan(self->d)) fprintf (f, " %lf", self->d);
  else if (self->aux) fprintf (f, " %s", self->aux);
  fprintf (f, "\n");
}

typedef struct Vector
{
  size_t n;
  void ** array;
} Vector;

Vector *new_Vector (void)
{
  Vector *ret = malloc (sizeof(Vector));
  if (!ret) return ret;
  ret->n = 0;
  ret->array = malloc (sizeof(void*));
  if (!ret->array)
  {
    free (ret);
    return NULL;
  }
  ret->array[0]=NULL;
  return ret;
}

void Vector_append (Vector * const self, const void * const p)
{
  //fprintf (stderr, "Vector_append (%p, %p):\n", self, p);
  size_t n = self->n;
  void ** const newarray = realloc (self->array, (n+1)*sizeof(p));
  if (!newarray) {perror ("Vector_append"); abort();}
  self->array = newarray;
  newarray[n++] = (void*)p;
  self->n = n;
}

char *freadline (FILE * const f)
{
  char *ret = new_String (NULL);
  while (!(feof(f)||ferror(f)))
  {
    const int C = fgetc (f);
    if ((EOF==C)&&(feof(f)||ferror(f))) return ret;
    ret = String_append (ret, (char)C);
    if ('\n'==C) return ret;
  }
  return ret;
}

// Returns a list of commands.
Vector *readCode (FILE *f)
{
  Vector * const code = new_Vector();
  while (!feof(f))
  {
    char *line = freadline (f);
    if (ferror(f))
    {
      perror ("freadline");
      exit (EXIT_FAILURE);
    }
    //printf ("%s\n", line);
    Command * const command = new_Command (line);
    //printf ("readCode: I read the command:\n");
    //Command_print (command, stdout);
    Vector_append (code, command);
  }
  return code;
}

typedef struct Frame
{
  size_t rindex, ntimes;
} Frame;

Frame * new_Frame (const size_t rindex, const size_t ntimes)
{
  Frame *ret = malloc (sizeof(Frame));
  if (!ret) return ret;
  ret->rindex = rindex;
  ret->ntimes = ntimes;
  return ret;
}

typedef struct Turtle
{
  Screen screen;
  Point position;
  double angle;
  bool drawing;
  unsigned char color;
  double step_length;
} Turtle;

Turtle * new_Turtle (const Screen * const screen)
{
  Turtle * const self = malloc (sizeof(Turtle));
  if (!self) return NULL;
  assert (screen);
  self->screen = *screen;
  self->position.x = 0.5;
  self->position.y = 0.5;
  self->angle = -(M_PI/2); // radians
  self->drawing = true;
  self->color = 255;
  self->step_length = screen->width > screen->height ? 0.5/screen->width : 0.5/screen->height;
  //fprintf (stderr, "step_length = %lf\n", self->step_length);
  return self;
}

double r2d (const double radians)
{
  return (radians*180)/M_PI;
}

double d2r (const double degrees)
{
  return (degrees*M_PI)/180;
}

void Turtle_print (Turtle *self, FILE *f)
{
  fprintf (f, "<Turtle position=%lf,%lf degrees=%lf step_length=%lf />\n", self->position.x, self->position.y, r2d(self->angle), self->step_length);
}

void Turtle_penDown (Turtle * const self)
{
    self->drawing = true;
    Screen_turn (&self->screen, self->position, self->color);
}

//size_t forwards = 0;

void Turtle_forward (Turtle * const self, double distance)
{
  //fprintf (stderr, "Turtle_forward (%p, %lf):\n", self, distance);
  //fprintf (stderr, "This is the %lu time Turtle_forward is called.\n", ++forwards);
  assert (self->step_length > 0);
  while (distance>0)
  {
    self->position.x += (cos(self->angle)*self->step_length);
    self->position.y += (sin(self->angle)*self->step_length);
    //fprintf (stderr, "distance = %lf\tx = %lf\ty = %lf\n", distance, self->position.x, self->position.y);
    if (self->drawing) Screen_turn (&self->screen, self->position, self->color);
    distance -= self->step_length;
  }
}

//size_t steps = 0;

void Turtle_executeCode (Turtle * const self, Vector * const code)
{
  //fprintf (stderr, "Turtle_executeCode (%p, %p):\n", self, code);
  Vector *stack = new_Vector();
  long int index = -1;
  while (1)
  {
    //++steps;
    //fprintf (stderr, "steps = %lu\n", steps);
    ++index;
    if (index == code->n) return;
    //fprintf (stderr, "index = %ld\n", index);
    const Command * const command = code->array [index];
    //Command_print (command, stderr);
    if (command->c == 'A')
    {
      self->angle = d2r(command->d);
      continue;
    }
    if (command->c == 'X')
    {
      self->position.x = command->d;
      continue;
    }
    if (command->c == 'Y')
    {
      self->position.y = command->d;
      continue;
    }
    if (command->c == 'F')
    {
      Turtle_forward (self, command->d);
      continue;
    }
    if (command->c == 'T')
    {
      //fprintf (stderr, "old angle = %lf radians\n", self->angle);
      self->angle += d2r (command->d);
      //fprintf (stderr, "new angle = %lf radians\n", self->angle);
      continue;
    }
    if (command->c == 'U')
    {
      self->drawing = false;
      continue;
    }
    if (command->c == 'D')
    {
      Turtle_penDown (self);
      continue;
    }
    if (command->c == 'R')
    {
      Vector_append (stack, new_Frame (index, command->d));
      continue;
    }
    if (command->c == 'E')
    {
      if (stack->n == 0)
      {
        fprintf (stderr, "The command index %ld is an E without a corresponding R .\n", index);
        exit (EXIT_FAILURE);
      }
      --(stack->n);
      Frame * const frame = stack->array[stack->n];
      //fprintf (stderr, "frame = %p\n", frame);
      if (frame->ntimes > 1)
      {
        --(frame->ntimes);
        index = frame->rindex;
        Vector_append (stack, frame);
        continue;
      }
    }
    if (command->c == 'C')
    {
      self->color = command->d;
      continue;
    }
    if (command->c == '#')
    {
      if (self->screen.pgm->comment == default_comment)
          self->screen.pgm->comment = command->aux;
      continue;
    }
    if (command->c == 'P')
    {
      fprintf (stderr, "The usage of the command P is deprecated.\n");
      self->step_length = command->d;
      if (self->step_length <=0)
      {
        fprintf (stderr, "At line %lu: The step_length must be greater than 0.\n", index+1);
        abort();
      }
      continue;
    }
    if (command->c == 'B')
    {
      Turtle_print (self, stdout);
      continue;
    }
  }
}

int main (int argc, char ** argv)
{
  if (argc != 4)
  {
    printf ("cat file.tg | tg width height file.pgm");
    exit (EXIT_FAILURE);
  }
  const Screen * const screen = new_Screen (atoi(argv[1]), atoi(argv[2]));
  const char * const fname = argv[3];
  if (fopen (fname, "rb"))
  {
    fprintf (stderr, "%s exists.\n", fname);
    return EXIT_FAILURE;
  }
  FILE * const f = fopen (fname, "wb");
  if (!f)
  {
    perror ("fopen");
    exit (EXIT_FAILURE);
  }
  Turtle * const turtle = new_Turtle (screen);
  Vector * const code = readCode (stdin);
  Turtle_executeCode (turtle, code);
  PGM_write (screen->pgm, f);
  return 0;
}
