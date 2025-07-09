#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "MidiFile.h"
#include <map>

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
  Length() = default;
  Length(int, int);

  int to_int() const;
  Length& operator+=(Length);
  Length operator*(int n) const;
  bool operator==(Length) const;
};

struct Note {
  Tone tone;
  int octave;
  Length length;
};

struct Rest {
  Length length;
};

struct Signature {
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

static const std::map<Music::Tone, std::string> tone_to_string = {
    {Music::Tone::C, "C"},   {Music::Tone::CS, "C#"}, {Music::Tone::D, "D"},
    {Music::Tone::DS, "D#"}, {Music::Tone::E, "E"},   {Music::Tone::F, "F"},
    {Music::Tone::FS, "F#"}, {Music::Tone::G, "G"},   {Music::Tone::GS, "G#"},
    {Music::Tone::A, "A"},   {Music::Tone::AS, "A#"}, {Music::Tone::B, "B"}};

template <>
struct std::formatter<Music::Length> : std::formatter<std::string> {
  auto format(Music::Length l, format_context& c) const {
    if (l.dem == 1)
      return formatter<string>::format(std::format("{}", l.num), c);
    else
      return formatter<string>::format(std::format("{}/{}", l.num, l.dem), c);
  }
};

template <>
struct std::formatter<Music::Note> : std::formatter<std::string> {
  auto format(Music::Note n, format_context& c) const {
    return formatter<string>::format(
        std::format("{}{}-{}", tone_to_string.at(n.tone), n.octave, n.length),
        c);
  }
};

template <>
struct std::formatter<Music::Rest> : std::formatter<std::string> {
  auto format(Music::Rest r, format_context& c) const {
    return formatter<string>::format(std::format("R-{}", r.length), c);
  }
};

#endif
