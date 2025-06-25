#include "Music.hpp"
#include <numeric>

namespace Music {
int Length::to_int() const {
  return num / dem;
}

Length Length::operator*(int n) const {
  int f = std::gcd(num * n, dem);
  return {num * n / f, dem / f};
}

Midi::Midi(const char* _name, int _measure_duration) :
    name{_name},
    measure_duration{_measure_duration} {
  midi.setTicksPerQuarterNote(measure_duration / 4);
}

void Midi::addNote(Music::Note note) {
  int midi_note = note.octave * 12 + std::to_underlying(note.tone);
  int length = (note.length * measure_duration).to_int();

  midi.addNoteOn(track, tick, canal, midi_note, 100);
  tick += length;
  midi.addNoteOff(track, tick, canal, midi_note);
}

void Midi::addRest(Music::Rest rest) {
  int length = (rest.length * measure_duration).to_int();
  tick += length;
}

void Midi::write() {
  midi.sortTracks();
  midi.write(name);
}
}; // namespace Music
