import pandas as pd

data = {"Name": ["louis", "wb", "yjb"],
        "Age": [18, 19, 18],
        "Score": [100, 200, 300]}

df = pd.DataFrame(data, index = ["Student 1", "Student 2", "Student 3"])

df["Job"] = ["a", "b", "c"]

newRow = pd.DataFrame([{"Name": "yjt", "Age": 19, "Score": 400, "Job": "d"}],
                      index = ["Student 4"])

df = pd.concat([df, newRow])
print(df)