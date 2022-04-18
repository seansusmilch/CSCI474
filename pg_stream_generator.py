from pathlib import Path
import random

MAX_PG_NUM = [15, 30, 60]
NUM_REQUESTS = 1024

random_pre = './randomstream'
locality_pre = './localitystream'

def write_stream(dest:Path, stream:list):
    with open(dest, 'w') as f:
        f.writelines(stream[0:NUM_REQUESTS])
        f.close()

for maxpgs in MAX_PG_NUM:
    # Random
    rand_stream = []
    for x in range(NUM_REQUESTS):
        rand_stream.append(f'{int(random.random()*maxpgs) + 1}\n')

    write_stream(Path(f'{random_pre}{maxpgs}.txt'), rand_stream)

    # Temporal locality
    locality_stream = []
    weights = [1,10]
    while len(locality_stream) < NUM_REQUESTS:
        if len(locality_stream) < 4:
            locality_stream.append(f'{int(random.random()*maxpgs) + 1}\n')
            continue

        choices = [
            locality_stream[random.randrange(-6,-2):-1],
            [f'{int(random.random()*maxpgs) + 1}\n']
        ]

        next = random.choices(choices, weights=weights)[0]
        if len(next) != 1:
            weights[1] += 1
        else:
            weights[0] += 1
        
        print(f'{weights}\t{next}')
        locality_stream += next

    write_stream(Path(f'{locality_pre}{maxpgs}.txt'), locality_stream)