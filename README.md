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


# Results 

### Discovery of the input data + Check/benchmarks of the outputs

Did use python->pandas to explore the dataset and find the most import pieces of information (min,max,mean,number of rows...)

|       | Source Account | Destination Account | Amount     | Signature     |
| ----- | -------------- | ------------------- | ---------- | ------------- |
| count | 1000000.000000 | 1000000.000000      | 1000000.00 | 1.000000e+06  |
| mean  | 499.218960     | 499.579172          | 2391.95603 | -2.117608e+06 |
| std   | 288.752217     | 288.754757          | 1807.69148 | 6.062849e+05  |
| min   | 0.000000       | 0.000000            | 0.00       | -3.110000e+06 |
| 25%   | 249.000000     | 250.000000          | 954.10500  | -2.581030e+06 |
| 50%   | 499.000000     | 500.000000          | 2021.74000 | -2.217382e+06 |
| 75%   | 749.000000     | 750.000000          | 3449.70500 | -1.757232e+06 |
| max   | 999.000000     | 999.000000          | 15566.1800 | 1.980400e+06  |

### What I achieved in 3 hours

- First I tried to plan how I'd develop that on a piece of paper. I had the big ideas on how to do it but because of a lack of time I decided to develop and think about the conception at the same time (multi-threading work I guess).

1. Read the CSV
2. Processing and analysis
3. Display of the result
4. Translate from a sequential program to a multi-threading program (using omp)
5. Benchmark

### Benchmarks :

<img src="./benchParra.png"></img>
Here's the rsults i found. I think there might be a little problem with the sync somewhere. But I still have a really good accuracy
We can also check the time it took, knowing that in sequential this took +5 minutes

### What I couldn't do

- std : It's there but only physically
- Median : No time to do it
- Quartile : Same as median

### Files : 

main.cpp -> Use of Multi-Threading
main_seq.cpp -> Basic sequential program
