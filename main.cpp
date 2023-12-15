#include <iostream>
#include <fstream>
#include <sstream>
#include <omp.h>
#include <chrono>
#include <thread>

#define N_TRANSACTIONS 1000000
#define N_ACCOUNTS 10000

struct Transaction
{
    int sourceAccount;
    int destinationAccount;
    int amount;
    int signature;
};

Transaction transactions[N_TRANSACTIONS];
int accountBalances[N_ACCOUNTS];
int numTransactionsFrauduleuses = 0;
int processedTransactions = 0;
int sumAmounts = 0;
int min = 100000;
int max = 0;
int quartile = 0;
int median = 0;
float stDeviation = 0;

double total_time = omp_get_wtime();

void readCSV(const std::string &filename)
{
    std::ifstream file(filename);
    std::string line;
    int numTransactionsCount = 0;

    std::getline(file, line);

    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::string temp;

        std::getline(ss, temp, ';');
        transactions[numTransactionsCount].sourceAccount = std::stoi(temp);

        std::getline(ss, temp, ';');
        transactions[numTransactionsCount].destinationAccount = std::stoi(temp);

        std::getline(ss, temp, ';');
        transactions[numTransactionsCount].amount = static_cast<int>(std::stod(temp));

        std::getline(ss, temp, ';');
        transactions[numTransactionsCount].signature = std::stoi(temp);

        numTransactionsCount++;
    }
}

bool verifySignature(int sourceAccount, int destinationAccount, int amount, int signature)
{
    double signature_verif = sourceAccount * destinationAccount;
    for (int j = 0; j < 20000; j++)
    {
        if (j % 2 == 0)
        {
            signature_verif -= j / 32;
            signature_verif += 1;
        }
        else
        {
            signature_verif += amount / 32;
        }
    }
    if (signature == signature_verif)
    {
        return true;
    }
    else
    {
#pragma omp atomic
        numTransactionsFrauduleuses++;
        return false;
    }
}

void processTransactions()
{
    double start_time = omp_get_wtime();
    int local_processedTransactions = 0;
    int local_sumAmounts = 0;
    int local_min = 100000;
    int local_max = 0;
    double local_averageAmount = 0.0;
    float local_stDeviation = 0.0;

#pragma omp parallel for reduction(+ : local_processedTransactions, local_sumAmounts)
    for (int i = 0; i < N_TRANSACTIONS; ++i)
    {
        if (verifySignature(transactions[i].sourceAccount, transactions[i].destinationAccount, transactions[i].amount, transactions[i].signature))
        {
#pragma omp atomic
            accountBalances[transactions[i].sourceAccount] -= transactions[i].amount;

#pragma omp atomic
            accountBalances[transactions[i].destinationAccount] += transactions[i].amount;

            local_sumAmounts += transactions[i].amount;
            local_min = std::min(local_min, transactions[i].amount);
            local_max = std::max(local_max, transactions[i].amount);
        }
        local_processedTransactions++;

        double current_time = omp_get_wtime();
        if (current_time - start_time >= 1.0)
        {
#pragma omp critical
            {
                processedTransactions += local_processedTransactions;
                sumAmounts += local_sumAmounts;
                min = std::min(min, local_min);
                max = std::max(max, local_max);

                local_averageAmount = static_cast<double>(sumAmounts) / processedTransactions;
                double lastAverageAmount = static_cast<double>(local_sumAmounts) / local_processedTransactions;
                double percentageFraud = static_cast<double>(numTransactionsFrauduleuses) / processedTransactions * 100;
                local_stDeviation = sqrt(lastAverageAmount - local_averageAmount) / processedTransactions;

                std::time_t t = std::time(nullptr);
                char mbstr[100];
                std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

                std::cout << "| Dernière Actualisation                                      |" << std::endl;
                std::cout << "--------------------------------------------------------------" << std::endl;
                std::cout << "| Nombre de transactions traitées sur dernière actualisation: " << local_processedTransactions << std::endl;
                std::cout << "| Moyenne du montant des transactions sur la dernière actualisation: " << lastAverageAmount << "€" << std::endl;
                std::cout << "| Montant minimum des transactions sur la dernière actualisation: " << local_min << "€" << std::endl;
                std::cout << "| Montant maximum des transactions sur la dernière actualisation: " << local_max << "€" << std::endl;
                std::cout << "| Ecart-type du montant des transactions sur la dernière actualisation: " << local_stDeviation << "€" << std::endl;
                std::cout << std::endl;

                std::cout << "| Totaux                                                      |" << std::endl;
                std::cout << "--------------------------------------------------------------" << std::endl;
                std::cout << "| Nombre de transactions traitées: " << processedTransactions << std::endl;
                std::cout << "| Moyenne totale du montant des transactions: " << local_averageAmount << "€" << std::endl;
                std::cout << "| Nombre de transactions frauduleuses: " << numTransactionsFrauduleuses << std::endl;
                std::cout << "| Pourcentage de transactions frauduleuses: " << percentageFraud << "%" << std::endl;
                std::cout << "| Montant minimum des transactions: " << min << "€" << std::endl;
                std::cout << "| Montant maximum des transactions: " << max << "€" << std::endl;
                std::cout << std::endl;
            }

            start_time = current_time;
            local_processedTransactions = 0;
            local_sumAmounts = 0;
            local_min = 100000;
            local_max = 0;
        }
    }
}

int main()
{
    for (int i = 0; i < N_ACCOUNTS; ++i)
    {
        accountBalances[i] = 1000000;
    }

    readCSV("transactions.csv");
    processTransactions();

    std::cout << "Temps total: " << omp_get_wtime() - total_time << "s" << std::endl;

    return 0;
}
