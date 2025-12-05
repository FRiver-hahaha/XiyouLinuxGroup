import pandas as pd

income = {"day 1": 3000, "day 2": 3100, "day 3": 2800,
          "day 4": 3200, "day 5": 3050}

seriesIncomes = pd.Series(income)

print(seriesIncomes)

