import pandas as pd


x = pd.read_csv('./TD_Note/transactions.csv', sep=';')
print(x.describe())
print(x['Amount'].sort_values(ascending=True).head(10))