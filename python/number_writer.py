import json

numbers = [1,2,3,4,5,6,7,8,9,10]

file_name = 'numbers.json'
with open(file_name, "w") as f_obj:
    json.dump(numbers, f_obj)