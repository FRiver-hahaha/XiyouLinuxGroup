alien_0 = {'color': 'green', 'points': 5}
print(alien_0)

alien_0['x'] = 0
alien_0['y'] = 1920
print(alien_0)

alien_1 = {}
alien_1['color'] = 'green'
alien_1['points'] = 6
print(alien_1)

print("the color is " + alien_1['color'])
alien_1['color'] = 'yellow'
print("the color is " + alien_1['color'])

del alien_0['points']
print(alien_0)

favorite_language = {
    'jen': 'python',
    'sarah': 'C',
    'edward': 'ruby',
    'phil': 'python',
}

friends = ['jen', 'sarah', 'edward']

for name in friends :
    print(name.title())

    if name in favorite_language.keys() :
        print("    Hi " + name.title() + ", I see your favorite language is " + favorite_language[name].title())