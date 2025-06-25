#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "MidiFile.h"
#include <numeric>
#include <utility>

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

  int to_int() const {
    return num / dem;
  }

  Length operator*(int n) const {
    int f = std::gcd(num * n, dem);
    return {num * n / f, dem / f};
  }
};

struct Note {
  Tone tone;
  int octave;
  Length length;
};

struct Rest {
  Length length;
};

struct Midi {
  smf::MidiFile midi;
  const char* name;
  const int measure_duration;
  int tick = 0;
  int track = 0;
  int canal = 0;

  Midi(const char* _name, int _measure_duration) :
      name{_name},
      measure_duration{_measure_duration} {
    midi.setTicksPerQuarterNote(measure_duration / 4);
  }

  void addNote(Music::Note note) {
    int midi_note = note.octave * 12 + std::to_underlying(note.tone);
    int length = (note.length * measure_duration).to_int();

    midi.addNoteOn(track, tick, canal, midi_note, 100);
    tick += length;
    midi.addNoteOff(track, tick, canal, midi_note);
  }

  void addRest(Music::Rest rest) {
    int length = (rest.length * measure_duration).to_int();
    tick += length;
  }

  void write() {
    midi.sortTracks();
    midi.write(name);
  }
};
}; // namespace Music

#endif
