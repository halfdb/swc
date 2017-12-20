#! /usr/bin/python3
import random
import string

chars = list(string.ascii_letters);
chars += ['"',',','/','*','+','-','=','==','(',')','_','&&','||','!','!=','<','>', ';', ' ', '%',]
chars += [' int ', ' float ', ' void ', ' for ', ' in ', ' ... ', ' { ', '}', ' if ', ' while ',' read ', ' print ', ' return ']

l = len(chars)
o = ''

for _ in range(100):
  o += chars[random.randrange(l)]
o += '\n'

with open('8.sw', 'w') as f:
  f.write(o)

