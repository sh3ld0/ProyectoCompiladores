#include "MidiFile.h"
#include <fstream>
#include <iostream>
#include <map>
#include <vector>

std::map<std::string, int> notaAMidi = {
    {"C", 0},  {"C#", 1}, {"D", 2},  {"D#", 3}, {"E", 4},   {"F", 5},
    {"F#", 6}, {"G", 7},  {"G#", 8}, {"A", 9},  {"A#", 10}, {"B", 11}};

int convertirNota(std::string notaCompleta) {
  std::string nota = "";
  std::string octava = "";

  for (char c : notaCompleta) {
    if (isdigit(c)) {
      octava += c;
    } else {
      nota += c;
    }
  }

  if (notaAMidi.find(nota) == notaAMidi.end())
    return -1;
  return 12 + (stoi(octava) * 12) + notaAMidi[nota];
}

int main() {
  std::ifstream archivo("resources/musica.txt");
  if (!archivo.is_open()) {
    std::cerr << "No se pudo abrir el archivo de entrada.\n";
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
      int nota = convertirNota(palabra);
      if (nota != -1)
        notas.push_back(nota);
      else {
        std::cout << "Nota no reconocida: " << palabra << '\n';
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
  for (int nota : notas) {
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

  std::cout << "Archivo MIDI 'salida.mid' generado correctamente.\n";
  return 0;
}
