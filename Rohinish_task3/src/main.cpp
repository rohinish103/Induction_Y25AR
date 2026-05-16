#include "SmartBank.hpp"

#include <iostream>

using smartbank::AccountConfig;
using smartbank::AccountType;
using smartbank::ATMCard;
using smartbank::Bank;
using smartbank::CardType;
using smartbank::CustomerProfile;
using smartbank::LoanRejectedException;
using smartbank::LoanType;
using smartbank::NotificationType;
using smartbank::SmartBankException;

namespace {

void printSection(const std::string& title) {
    std::cout << "\n==== " << title << " ====\n";
}

void printNotifications(const Bank& bank) {
    printSection("Notification Log");
    for (const auto& entry : bank.notificationLog()) {
        std::cout << "- " << entry << '\n';
    }
}

}  // namespace

int main() {
    try {
        Bank bank(101, "SmartBank Enterprise System");

        printSection("Bootstrapping Bank");
        auto mainBranch = bank.createBranch(1, "SmartBank Central", "SBIN0001001", "Bengaluru");
        auto techBranch = bank.createBranch(2, "SmartBank Tech Park", "SBIN0001002", "Hyderabad");
        bank.hireEmployee(1001, "Ananya Mehra", "Manager", 95000.0, mainBranch->branchId());
        bank.hireEmployee(1002, "Rohit Shah", "Cashier", 42000.0, techBranch->branchId());
        std::cout << bank.bankSummary();

        printSection("Registering Customers");
        auto customerA = bank.registerCustomer(CustomerProfile {1,
                                                                "Rohinish Verma",
                                                                "2004-03-15",
                                                                "Male",
                                                                "9876543210",
                                                                "rohinish@example.com",
                                                                "Pune",
                                                                "1234-5678-9101",
                                                                "ABCDE1234F"});
        auto customerB = bank.registerCustomer(CustomerProfile {2,
                                                                "Aarohi Sen",
                                                                "2003-09-22",
                                                                "Female",
                                                                "9988776655",
                                                                "aarohi@example.com",
                                                                "Delhi",
                                                                "9876-5432-1001",
                                                                "PQRST6789L"});
        std::cout << customerA->summary() << '\n';
        std::cout << customerB->summary() << '\n';

        printSection("Opening Accounts Through Factory Pattern");
        auto savings = bank.openAccount(1,
                                        1,
                                        AccountConfig {
                                            AccountType::Savings,
                                            25000.0,
                                            4.5,
                                            5000.0,
                                            15000.0,
                                            0.0,
                                            "",
                                            "",
                                            0.0,
                                            0,
                                        });
        auto current = bank.openAccount(2,
                                        2,
                                        AccountConfig {
                                            AccountType::Current,
                                            10000.0,
                                            0.0,
                                            0.0,
                                            0.0,
                                            50000.0,
                                            "Aarohi Ventures",
                                            "",
                                            0.0,
                                            0,
                                        });
        auto fixedDeposit = bank.openAccount(1,
                                             1,
                                             AccountConfig {
                                                 AccountType::FixedDeposit,
                                                 50000.0,
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 0.0,
                                                 "",
                                                 "2027-05-16",
                                                 6.75,
                                                 12,
                                             });
        std::cout << savings->summary() << '\n';
        std::cout << current->summary() << '\n';
        std::cout << fixedDeposit->summary() << '\n';

        printSection("Transactions");
        bank.deposit(savings->accountNumber(), 5000.0);
        bank.withdraw(savings->accountNumber(), 3000.0);
        bank.transfer(savings->accountNumber(), current->accountNumber(), 4000.0);
        std::cout << "Savings after transactions: " << savings->summary() << '\n';
        std::cout << "Current after transfer: " << current->summary() << '\n';

        printSection("Loan Management");
        auto approvedLoan = bank.requestLoan(1, LoanType::Home, 750000.0, 8.2, 15);
        bank.reviewLoan(approvedLoan->loanId(), true);
        std::cout << approvedLoan->summary() << '\n';

        auto rejectedLoan = bank.requestLoan(2, LoanType::Personal, 120000.0, 13.5, 3);
        try {
            bank.reviewLoan(rejectedLoan->loanId(), false);
        } catch (const LoanRejectedException& error) {
            std::cout << "Handled custom exception: " << error.what() << '\n';
        }
        std::cout << rejectedLoan->summary() << '\n';

        printSection("ATM Services");
        auto card = bank.issueATMCard(savings->accountNumber(), 1234, CardType::Debit, "2029-12");
        std::cout << card->summary() << '\n';
        bank.atmWithdraw(card->cardNumber(), 1234, 2000.0);
        std::cout << "Savings after ATM withdrawal: " << savings->summary() << '\n';

        try {
            bank.atmWithdraw(card->cardNumber(), 9999, 100.0);
        } catch (const SmartBankException& error) {
            std::cout << "Expected ATM error: " << error.what() << '\n';
        }

        printSection("Fixed Deposit Maturity");
        try {
            bank.withdraw(fixedDeposit->accountNumber(), 1000.0);
        } catch (const SmartBankException& error) {
            std::cout << "Expected FD restriction: " << error.what() << '\n';
        }
        bank.matureFixedDeposit(fixedDeposit->accountNumber());
        bank.withdraw(fixedDeposit->accountNumber(), 1000.0);
        std::cout << "Fixed deposit after maturity withdrawal: " << fixedDeposit->summary() << '\n';

        printSection("Switching Notification Strategy");
        bank.setNotificationStrategy(NotificationType::Email);
        bank.deposit(current->accountNumber(), 2500.0);
        std::cout << "Active notification channel: " << bank.activeNotificationChannel() << '\n';

        printSection("Reports");
        std::cout << bank.bankSummary();
        std::cout << '\n' << bank.customerReport(customerA->customerId()) << '\n';
        std::cout << '\n' << bank.customerReport(customerB->customerId()) << '\n';

        printNotifications(bank);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Fatal error: " << error.what() << '\n';
        return 1;
    }
}
