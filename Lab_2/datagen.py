import sys
from random import randint, choice, random

import datetime

now = datetime.datetime.now()

flats_amount = 100

if len(sys.argv) > 1:
    flats_amount = int(sys.argv[1])

output_filename = "data_{}{}{}_{}.txt".format(now.hour, now.minute, now.second, flats_amount)

if len(sys.argv) > 2:
    output_filename = sys.argv[2]

def get_address():
    streets = [
        "Gladwell Street",
        "Modoc Alley",
        "Simons Hollow Road",
        "Poplar Chase Lane",
        "Oliverio Drive",
        "Michael Street",
        "Half and Half Drive",
        "Tetrick Road",
        "Dovetail Drive",
        "Hilltop Street",
        "Park Boulevard",
        "Snowbird Lane"
    ]
    return "{}, {}".format(choice(streets), randint(1, 80))

def get_price():
    min_price = 100.00
    max_price = 300.00
    price = min_price + random() * (max_price - min_price)
    return round(price, 2)

def get_rooms():
    min_rooms = 1
    max_rooms = 10
    return randint(min_rooms, max_rooms)

def get_area(rooms):
    min_room_area =  5.00
    max_room_area = 20.00
    area = rooms * (min_room_area + random() * max_room_area)
    return round(area, 2)

def get_pets():
    return choice(["yes", "no"])

def get_trim():
    return choice(["yes", "no"])

def get_time():
    day = randint(1, 31)
    month = randint(1, 12)
    year = randint(1900, 2020)
    return "{:02d}.{:02d}.{:04d}".format(day, month, year)

def get_owners():
    min_owners = 1
    max_owners = 40
    return randint(min_owners, max_owners)

def get_lodgers():
    min_lodgers = 1
    max_lodgers = 10
    return randint(min_lodgers, max_lodgers)

def get_primary_flat():
    rooms = get_rooms()
    return "; ".join(map(str, [
        get_address(),
        get_area(rooms),
        rooms,
        get_price(),
        "PRIMARY",
        get_trim()
    ]))

def get_secondary_flat():
    rooms = get_rooms()
    return "; ".join(map(str, [
        get_address(),
        get_area(rooms),
        rooms,
        get_price(),
        "SECONDARY",
        get_pets(),
        get_time(),
        get_owners(),
        get_lodgers()
    ]))

def get_flat():
    return choice((get_primary_flat, get_secondary_flat))()

with open(output_filename, "w") as file:
    for i in range(flats_amount):
        print(get_flat(), file=file)
