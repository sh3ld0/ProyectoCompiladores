#include "MidiFile.h"
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <sstream>

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
  B = 11
};

struct NoteLength {
  int num, dem;

  int to_int() const {
    return num / dem;
  }

  NoteLength operator*(int n) const {
    int f = std::gcd(num * n, dem);
    return {num * n / f, dem / f};
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const NoteLength& duration) {
    return os << duration.num << '/' << duration.dem;
  }

  friend std::istream& operator>>(std::istream& is, NoteLength& duration) {
    is >> duration.num;

    if (is.peek() != '/') {
      duration.dem = 1;
    } else {
      is.ignore(1);
      is >> duration.dem;
    }

    return is;
  }
};

const std::map<std::string, Tone> string_to_tone = {
    {"C", Tone::C},   {"C#", Tone::CS}, {"D", Tone::D},   {"D#", Tone::DS},
    {"E", Tone::E},   {"F", Tone::F},   {"F#", Tone::FS}, {"G", Tone::G},
    {"G#", Tone::GS}, {"A", Tone::A},   {"A#", Tone::AS}, {"B", Tone::B}};

std::pair<int, NoteLength> to_key_and_length(std::string&& note_str) {
  std::stringstream ss(std::move(note_str));

  std::string note_tone = "";
  std::string note_octave = "";

  for (char c = ss.get(); c != '-'; c = ss.get())
    if (std::isdigit(c))
      note_octave += c;
    else
      note_tone += c;

  NoteLength length;
  ss >> length;

  auto octave = 12 * std::stoi(note_octave);
  auto tone = std::to_underlying(string_to_tone.at(note_tone));

  return {tone + octave, length};
}
}; // namespace Music

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr
        << "Ingrese el nombre del archio a compilar como primer argumento\n";
    return 1;
  }

  std::ifstream in_file(argv[1]);
  if (!in_file.is_open()) {
    std::cerr << "No se pudo abrir el archivo " << argv[1] << '\n';
    return 1;
  }

  smf::MidiFile midi;
  int measure_duration = 4 * 60;
  midi.setTicksPerQuarterNote(measure_duration / 4);
  int tick = 0;
  int track = 0;
  int canal = 0;

  std::string word;
  while (in_file >> word) {
    auto [key, length] = Music::to_key_and_length(std::move(word));
    if (key == -1) {
      std::cout << "Símbolo no reconocido: " << word << '\n';
      return 2;
    }

    int duration = (length * measure_duration).to_int();
    midi.addNoteOn(track, tick, canal, key, 100);
    midi.addNoteOff(track, tick + duration, canal, key);
    tick += duration;
  }

  in_file.close();

  midi.sortTracks(); // ordenar eventos por tiempo por si las dudas
  midi.write("salida.mid");
  return 0;
}
