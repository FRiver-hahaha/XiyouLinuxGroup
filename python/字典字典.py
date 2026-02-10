users = {
    'aeinstein': {
        'first': 'albert', 
        'last': 'einstein',
        'location': 'princeton',
    },
    'mcurie': {
        'first': 'marie',
        'last': 'curie',
        'location': 'paris',
    },
}

for user_name, user_info in users.items():
    print("\nUsername:" + user_name.title())
    full_name = user_info['first'] + " " + user_info['last']
    location = user_info['location']
    
    print("\tFullname:" + full_name.title())
    print("\tLocation:" + location.title())