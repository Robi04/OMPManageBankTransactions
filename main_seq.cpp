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
    bool transac_valide = false;
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
    int lastProcessedTransactions = 0;
    int lastSumAmounts = 0;
    int lastMin = 10000;
    int lastMax = 0;
    int lastQuartile = 0;
    int lastMedian = 0;
    double averageAmount = 0;
    float lastStDeviation = 0;

    for (int i = 0; i < N_TRANSACTIONS; ++i)
    {
        if (verifySignature(transactions[i].sourceAccount, transactions[i].destinationAccount, transactions[i].amount, transactions[i].signature))
        {
#pragma omp atomic
            accountBalances[transactions[i].sourceAccount] -= transactions[i].amount;

#pragma omp atomic
            accountBalances[transactions[i].destinationAccount] += transactions[i].amount;

#pragma omp atomic
            lastSumAmounts += transactions[i].amount;
        }
        else
        {
            continue;
        }
        if (transactions[i].amount < lastMin)
        {
            lastMin = transactions[i].amount;
        }
        if (transactions[i].amount > lastMax)
        {
            lastMax = transactions[i].amount;
        }

        lastProcessedTransactions++;
        double current_time = omp_get_wtime();
        if (current_time - start_time >= 1.0)
        {
            processedTransactions += lastProcessedTransactions;
            sumAmounts += lastSumAmounts;

            if (lastMin < min)
            {
                min = lastMin;
            }
            if (lastMax > max)
            {
                max = lastMax;
            }

            // Calculer les statistiques
            averageAmount = static_cast<double>(sumAmounts) / processedTransactions;
            double lastAverageAmount = static_cast<double>(lastSumAmounts) / lastProcessedTransactions;
            double percentageFraud = static_cast<double>(numTransactionsFrauduleuses) / i * 100;
            lastStDeviation = sqrt(lastAverageAmount - averageAmount) / processedTransactions;

            // Afficher les statistiques
            std::time_t t = std::time(nullptr);
            char mbstr[100];
            std::strftime(mbstr, sizeof(mbstr), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

            std::cout << "| Dernière Actualisation                                      |" << std::endl;
            std::cout << "--------------------------------------------------------------" << std::endl;
            std::cout << "| Nombre de transactions traitées sur dernière actualisation: " << lastProcessedTransactions << std::endl;
            std::cout << "| Moyenne du montant des transactions sur la dernière actualisation: " << lastAverageAmount << "€" << std::endl;
            std::cout << "| Montant minimum des transactions sur la dernière actualisation: " << lastMin << "€" << std::endl;
            std::cout << "| Montant maximum des transactions sur la dernière actualisation: " << lastMax << "€" << std::endl;
            std::cout << "| Ecart-type du montant des transactions sur la dernière actualisation: " << lastStDeviation << "€" << std::endl;
            std::cout << std::endl;

            std::cout << "| Totaux                                                      |" << std::endl;
            std::cout << "--------------------------------------------------------------" << std::endl;
            std::cout << "| Nombre de transactions traitées: " << processedTransactions << std::endl;
            std::cout << "| Moyenne totale du montant des transactions: " << averageAmount << "€" << std::endl;
            std::cout << "| Nombre de transactions frauduleuses: " << numTransactionsFrauduleuses << std::endl;
            std::cout << "| Pourcentage de transactions frauduleuses: " << percentageFraud << "%" << std::endl;
            std::cout << "| Montant minimum des transactions: " << min << "€" << std::endl;
            std::cout << "| Montant maximum des transactions: " << max << "€" << std::endl;

            std::cout << std::endl;
            std::cout << std::endl;
            std::cout << std::endl;

            start_time = current_time;
            lastSumAmounts = sumAmounts;
            sumAmounts = 0;
            lastProcessedTransactions = 0;
            processedTransactions++;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

int main()
{
    for (int i = 0; i < N_ACCOUNTS; ++i)
    {
        accountBalances[i] = 1000000;
    }

    // Lire les transactions
    readCSV("transactions.csv");

    // Traiter les transactions
    processTransactions();

    return 0;
}