import random
from fractions import Fraction

# Definiciones de la gramática
pitches = ["C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"]
octaves = [3, 4, 5]
lengths = ["1", "1/2", "1/4", "1/8", "2/4", "3/4", "1/16"]
length_fractions = [Fraction(l) for l in lengths]

# Lista de posibles compases
time_signatures = [Fraction(3, 4), Fraction(4, 4), Fraction(6, 8)]


def random_length(max_length):
    valid = [l for l in length_fractions if l <= max_length]
    return random.choice(valid) if valid else None


def random_pitch():
    return f"{random.choice(pitches)}{random.choice(octaves)}"


def generate_note(length_frac):
    return f"{random_pitch()}-{length_frac}"


def generate_rest(length_frac):
    return f"R-{length_frac}"


def generate_bar(time_sig):
    bar = ["|"]
    remaining = time_sig
    while remaining > 0:
        length = random_length(remaining)
        if not length:
            break
        token = (
            generate_rest(length)
            if random.random() < 0.2
            else generate_note(length)
        )
        bar.append(token)
        remaining -= length
    return " ".join(bar)


def generate_score(n_symbols):
    output = []
    symbol_count = 0
    current_signature = random.choice(time_signatures)
    output.append(
        f"SIGNATURE-{current_signature.numerator}/{current_signature.denominator}"
    )

    while symbol_count < n_symbols:
        # Probabilidad de cambiar compás
        if random.random() < 0.15:
            current_signature = random.choice(time_signatures)
            output.append(
                f"SIGNATURE-{current_signature.numerator}/{current_signature.denominator}"
            )

        bar = generate_bar(current_signature)
        symbols_in_bar = len(bar.split()) - 1  # Excluye "|"
        if symbol_count + symbols_in_bar > n_symbols:
            break
        output.append(bar)
        symbol_count += symbols_in_bar

    return "\n".join(output)


if __name__ == "__main__":
    N = int(input("N? "))
    output_file = f"score.txt"
    score_text = generate_score(N)

    with open(output_file, "w") as f:
        f.write(score_text)

    print(f"Archivo generado: {output_file}")
