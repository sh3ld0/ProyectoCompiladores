#include "Lexer.hpp"
#include "MidiFile.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>

void debug_print_tokens(const std::vector<Token>& tokens) {
  auto tokenTypeToString = [](TokenType type) {
    switch (type) {
    case TokenType::NOTE_NAME:
      return "NOTE_NAME";
    case TokenType::OCTAVE:
      return "OCTAVE";
    case TokenType::DASH:
      return "DASH";
    case TokenType::FRACTION:
      return "FRACTION";
    case TokenType::REST:
      return "REST";
    case TokenType::LOOP:
      return "LOOP";
    case TokenType::NUMBER:
      return "NUMBER";
    case TokenType::LBRACE:
      return "LBRACE";
    case TokenType::RBRACE:
      return "RBRACE";
    case TokenType::END_OF_FILE:
      return "EOF";
    case TokenType::INVALID:
      return "INVALID";
    default:
      return "UNKNOWN";
    }
  };

  for (const auto& token : tokens) {
    std::cout << "Línea " << token.line << ":\t"
              << tokenTypeToString(token.type) << "\t-> \"" << token.lexeme
              << "\"\n";
  }
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Debe ingresar el archivo a convertir como primer argumento\n"
              << "y el nombre del archivo convertido como segundo argumento\n";
    return 1;
  }

  std::ifstream file(argv[1]);
  if (!file.is_open()) {
    std::cerr << "No se pudo abrir el archivo: " << argv[1] << '\n';
    return 1;
  }

  auto tokens = tokenize(file);

  // --- Mapa de notas a números MIDI ---
  std::map<std::string, int> notaAMidi = {
      {"C", 0},  {"C#", 1}, {"D", 2},  {"D#", 3}, {"E", 4},   {"F", 5},
      {"F#", 6}, {"G", 7},  {"G#", 8}, {"A", 9},  {"A#", 10}, {"B", 11}};

  smf::MidiFile midi;
  int measure_duration = 4 * 60;
  midi.setTicksPerQuarterNote(measure_duration / 4);
  int tick = 0;
  int track = 0;
  int canal = 0;

  for (size_t i = 0; i < tokens.size(); ++i) {
    if (tokens[i].type == TokenType::NOTE_NAME) {
      if (i + 3 < tokens.size() && tokens[i + 1].type == TokenType::OCTAVE &&
          tokens[i + 2].type == TokenType::DASH &&
          tokens[i + 3].type == TokenType::FRACTION) {

        std::string nota = tokens[i].lexeme;
        int octava = std::stoi(tokens[i + 1].lexeme);
        std::string fraccion = tokens[i + 3].lexeme;

        int midi_note = 12 + octava * 12 + notaAMidi[nota];
        int num = std::stoi(fraccion.substr(0, fraccion.find('/')));
        int den = std::stoi(fraccion.substr(fraccion.find('/') + 1));
        int duracion = (measure_duration * num) / den;

        midi.addNoteOn(track, tick, canal, midi_note, 100);
        midi.addNoteOff(track, tick + duracion, canal, midi_note);
        tick += duracion;

        i += 3; // saltar tokens que ya procesamos
      }
    } else if (tokens[i].type == TokenType::REST) {
      if (i + 2 < tokens.size() && tokens[i + 1].type == TokenType::DASH &&
          tokens[i + 2].type == TokenType::FRACTION) {

        std::string fraccion = tokens[i + 2].lexeme;
        int num = std::stoi(fraccion.substr(0, fraccion.find('/')));
        int den = std::stoi(fraccion.substr(fraccion.find('/') + 1));
        int duracion = (measure_duration * num) / den;

        tick += duracion;
        i += 2;
      }
    }
  }

  midi.sortTracks();
  midi.write(argv[2]);
  std::cout << "Archivo " << argv[2] << " generado correctamente.\n";
  return 0;
}
