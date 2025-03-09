import random
import os

output_dir = "token_files"
os.makedirs(output_dir, exist_ok=True)

num_files = 10
file_size_kb = 2000
file_size_bytes = file_size_kb * 1024

words = ["token", "tokenizar", "test", "prueba", "probando", "aleatorio"]

min_words_per_line = 5
max_words_per_line = 15

for i in range(1, num_files + 1):
    file_path = os.path.join(output_dir, f"tokens_{i}.txt")

    with open(file_path, "w") as f:
        current_size = 0
        while current_size < file_size_bytes:
            num_words = random.randint(min_words_per_line, max_words_per_line)
            line = " ".join(random.choices(words, k=num_words)) + "\n"
            f.write(line)
            current_size += len(line)

