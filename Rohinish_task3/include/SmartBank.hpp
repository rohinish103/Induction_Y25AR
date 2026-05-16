#ifndef SMARTBANK_HPP
#define SMARTBANK_HPP

#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace smartbank {

class Branch;
class Customer;
class Employee;
class Loan;
class Account;
class ATMCard;

enum class AccountStatus { Active, Blocked, Closed };
enum class CardStatus { Active, Blocked, Expired };
enum class LoanStatus { Pending, Approved, Rejected };
enum class TransactionStatus { Success, Pending, Failed };
enum class AccountType { Savings, Current, FixedDeposit };
enum class CardType { Debit, Credit };
enum class NotificationType { SMS, Email };
enum class LoanType { Home, Car, Personal };

std::string toString(AccountStatus status);
std::string toString(CardStatus status);
std::string toString(LoanStatus status);
std::string toString(TransactionStatus status);
std::string toString(AccountType type);
std::string toString(CardType type);
std::string toString(NotificationType type);
std::string toString(LoanType type);
std::string currentTimestamp();

class SmartBankException : public std::runtime_error {
public:
    explicit SmartBankException(const std::string& message);
};

class InsufficientBalanceException : public SmartBankException {
public:
    explicit InsufficientBalanceException(const std::string& message);
};

class InvalidPINException : public SmartBankException {
public:
    explicit InvalidPINException(const std::string& message);
};

class AccountBlockedException : public SmartBankException {
public:
    explicit AccountBlockedException(const std::string& message);
};

class LoanRejectedException : public SmartBankException {
public:
    explicit LoanRejectedException(const std::string& message);
};

struct Transaction {
    int transactionId {};
    std::string transactionType;
    double amount {};
    std::string transactionDate;
    long long senderAccount {};
    std::optional<long long> receiverAccount;
    TransactionStatus status {TransactionStatus::Pending};
    std::string notes;
};

struct CustomerProfile {
    int customerId {};
    std::string fullName;
    std::string dob;
    std::string gender;
    std::string mobileNumber;
    std::string email;
    std::string address;
    std::string aadhaarNumber;
    std::string panNumber;
};

struct AccountConfig {
    AccountType type {AccountType::Savings};
    double initialDeposit {};
    double interestRate {};
    double minimumBalance {};
    double dailyWithdrawalLimit {};
    double overdraftLimit {};
    std::string businessName;
    std::string maturityDate;
    double fdInterestRate {};
    int tenureMonths {};
};

class Notification {
public:
    virtual ~Notification() = default;
    virtual std::string send(const Customer& customer,
                             const std::string& subject,
                             const std::string& message) = 0;
    virtual std::string channelName() const = 0;
};

class SMSNotification : public Notification {
public:
    std::string send(const Customer& customer,
                     const std::string& subject,
                     const std::string& message) override;
    std::string channelName() const override;
};

class EmailNotification : public Notification {
public:
    std::string send(const Customer& customer,
                     const std::string& subject,
                     const std::string& message) override;
    std::string channelName() const override;
};

class Employee {
public:
    Employee(int employeeId,
             std::string employeeName,
             std::string designation,
             double salary,
             std::shared_ptr<Branch> branch);

    int employeeId() const;
    const std::string& employeeName() const;
    const std::string& designation() const;
    double salary() const;
    std::shared_ptr<Branch> branch() const;
    std::string summary() const;

private:
    int employeeId_ {};
    std::string employeeName_;
    std::string designation_;
    double salary_ {};
    std::weak_ptr<Branch> branch_;
};

class Branch {
public:
    Branch(int branchId, std::string branchName, std::string ifscCode, std::string address);

    int branchId() const;
    const std::string& branchName() const;
    const std::string& ifscCode() const;
    const std::string& address() const;

    void addAccount(const std::shared_ptr<Account>& account);
    void addEmployee(const std::shared_ptr<Employee>& employee);

    const std::vector<std::shared_ptr<Account>>& accounts() const;
    const std::vector<std::shared_ptr<Employee>>& employees() const;
    std::string summary() const;

private:
    int branchId_ {};
    std::string branchName_;
    std::string ifscCode_;
    std::string address_;
    std::vector<std::shared_ptr<Account>> accounts_;
    std::vector<std::shared_ptr<Employee>> employees_;
};

class Customer {
public:
    explicit Customer(CustomerProfile profile);

    int customerId() const;
    const std::string& fullName() const;
    const std::string& dob() const;
    const std::string& gender() const;
    const std::string& mobileNumber() const;
    const std::string& email() const;
    const std::string& address() const;
    const std::string& aadhaarNumber() const;
    const std::string& panNumber() const;

    void addAccount(const std::shared_ptr<Account>& account);
    void addLoan(const std::shared_ptr<Loan>& loan);
    void addCard(const std::shared_ptr<ATMCard>& card);

    const std::vector<std::shared_ptr<Account>>& accounts() const;
    const std::vector<std::shared_ptr<Loan>>& loans() const;
    const std::vector<std::shared_ptr<ATMCard>>& cards() const;
    std::string summary() const;

private:
    CustomerProfile profile_;
    std::vector<std::shared_ptr<Account>> accounts_;
    std::vector<std::shared_ptr<Loan>> loans_;
    std::vector<std::shared_ptr<ATMCard>> cards_;
};

class Loan {
public:
    Loan(int loanId,
         LoanType loanType,
         double loanAmount,
         double interestRate,
         int tenureYears,
         std::shared_ptr<Customer> customer);

    int loanId() const;
    LoanType loanType() const;
    double loanAmount() const;
    double interestRate() const;
    int tenureYears() const;
    double emiAmount() const;
    LoanStatus status() const;
    std::shared_ptr<Customer> customer() const;

    void approve();
    void reject();
    std::string summary() const;

private:
    double calculateEmi() const;

    int loanId_ {};
    LoanType loanType_ {LoanType::Personal};
    double loanAmount_ {};
    double interestRate_ {};
    int tenureYears_ {};
    double emiAmount_ {};
    LoanStatus status_ {LoanStatus::Pending};
    std::weak_ptr<Customer> customer_;
};

class Account {
public:
    Account(long long accountNumber,
            AccountType accountType,
            double balance,
            std::string dateOpened,
            std::shared_ptr<Branch> branch,
            std::shared_ptr<Customer> customer);
    virtual ~Account() = default;

    long long accountNumber() const;
    AccountType accountType() const;
    double balance() const;
    const std::string& dateOpened() const;
    AccountStatus status() const;
    std::shared_ptr<Branch> branch() const;
    std::shared_ptr<Customer> customer() const;
    const std::vector<Transaction>& transactions() const;

    virtual void deposit(double amount);
    virtual void withdraw(double amount);
    virtual double availableFunds() const = 0;
    virtual void applyMonthEnd() = 0;
    virtual std::string productDetails() const = 0;

    void recordTransaction(const Transaction& transaction);
    void block();
    void unblock();
    std::string summary() const;

protected:
    void ensureActive() const;
    void setBalance(double newBalance);

private:
    long long accountNumber_ {};
    AccountType accountType_ {AccountType::Savings};
    double balance_ {};
    std::string dateOpened_;
    AccountStatus status_ {AccountStatus::Active};
    std::weak_ptr<Branch> branch_;
    std::weak_ptr<Customer> customer_;
    std::vector<Transaction> transactions_;
};

class SavingsAccount : public Account {
public:
    SavingsAccount(long long accountNumber,
                   double initialDeposit,
                   std::string dateOpened,
                   std::shared_ptr<Branch> branch,
                   std::shared_ptr<Customer> customer,
                   double interestRate,
                   double minimumBalance,
                   double dailyWithdrawalLimit);

    void withdraw(double amount) override;
    double availableFunds() const override;
    void applyMonthEnd() override;
    std::string productDetails() const override;

private:
    double interestRate_ {};
    double minimumBalance_ {};
    double dailyWithdrawalLimit_ {};
    double withdrawnToday_ {};
};

class CurrentAccount : public Account {
public:
    CurrentAccount(long long accountNumber,
                   double initialDeposit,
                   std::string dateOpened,
                   std::shared_ptr<Branch> branch,
                   std::shared_ptr<Customer> customer,
                   double overdraftLimit,
                   std::string businessName);

    void withdraw(double amount) override;
    double availableFunds() const override;
    void applyMonthEnd() override;
    std::string productDetails() const override;

private:
    double overdraftLimit_ {};
    std::string businessName_;
};

class FixedDepositAccount : public Account {
public:
    FixedDepositAccount(long long accountNumber,
                        double initialDeposit,
                        std::string dateOpened,
                        std::shared_ptr<Branch> branch,
                        std::shared_ptr<Customer> customer,
                        double fdInterestRate,
                        int tenureMonths,
                        std::string maturityDate);

    void deposit(double amount) override;
    void withdraw(double amount) override;
    double availableFunds() const override;
    void applyMonthEnd() override;
    void mature();
    std::string productDetails() const override;

private:
    double fdAmount_ {};
    std::string maturityDate_;
    double fdInterestRate_ {};
    int tenureMonths_ {};
    bool matured_ {false};
};

class ATMCard {
public:
    ATMCard(long long cardNumber,
            int cvv,
            std::string expiryDate,
            int pin,
            CardType cardType,
            std::shared_ptr<Account> linkedAccount);

    long long cardNumber() const;
    int cvv() const;
    const std::string& expiryDate() const;
    CardType cardType() const;
    CardStatus cardStatus() const;
    std::shared_ptr<Account> linkedAccount() const;

    void authenticate(int pin);
    void withdrawCash(int pin, double amount);
    void changePin(int currentPin, int newPin);
    void block();
    std::string summary() const;

private:
    long long cardNumber_ {};
    int cvv_ {};
    std::string expiryDate_;
    int pin_ {};
    CardType cardType_ {CardType::Debit};
    CardStatus cardStatus_ {CardStatus::Active};
    std::weak_ptr<Account> linkedAccount_;
    int failedAttempts_ {};
};

class AccountFactory {
public:
    static std::shared_ptr<Account> createAccount(long long accountNumber,
                                                  const AccountConfig& config,
                                                  const std::string& dateOpened,
                                                  const std::shared_ptr<Branch>& branch,
                                                  const std::shared_ptr<Customer>& customer);
};

class Bank {
public:
    Bank(int bankId, std::string bankName);

    int bankId() const;
    const std::string& bankName() const;

    std::shared_ptr<Branch> createBranch(int branchId,
                                         const std::string& branchName,
                                         const std::string& ifscCode,
                                         const std::string& address);
    std::shared_ptr<Customer> registerCustomer(const CustomerProfile& profile);
    std::shared_ptr<Employee> hireEmployee(int employeeId,
                                           const std::string& employeeName,
                                           const std::string& designation,
                                           double salary,
                                           int branchId);
    std::shared_ptr<Account> openAccount(int customerId,
                                         int branchId,
                                         const AccountConfig& config,
                                         const std::string& dateOpened = currentTimestamp());
    std::shared_ptr<Loan> requestLoan(int customerId,
                                      LoanType loanType,
                                      double loanAmount,
                                      double interestRate,
                                      int tenureYears);
    void reviewLoan(int loanId, bool approve);
    std::shared_ptr<ATMCard> issueATMCard(long long accountNumber,
                                          int pin,
                                          CardType cardType,
                                          const std::string& expiryDate);

    void deposit(long long accountNumber, double amount);
    void withdraw(long long accountNumber, double amount);
    void transfer(long long senderAccountNumber, long long receiverAccountNumber, double amount);
    void atmWithdraw(long long cardNumber, int pin, double amount);
    void matureFixedDeposit(long long accountNumber);

    void setNotificationStrategy(NotificationType type);
    std::string activeNotificationChannel() const;

    std::shared_ptr<Branch> findBranch(int branchId) const;
    std::shared_ptr<Customer> findCustomer(int customerId) const;
    std::shared_ptr<Employee> findEmployee(int employeeId) const;
    std::shared_ptr<Account> findAccount(long long accountNumber) const;
    std::shared_ptr<Loan> findLoan(int loanId) const;
    std::shared_ptr<ATMCard> findCard(long long cardNumber) const;

    const std::vector<std::shared_ptr<Branch>>& branches() const;
    const std::vector<std::shared_ptr<Customer>>& customers() const;
    const std::vector<std::shared_ptr<Employee>>& employees() const;
    const std::vector<std::string>& notificationLog() const;

    std::string bankSummary() const;
    std::string customerReport(int customerId) const;

private:
    void notifyCustomer(const std::shared_ptr<Customer>& customer,
                        const std::string& subject,
                        const std::string& message);
    int nextTransactionId();
    int nextLoanId();
    long long nextAccountNumber();
    long long nextCardNumber();

    int bankId_ {};
    std::string bankName_;
    std::vector<std::shared_ptr<Branch>> branches_;
    std::vector<std::shared_ptr<Customer>> customers_;
    std::vector<std::shared_ptr<Employee>> employees_;
    std::vector<std::string> notificationLog_;

    std::unordered_map<int, std::shared_ptr<Branch>> branchIndex_;
    std::unordered_map<int, std::shared_ptr<Customer>> customerIndex_;
    std::unordered_map<int, std::shared_ptr<Employee>> employeeIndex_;
    std::unordered_map<long long, std::shared_ptr<Account>> accountIndex_;
    std::unordered_map<int, std::shared_ptr<Loan>> loanIndex_;
    std::unordered_map<long long, std::shared_ptr<ATMCard>> cardIndex_;

    std::unique_ptr<Notification> notifier_;
    NotificationType notificationType_ {NotificationType::SMS};
    int transactionCounter_ {1000};
    int loanCounter_ {500};
    long long accountCounter_ {1000000000LL};
    long long cardCounter_ {4000000000000000LL};
};

}  // namespace smartbank

#endif
