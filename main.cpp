#include "MidiFile.h"
#include <fstream>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

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

const std::map<std::string, Tone> string_to_tone = {
    {"C", Tone::C},   {"C#", Tone::CS}, {"D", Tone::D},   {"D#", Tone::DS},
    {"E", Tone::E},   {"F", Tone::F},   {"F#", Tone::FS}, {"G", Tone::G},
    {"G#", Tone::GS}, {"A", Tone::A},   {"A#", Tone::AS}, {"B", Tone::B}};

int note_to_pitch(std::string note) {
  std::string note_tone = "";
  std::string note_octave = "";

  for (char c : note)
    if (std::isdigit(c))
      note_octave += c;
    else
      note_tone += c;

  auto octave = 12 * std::stoi(note_octave);
  auto tone = std::to_underlying(string_to_tone.at(note_tone));

  return tone + octave;
}
}; // namespace Music

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr
        << "Ingrese el nombre del archio a compilar como primer argumento\n";
    return 1;
  }

  std::ifstream archivo(argv[1]);
  if (!archivo.is_open()) {
    std::cerr << "No se pudo abrir el archivo " << argv[1] << '\n';
    return 1;
  }

  std::vector<int> notas;
  std::string palabra;
  bool playMode = false;
  // almacena las notas en un tipo de dato que acepta midi, en este caso numeros
  // de 0 a 127
  while (archivo >> palabra) {
    if (palabra == "play") {
      playMode = true;
    } else if (playMode) {
      int nota = Music::note_to_pitch(palabra);
      if (nota != -1)
        notas.push_back(nota);
      else {
        std::cout << "Símbolo no reconocido: " << palabra << '\n';
        return 2;
      }
    }
  }

  archivo.close();

  smf::MidiFile midi;

  int track = 0;
  int tick = 0;
  int duracion = 120; // duración de nota
  int canal = 0;
  int cont = 0;
  for (auto nota : notas) {
    cont++;
    if (cont == 8) {
      tick += duracion;
    } else if (cont == 12) {
      tick += duracion;
    }
    midi.addNoteOn(track, tick, canal, nota, 100);
    midi.addNoteOff(track, tick + duracion, canal, nota);
    tick += duracion;
  }

  midi.sortTracks(); // ordenar eventos por tiempo por si las dudas
  midi.write("salida.mid");
  return 0;
}
