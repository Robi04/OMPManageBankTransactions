# Manage and analyse transactions from banks 
## Each transaction is recorded and stored inside a .csv file
- id account who gives
- id account who receive
- amount
- signature

### Details
- Signature is a numeric result that checks if the transaction is a fraud or not 
- After each transaction -> update of statistics metrics

## Content:
main.cpp : main program with the treatment in multithreading
main_seq.cpp : same program but in sequential
