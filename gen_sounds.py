import wave, struct, math, os

def generate_tone(filename, freqs, duration_ms, vol=0.5):
    sample_rate = 44100
    num_samples = int(sample_rate * (duration_ms / 1000.0))
    
    with wave.open(filename, 'w') as w:
        w.setnchannels(1)
        w.setsampwidth(2)
        w.setframerate(sample_rate)
        
        for i in range(num_samples):
            # To make it interesting, average multiple frequencies or change freq over time
            t = i / sample_rate
            value = 0
            if isinstance(freqs, list):
                for f in freqs:
                    value += math.sin(2.0 * math.pi * f * t)
                value = value / len(freqs)
            else:
                value = math.sin(2.0 * math.pi * freqs * t)
                
            # Envelope (fade out)
            env = 1.0 - (i / num_samples)
            sample = int(vol * 32767.0 * value * env)
            w.writeframesraw(struct.pack('<h', sample))

os.makedirs('assets', exist_ok=True)

# Click: short high pip
generate_tone('assets/click.wav', 800, 50, 0.5)

# Win: chord
generate_tone('assets/win.wav', [440, 554, 659, 880], 1000, 0.4)

print("Generated audio files in assets/")
