import pandas as pd

df = pd.read_csv(".venv/text.csv")

# 删除不需要的列
# df = df.drop(columns = ["Type2", "No"])

# 数据清洗
# df = df.dropna(subset = ["Type2"])
# df = df.fillna({"Type2": "None"})

df["Type1"] = df["Type1"].replace({"Grass": "GRASS"})


df["Type1"] = df["Type1"].replace({"Bug": "BUG",
                                   "Fire": "FIRE",
                                   "Water": "WATER"})

df["Legendary"] = df["Legendary"].astype(bool)

print(df.to_string())