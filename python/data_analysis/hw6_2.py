import pandas as pd

def data():
    df = pd.read_csv("scores.csv")
    df = df.dropna()
    df = df[
        (df["语文"] >= 0) & (df["语文"] <= 150) & (df["数学"] >= 0) & (df["数学"] <= 150) & (df["英语"] >= 0) & (df["英语"] <= 150)]
    df = df.drop_duplicates()
    df["总分"] = df["语文"] + df["数学"] + df["英语"]
    print(df)

    with open("score_statistics.txt", 'w', encoding="utf-8") as f:
        words = ["语文", "数学", "英语", "理综"]
        for each in words:
            f.write(f"{each}：")
            t = df[f"{each}"].sum()
            m = df[f"{each}"].median()
            c = df[f"{each}"].count()
            f.write(f"平均分：{(t / c):.2f}")
            f.write(f"总分：{t:.2f}")
            f.write(f"中位数：{m:.2f}\n")

if __name__ == "__main__":
    data()