import pandas as pd

df = pd.read_csv(".venv/text.csv", index_col = ["Name"])


# print(df.mean(numeric_only = True))# 均值
# print(df.sum(numeric_only = True))
# print(df.min(numeric_only = True))
# print(df.max(numeric_only = True))
print(df["Weight"].count())

group = df.groupby("Type1")

print(group["Height"].min())
