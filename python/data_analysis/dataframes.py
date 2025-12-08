import pandas as pd

data = {"Name": ["Spongebob", "Patrick", "Squidward"],
        "Age":  [30, 35, 40],
        }

df = pd.DataFrame(data, index = ["Employee 1", "Employee 2", "Employee 3"])

# 添加列

df["Job"] = ["Cook", "N/A", "Cashier"]

# 添加行
new_row = pd.DataFrame([{"Name": "Sandy", "Age": 18, "Job": "Engineer"},
                        {"Name": "louis", "Age": 19, "Job": "Student"},],
                       index = ["Employee 4", "Employee 5"])
df = pd.concat([df, new_row])

print(df)

# 数据清洗

df

