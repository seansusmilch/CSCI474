from pathlib import Path
import random

MAX_PG_NUM = 60

random_path = Path('./randomstream.txt')
locality_path = Path('localitystream.txt')

def write_stream(dest:Path, stream:list):
    with open(dest, 'w') as f:
        f.writelines(stream)
        f.close()

rand_stream = []
for x in range(1500):
    rand_stream.append(f'{int(random.random()*MAX_PG_NUM) + 1}\n')

write_stream(random_path, rand_stream)

# Temporal locality
locality_stream = []
weights = [1,10]
while len(locality_stream) < 1500:
    if len(locality_stream) < 4:
        locality_stream.append(f'{int(random.random()*MAX_PG_NUM) + 1}\n')
        continue

    choices = [
        locality_stream[random.randrange(-6,-2):-1],
        [f'{int(random.random()*MAX_PG_NUM) + 1}\n']
    ]

    next = random.choices(choices, weights=weights)[0]
    if len(next) != 1:
        weights[1] += 1
    else:
        weights[0] += 1
    
    print(f'{weights}\t{next}')
    locality_stream += next

write_stream(locality_path, locality_stream)