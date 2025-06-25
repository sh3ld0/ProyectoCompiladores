#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "MidiFile.h"

namespace Music {
enum class Tone {
  C = 0,
  CS = 1,
  D = 2,
  DS = 3,
  E = 4,
  F = 5,
  FS = 6,
  G = 7,
  GS = 8,
  A = 9,
  AS = 10,
  B = 11,
};

struct Length {
  int num, dem;

  int to_int() const;
  Length operator*(int n) const;
};

struct Note {
  Tone tone;
  int octave;
  Length length;
};

struct Rest {
  Length length;
};

struct Bar {};

struct Midi {
  smf::MidiFile midi;
  const char* name;
  const int measure_duration;
  int tick = 0;
  int track = 0;
  int canal = 0;

  Midi(const char* _name, int _measure_duration);
  void addNote(Music::Note note);
  void addRest(Music::Rest rest);
  void write();
};
}; // namespace Music

#endif
