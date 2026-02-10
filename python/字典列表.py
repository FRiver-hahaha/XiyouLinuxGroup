# alien_0 = {'color': 'green', 'points': 1}
# alien_1 = {'color': 'yellow', 'points': 2}
# alien_2 = {'color': 'red', 'points': 3}

# aliens = [alien_0, alien_1, alien_2]

# for alien in aliens: 
#     print(alien)

aliens = []

for alien_number in range(1, 31) :
    new_alien = {
        'color': 'green',
        'points': alien_number,
        'speed': 'slow',
    } 
    aliens.append(new_alien)

for alien in aliens[:5] :
    alien['color'] = 'yellow'
    alien['points'] = alien['points'] + 1
    alien['speed'] = 'medium'
    print(alien)
