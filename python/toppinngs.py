available_toppings = ['mushrooms', 'olives', 'green peppers', 'pepperoni', 'pineapple', 'extra cheese']
requested_toppings = ['mushrooms', 'french fries', 'extra cheese']

for requseted_topping in requested_toppings :
    if requseted_topping in available_toppings :
        print(requseted_topping.title() + " in available_toppings.")
    else :
        print("sorry, we don't have " + requseted_topping.title() + ".")