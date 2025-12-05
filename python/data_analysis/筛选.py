import pandas as pd

df = pd.read_csv(".venv/text.csv", index_col ="Name")

tall_pokemon = df[df["Height"] >= 2]
heavy_pokemon = df[df["Weight"] >= 5.0]
water_pokemon = df[df["Type1"] == "Water"]
Poison_pokemon = df[(df["Type2"] == "Poison") & (df["Type1"] == "Water")]



print(Poison_pokemon)