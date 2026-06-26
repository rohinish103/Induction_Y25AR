#include "SmartBank.hpp"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <utility>

namespace smartbank {

namespace {

std::string formatCurrency(double amount) {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(2) << amount;
    return stream.str();
}

}  // namespace

std::string toString(AccountStatus status) {
    switch (status) {
        case AccountStatus::Active:
            return "Active";
        case AccountStatus::Blocked:
            return "Blocked";
        case AccountStatus::Closed:
            return "Closed";
    }
    return "Unknown";
}

std::string toString(CardStatus status) {
    switch (status) {
        case CardStatus::Active:
            return "Active";
        case CardStatus::Blocked:
            return "Blocked";
        case CardStatus::Expired:
            return "Expired";
    }
    return "Unknown";
}

std::string toString(LoanStatus status) {
    switch (status) {
        case LoanStatus::Pending:
            return "Pending";
        case LoanStatus::Approved:
            return "Approved";
        case LoanStatus::Rejected:
            return "Rejected";
    }
    return "Unknown";
}

std::string toString(TransactionStatus status) {
    switch (status) {
        case TransactionStatus::Success:
            return "Success";
        case TransactionStatus::Pending:
            return "Pending";
        case TransactionStatus::Failed:
            return "Failed";
    }
    return "Unknown";
}

std::string toString(AccountType type) {
    switch (type) {
        case AccountType::Savings:
            return "Savings";
        case AccountType::Current:
            return "Current";
        case AccountType::FixedDeposit:
            return "FixedDeposit";
    }
    return "Unknown";
}

std::string toString(CardType type) {
    switch (type) {
        case CardType::Debit:
            return "Debit";
        case CardType::Credit:
            return "Credit";
    }
    return "Unknown";
}

std::string toString(NotificationType type) {
    switch (type) {
        case NotificationType::SMS:
            return "SMS";
        case NotificationType::Email:
            return "Email";
    }
    return "Unknown";
}

std::string toString(LoanType type) {
    switch (type) {
        case LoanType::Home:
            return "Home";
        case LoanType::Car:
            return "Car";
        case LoanType::Personal:
            return "Personal";
    }
    return "Unknown";
}

std::string currentTimestamp() {
    const auto now = std::time(nullptr);
    std::tm localTime {};
#if defined(_WIN32)
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

SmartBankException::SmartBankException(const std::string& message) : std::runtime_error(message) {}

InsufficientBalanceException::InsufficientBalanceException(const std::string& message)
    : SmartBankException(message) {}

InvalidPINException::InvalidPINException(const std::string& message) : SmartBankException(message) {}

AccountBlockedException::AccountBlockedException(const std::string& message)
    : SmartBankException(message) {}

LoanRejectedException::LoanRejectedException(const std::string& message) : SmartBankException(message) {}

std::string SMSNotification::send(const Customer& customer,
                                  const std::string& subject,
                                  const std::string& message) {
    (void)subject;
    return "[SMS] to " + customer.mobileNumber() + " | " + customer.fullName() + " | " + message;
}

std::string SMSNotification::channelName() const {
    return "SMS";
}

std::string EmailNotification::send(const Customer& customer,
                                    const std::string& subject,
                                    const std::string& message) {
    return "[EMAIL] to " + customer.email() + " | subject=" + subject + " | " + message;
}

std::string EmailNotification::channelName() const {
    return "Email";
}

Employee::Employee(int employeeId,
                   std::string employeeName,
                   std::string designation,
                   double salary,
                   std::shared_ptr<Branch> branch)
    : employeeId_(employeeId),
      employeeName_(std::move(employeeName)),
      designation_(std::move(designation)),
      salary_(salary),
      branch_(branch) {}

int Employee::employeeId() const {
    return employeeId_;
}

const std::string& Employee::employeeName() const {
    return employeeName_;
}

const std::string& Employee::designation() const {
    return designation_;
}

double Employee::salary() const {
    return salary_;
}

std::shared_ptr<Branch> Employee::branch() const {
    return branch_.lock();
}

std::string Employee::summary() const {
    std::ostringstream stream;
    stream << employeeName_ << " (" << designation_ << ", salary=" << formatCurrency(salary_) << ")";
    return stream.str();
}

Branch::Branch(int branchId, std::string branchName, std::string ifscCode, std::string address)
    : branchId_(branchId),
      branchName_(std::move(branchName)),
      ifscCode_(std::move(ifscCode)),
      address_(std::move(address)) {}

int Branch::branchId() const {
    return branchId_;
}

const std::string& Branch::branchName() const {
    return branchName_;
}

const std::string& Branch::ifscCode() const {
    return ifscCode_;
}

const std::string& Branch::address() const {
    return address_;
}

void Branch::addAccount(const std::shared_ptr<Account>& account) {
    accounts_.push_back(account);
}

void Branch::addEmployee(const std::shared_ptr<Employee>& employee) {
    employees_.push_back(employee);
}

const std::vector<std::shared_ptr<Account>>& Branch::accounts() const {
    return accounts_;
}

const std::vector<std::shared_ptr<Employee>>& Branch::employees() const {
    return employees_;
}

std::string Branch::summary() const {
    std::ostringstream stream;
    stream << branchName_ << " [" << ifscCode_ << "] accounts=" << accounts_.size()
           << " employees=" << employees_.size();
    return stream.str();
}

Customer::Customer(CustomerProfile profile) : profile_(std::move(profile)) {}

int Customer::customerId() const {
    return profile_.customerId;
}

const std::string& Customer::fullName() const {
    return profile_.fullName;
}

const std::string& Customer::dob() const {
    return profile_.dob;
}

const std::string& Customer::gender() const {
    return profile_.gender;
}

const std::string& Customer::mobileNumber() const {
    return profile_.mobileNumber;
}

const std::string& Customer::email() const {
    return profile_.email;
}

const std::string& Customer::address() const {
    return profile_.address;
}

const std::string& Customer::aadhaarNumber() const {
    return profile_.aadhaarNumber;
}

const std::string& Customer::panNumber() const {
    return profile_.panNumber;
}

void Customer::addAccount(const std::shared_ptr<Account>& account) {
    accounts_.push_back(account);
}

void Customer::addLoan(const std::shared_ptr<Loan>& loan) {
    loans_.push_back(loan);
}

void Customer::addCard(const std::shared_ptr<ATMCard>& card) {
    cards_.push_back(card);
}

const std::vector<std::shared_ptr<Account>>& Customer::accounts() const {
    return accounts_;
}

const std::vector<std::shared_ptr<Loan>>& Customer::loans() const {
    return loans_;
}

const std::vector<std::shared_ptr<ATMCard>>& Customer::cards() const {
    return cards_;
}

std::string Customer::summary() const {
    std::ostringstream stream;
    stream << fullName() << " (#" << customerId() << ") accounts=" << accounts_.size()
           << " loans=" << loans_.size() << " cards=" << cards_.size();
    return stream.str();
}

Loan::Loan(int loanId,
           LoanType loanType,
           double loanAmount,
           double interestRate,
           int tenureYears,
           std::shared_ptr<Customer> customer)
    : loanId_(loanId),
      loanType_(loanType),
      loanAmount_(loanAmount),
      interestRate_(interestRate),
      tenureYears_(tenureYears),
      emiAmount_(calculateEmi()),
      customer_(customer) {}

int Loan::loanId() const {
    return loanId_;
}

LoanType Loan::loanType() const {
    return loanType_;
}

double Loan::loanAmount() const {
    return loanAmount_;
}

double Loan::interestRate() const {
    return interestRate_;
}

int Loan::tenureYears() const {
    return tenureYears_;
}

double Loan::emiAmount() const {
    return emiAmount_;
}

LoanStatus Loan::status() const {
    return status_;
}

std::shared_ptr<Customer> Loan::customer() const {
    return customer_.lock();
}

void Loan::approve() {
    status_ = LoanStatus::Approved;
}

void Loan::reject() {
    status_ = LoanStatus::Rejected;
}

double Loan::calculateEmi() const {
    const double monthlyRate = interestRate_ / (12.0 * 100.0);
    const double months = static_cast<double>(tenureYears_ * 12);
    if (months <= 0) {
        return 0.0;
    }
    if (monthlyRate == 0.0) {
        return loanAmount_ / months;
    }
    const double factor = std::pow(1.0 + monthlyRate, months);
    return loanAmount_ * monthlyRate * factor / (factor - 1.0);
}

std::string Loan::summary() const {
    std::ostringstream stream;
    stream << "Loan#" << loanId_ << " type=" << toString(loanType_) << " amount=" << formatCurrency(loanAmount_)
           << " EMI=" << formatCurrency(emiAmount_) << " status=" << toString(status_);
    return stream.str();
}

Account::Account(long long accountNumber,
                 AccountType accountType,
                 double balance,
                 std::string dateOpened,
                 std::shared_ptr<Branch> branch,
                 std::shared_ptr<Customer> customer)
    : accountNumber_(accountNumber),
      accountType_(accountType),
      balance_(balance),
      dateOpened_(std::move(dateOpened)),
      branch_(branch),
      customer_(customer) {}

long long Account::accountNumber() const {
    return accountNumber_;
}

AccountType Account::accountType() const {
    return accountType_;
}

double Account::balance() const {
    return balance_;
}

const std::string& Account::dateOpened() const {
    return dateOpened_;
}

AccountStatus Account::status() const {
    return status_;
}

std::shared_ptr<Branch> Account::branch() const {
    return branch_.lock();
}

std::shared_ptr<Customer> Account::customer() const {
    return customer_.lock();
}

const std::vector<Transaction>& Account::transactions() const {
    return transactions_;
}

void Account::deposit(double amount) {
    ensureActive();
    if (amount <= 0.0) {
        throw std::invalid_argument("Deposit amount must be positive.");
    }
    balance_ += amount;
}

void Account::withdraw(double amount) {
    ensureActive();
    if (amount <= 0.0) {
        throw std::invalid_argument("Withdrawal amount must be positive.");
    }
    if (amount > availableFunds()) {
        throw InsufficientBalanceException("Insufficient balance for withdrawal.");
    }
    balance_ -= amount;
}

void Account::recordTransaction(const Transaction& transaction) {
    transactions_.push_back(transaction);
}

void Account::block() {
    status_ = AccountStatus::Blocked;
}

void Account::unblock() {
    status_ = AccountStatus::Active;
}

std::string Account::summary() const {
    std::ostringstream stream;
    stream << "Account#" << accountNumber_ << " type=" << toString(accountType_) << " balance="
           << formatCurrency(balance_) << " status=" << toString(status_) << " | " << productDetails();
    return stream.str();
}

void Account::ensureActive() const {
    if (status_ != AccountStatus::Active) {
        throw AccountBlockedException("Account " + std::to_string(accountNumber_) + " is not active.");
    }
}

void Account::setBalance(double newBalance) {
    balance_ = newBalance;
}

SavingsAccount::SavingsAccount(long long accountNumber,
                               double initialDeposit,
                               std::string dateOpened,
                               std::shared_ptr<Branch> branch,
                               std::shared_ptr<Customer> customer,
                               double interestRate,
                               double minimumBalance,
                               double dailyWithdrawalLimit)
    : Account(accountNumber,
              AccountType::Savings,
              initialDeposit,
              std::move(dateOpened),
              std::move(branch),
              std::move(customer)),
      interestRate_(interestRate),
      minimumBalance_(minimumBalance),
      dailyWithdrawalLimit_(dailyWithdrawalLimit) {}

void SavingsAccount::withdraw(double amount) {
    ensureActive();
    if (amount <= 0.0) {
        throw std::invalid_argument("Withdrawal amount must be positive.");
    }
    if (withdrawnToday_ + amount > dailyWithdrawalLimit_) {
        throw InsufficientBalanceException("Daily withdrawal limit exceeded.");
    }
    if (balance() - amount < minimumBalance_) {
        throw InsufficientBalanceException("Withdrawal would breach the minimum balance.");
    }
    if (amount > availableFunds()) {
        throw InsufficientBalanceException("Insufficient balance for savings account.");
    }
    setBalance(balance() - amount);
    withdrawnToday_ += amount;
}

double SavingsAccount::availableFunds() const {
    return std::max(0.0, balance() - minimumBalance_);
}

void SavingsAccount::applyMonthEnd() {
    const double monthlyInterest = balance() * (interestRate_ / 100.0) / 12.0;
    setBalance(balance() + monthlyInterest);
    withdrawnToday_ = 0.0;
}

std::string SavingsAccount::productDetails() const {
    std::ostringstream stream;
    stream << "interest=" << interestRate_ << "% minBalance=" << formatCurrency(minimumBalance_)
           << " dailyLimit=" << formatCurrency(dailyWithdrawalLimit_);
    return stream.str();
}

CurrentAccount::CurrentAccount(long long accountNumber,
                               double initialDeposit,
                               std::string dateOpened,
                               std::shared_ptr<Branch> branch,
                               std::shared_ptr<Customer> customer,
                               double overdraftLimit,
                               std::string businessName)
    : Account(accountNumber,
              AccountType::Current,
              initialDeposit,
              std::move(dateOpened),
              std::move(branch),
              std::move(customer)),
      overdraftLimit_(overdraftLimit),
      businessName_(std::move(businessName)) {}

void CurrentAccount::withdraw(double amount) {
    ensureActive();
    if (amount <= 0.0) {
        throw std::invalid_argument("Withdrawal amount must be positive.");
    }
    if (amount > availableFunds()) {
        throw InsufficientBalanceException("Overdraft limit exceeded.");
    }
    setBalance(balance() - amount);
}

double CurrentAccount::availableFunds() const {
    return balance() + overdraftLimit_;
}

void CurrentAccount::applyMonthEnd() {
    if (balance() < 0.0) {
        setBalance(balance() - 50.0);
    }
}

std::string CurrentAccount::productDetails() const {
    std::ostringstream stream;
    stream << "business=" << businessName_ << " overdraft=" << formatCurrency(overdraftLimit_);
    return stream.str();
}

FixedDepositAccount::FixedDepositAccount(long long accountNumber,
                                         double initialDeposit,
                                         std::string dateOpened,
                                         std::shared_ptr<Branch> branch,
                                         std::shared_ptr<Customer> customer,
                                         double fdInterestRate,
                                         int tenureMonths,
                                         std::string maturityDate)
    : Account(accountNumber,
              AccountType::FixedDeposit,
              initialDeposit,
              std::move(dateOpened),
              std::move(branch),
              std::move(customer)),
      fdAmount_(initialDeposit),
      maturityDate_(std::move(maturityDate)),
      fdInterestRate_(fdInterestRate),
      tenureMonths_(tenureMonths) {}

void FixedDepositAccount::deposit(double amount) {
    ensureActive();
    if (amount <= 0.0) {
        throw std::invalid_argument("Deposit amount must be positive.");
    }
    throw SmartBankException("Additional deposits are not allowed for a fixed deposit account.");
}

void FixedDepositAccount::withdraw(double amount) {
    ensureActive();
    (void)amount;
    if (!matured_) {
        throw InsufficientBalanceException("Fixed deposit cannot be withdrawn before maturity.");
    }
    Account::withdraw(amount);
}

double FixedDepositAccount::availableFunds() const {
    return matured_ ? balance() : 0.0;
}

void FixedDepositAccount::applyMonthEnd() {
    const double monthlyInterest = balance() * (fdInterestRate_ / 100.0) / 12.0;
    setBalance(balance() + monthlyInterest);
}

void FixedDepositAccount::mature() {
    matured_ = true;
}

std::string FixedDepositAccount::productDetails() const {
    std::ostringstream stream;
    stream << "fdAmount=" << formatCurrency(fdAmount_) << " interest=" << fdInterestRate_
           << "% tenureMonths=" << tenureMonths_ << " maturityDate=" << maturityDate_
           << " matured=" << (matured_ ? "yes" : "no");
    return stream.str();
}

ATMCard::ATMCard(long long cardNumber,
                 int cvv,
                 std::string expiryDate,
                 int pin,
                 CardType cardType,
                 std::shared_ptr<Account> linkedAccount)
    : cardNumber_(cardNumber),
      cvv_(cvv),
      expiryDate_(std::move(expiryDate)),
      pin_(pin),
      cardType_(cardType),
      linkedAccount_(linkedAccount) {}

long long ATMCard::cardNumber() const {
    return cardNumber_;
}

int ATMCard::cvv() const {
    return cvv_;
}

const std::string& ATMCard::expiryDate() const {
    return expiryDate_;
}

CardType ATMCard::cardType() const {
    return cardType_;
}

CardStatus ATMCard::cardStatus() const {
    return cardStatus_;
}

std::shared_ptr<Account> ATMCard::linkedAccount() const {
    return linkedAccount_.lock();
}

void ATMCard::authenticate(int pin) {
    if (cardStatus_ != CardStatus::Active) {
        throw AccountBlockedException("ATM card is not active.");
    }
    if (pin != pin_) {
        ++failedAttempts_;
        if (failedAttempts_ >= 3) {
            cardStatus_ = CardStatus::Blocked;
            throw AccountBlockedException("ATM card blocked after repeated invalid PIN attempts.");
        }
        throw InvalidPINException("Invalid ATM PIN.");
    }
    failedAttempts_ = 0;
}

void ATMCard::withdrawCash(int pin, double amount) {
    authenticate(pin);
    auto account = linkedAccount();
    if (!account) {
        throw SmartBankException("Linked account no longer exists.");
    }
    account->withdraw(amount);
}

void ATMCard::changePin(int currentPin, int newPin) {
    authenticate(currentPin);
    if (newPin < 1000 || newPin > 9999) {
        throw InvalidPINException("New PIN must be a 4-digit number.");
    }
    pin_ = newPin;
}

void ATMCard::block() {
    cardStatus_ = CardStatus::Blocked;
}

std::string ATMCard::summary() const {
    std::ostringstream stream;
    stream << "Card#" << cardNumber_ << " type=" << toString(cardType_) << " status=" << toString(cardStatus_);
    return stream.str();
}

std::shared_ptr<Account> AccountFactory::createAccount(long long accountNumber,
                                                       const AccountConfig& config,
                                                       const std::string& dateOpened,
                                                       const std::shared_ptr<Branch>& branch,
                                                       const std::shared_ptr<Customer>& customer) {
    switch (config.type) {
        case AccountType::Savings:
            return std::make_shared<SavingsAccount>(accountNumber,
                                                    config.initialDeposit,
                                                    dateOpened,
                                                    branch,
                                                    customer,
                                                    config.interestRate,
                                                    config.minimumBalance,
                                                    config.dailyWithdrawalLimit);
        case AccountType::Current:
            return std::make_shared<CurrentAccount>(accountNumber,
                                                    config.initialDeposit,
                                                    dateOpened,
                                                    branch,
                                                    customer,
                                                    config.overdraftLimit,
                                                    config.businessName);
        case AccountType::FixedDeposit:
            return std::make_shared<FixedDepositAccount>(accountNumber,
                                                         config.initialDeposit,
                                                         dateOpened,
                                                         branch,
                                                         customer,
                                                         config.fdInterestRate,
                                                         config.tenureMonths,
                                                         config.maturityDate);
    }
    throw SmartBankException("Unsupported account type requested.");
}

Bank::Bank(int bankId, std::string bankName)
    : bankId_(bankId), bankName_(std::move(bankName)), notifier_(std::make_unique<SMSNotification>()) {}

int Bank::bankId() const {
    return bankId_;
}

const std::string& Bank::bankName() const {
    return bankName_;
}

std::shared_ptr<Branch> Bank::createBranch(int branchId,
                                           const std::string& branchName,
                                           const std::string& ifscCode,
                                           const std::string& address) {
    if (branchIndex_.count(branchId) > 0U) {
        throw SmartBankException("Branch already exists with ID " + std::to_string(branchId));
    }
    auto branch = std::make_shared<Branch>(branchId, branchName, ifscCode, address);
    branches_.push_back(branch);
    branchIndex_[branchId] = branch;
    return branch;
}

std::shared_ptr<Customer> Bank::registerCustomer(const CustomerProfile& profile) {
    if (customerIndex_.count(profile.customerId) > 0U) {
        throw SmartBankException("Customer already exists with ID " + std::to_string(profile.customerId));
    }
    auto customer = std::make_shared<Customer>(profile);
    customers_.push_back(customer);
    customerIndex_[profile.customerId] = customer;
    return customer;
}

std::shared_ptr<Employee> Bank::hireEmployee(int employeeId,
                                             const std::string& employeeName,
                                             const std::string& designation,
                                             double salary,
                                             int branchId) {
    auto branch = findBranch(branchId);
    if (!branch) {
        throw SmartBankException("Branch not found for employee assignment.");
    }
    auto employee = std::make_shared<Employee>(employeeId, employeeName, designation, salary, branch);
    employees_.push_back(employee);
    employeeIndex_[employeeId] = employee;
    branch->addEmployee(employee);
    return employee;
}

std::shared_ptr<Account> Bank::openAccount(int customerId,
                                           int branchId,
                                           const AccountConfig& config,
                                           const std::string& dateOpened) {
    auto customer = findCustomer(customerId);
    auto branch = findBranch(branchId);
    if (!customer || !branch) {
        throw SmartBankException("Unable to open account because customer or branch was not found.");
    }
    const auto accountNumber = nextAccountNumber();
    auto account = AccountFactory::createAccount(accountNumber, config, dateOpened, branch, customer);
    accountIndex_[accountNumber] = account;
    customer->addAccount(account);
    branch->addAccount(account);
    notifyCustomer(customer,
                   "Account Opened",
                   "New " + toString(config.type) + " account opened with number " + std::to_string(accountNumber));
    return account;
}

std::shared_ptr<Loan> Bank::requestLoan(int customerId,
                                        LoanType loanType,
                                        double loanAmount,
                                        double interestRate,
                                        int tenureYears) {
    auto customer = findCustomer(customerId);
    if (!customer) {
        throw SmartBankException("Customer not found for loan request.");
    }
    if (loanAmount <= 0.0 || tenureYears <= 0) {
        throw std::invalid_argument("Loan amount and tenure must be positive.");
    }
    auto loan = std::make_shared<Loan>(nextLoanId(), loanType, loanAmount, interestRate, tenureYears, customer);
    loanIndex_[loan->loanId()] = loan;
    customer->addLoan(loan);
    notifyCustomer(customer,
                   "Loan Requested",
                   "Loan request created for amount " + formatCurrency(loanAmount) + " (" + toString(loanType) + ")");
    return loan;
}

void Bank::reviewLoan(int loanId, bool approve) {
    auto loan = findLoan(loanId);
    if (!loan) {
        throw SmartBankException("Loan not found.");
    }
    auto customer = loan->customer();
    if (approve) {
        loan->approve();
        notifyCustomer(customer,
                       "Loan Approved",
                       "Loan #" + std::to_string(loanId) + " approved with EMI " + formatCurrency(loan->emiAmount()));
        return;
    }
    loan->reject();
    notifyCustomer(customer, "Loan Rejected", "Loan #" + std::to_string(loanId) + " has been rejected.");
    throw LoanRejectedException("Loan #" + std::to_string(loanId) + " rejected by the bank.");
}

std::shared_ptr<ATMCard> Bank::issueATMCard(long long accountNumber,
                                            int pin,
                                            CardType cardType,
                                            const std::string& expiryDate) {
    auto account = findAccount(accountNumber);
    if (!account) {
        throw SmartBankException("Account not found for ATM card issuance.");
    }
    auto customer = account->customer();
    if (!customer) {
        throw SmartBankException("Cannot issue ATM card without a valid customer.");
    }
    auto card = std::make_shared<ATMCard>(nextCardNumber(), 100 + static_cast<int>(cardIndex_.size() % 900), expiryDate, pin, cardType, account);
    cardIndex_[card->cardNumber()] = card;
    customer->addCard(card);
    notifyCustomer(customer,
                   "ATM Card Issued",
                   "ATM card " + std::to_string(card->cardNumber()) + " issued for account " +
                       std::to_string(accountNumber));
    return card;
}

void Bank::deposit(long long accountNumber, double amount) {
    auto account = findAccount(accountNumber);
    if (!account) {
        throw SmartBankException("Account not found for deposit.");
    }
    account->deposit(amount);
    Transaction transaction {nextTransactionId(),
                             "Deposit",
                             amount,
                             currentTimestamp(),
                             accountNumber,
                             std::nullopt,
                             TransactionStatus::Success,
                             "Cash deposit"};
    account->recordTransaction(transaction);
    notifyCustomer(account->customer(),
                   "Deposit Successful",
                   "Deposit of " + formatCurrency(amount) + " completed in account " + std::to_string(accountNumber));
}

void Bank::withdraw(long long accountNumber, double amount) {
    auto account = findAccount(accountNumber);
    if (!account) {
        throw SmartBankException("Account not found for withdrawal.");
    }
    account->withdraw(amount);
    Transaction transaction {nextTransactionId(),
                             "Withdraw",
                             amount,
                             currentTimestamp(),
                             accountNumber,
                             std::nullopt,
                             TransactionStatus::Success,
                             "Counter withdrawal"};
    account->recordTransaction(transaction);
    notifyCustomer(account->customer(),
                   "Withdrawal Successful",
                   "Withdrawal of " + formatCurrency(amount) + " completed in account " +
                       std::to_string(accountNumber));
}

void Bank::transfer(long long senderAccountNumber, long long receiverAccountNumber, double amount) {
    auto sender = findAccount(senderAccountNumber);
    auto receiver = findAccount(receiverAccountNumber);
    if (!sender || !receiver) {
        throw SmartBankException("Sender or receiver account not found for transfer.");
    }
    sender->withdraw(amount);
    receiver->deposit(amount);
    Transaction senderTransaction {nextTransactionId(),
                                   "TransferOut",
                                   amount,
                                   currentTimestamp(),
                                   senderAccountNumber,
                                   receiverAccountNumber,
                                   TransactionStatus::Success,
                                   "Transfer to account " + std::to_string(receiverAccountNumber)};
    Transaction receiverTransaction {nextTransactionId(),
                                     "TransferIn",
                                     amount,
                                     currentTimestamp(),
                                     senderAccountNumber,
                                     receiverAccountNumber,
                                     TransactionStatus::Success,
                                     "Transfer from account " + std::to_string(senderAccountNumber)};
    sender->recordTransaction(senderTransaction);
    receiver->recordTransaction(receiverTransaction);
    notifyCustomer(sender->customer(),
                   "Transfer Debited",
                   "Transfer of " + formatCurrency(amount) + " to account " + std::to_string(receiverAccountNumber));
    notifyCustomer(receiver->customer(),
                   "Transfer Credited",
                   "Transfer of " + formatCurrency(amount) + " received from account " +
                       std::to_string(senderAccountNumber));
}

void Bank::atmWithdraw(long long cardNumber, int pin, double amount) {
    auto card = findCard(cardNumber);
    if (!card) {
        throw SmartBankException("ATM card not found.");
    }
    card->withdrawCash(pin, amount);
    auto account = card->linkedAccount();
    if (!account) {
        throw SmartBankException("ATM card does not reference a valid account.");
    }
    Transaction transaction {nextTransactionId(),
                             "ATMWithdraw",
                             amount,
                             currentTimestamp(),
                             account->accountNumber(),
                             std::nullopt,
                             TransactionStatus::Success,
                             "ATM cash withdrawal"};
    account->recordTransaction(transaction);
    notifyCustomer(account->customer(),
                   "ATM Withdrawal Successful",
                   "ATM withdrawal of " + formatCurrency(amount) + " completed from account " +
                       std::to_string(account->accountNumber()));
}

void Bank::matureFixedDeposit(long long accountNumber) {
    auto account = findAccount(accountNumber);
    if (!account) {
        throw SmartBankException("Account not found for FD maturity.");
    }
    auto* fixedDeposit = dynamic_cast<FixedDepositAccount*>(account.get());
    if (!fixedDeposit) {
        throw SmartBankException("Specified account is not a fixed deposit account.");
    }
    fixedDeposit->mature();
    notifyCustomer(account->customer(),
                   "Fixed Deposit Matured",
                   "FD account " + std::to_string(accountNumber) + " is now available for withdrawal.");
}

void Bank::setNotificationStrategy(NotificationType type) {
    notificationType_ = type;
    if (type == NotificationType::SMS) {
        notifier_ = std::make_unique<SMSNotification>();
    } else {
        notifier_ = std::make_unique<EmailNotification>();
    }
}

std::string Bank::activeNotificationChannel() const {
    return notifier_ ? notifier_->channelName() : "None";
}

std::shared_ptr<Branch> Bank::findBranch(int branchId) const {
    const auto iterator = branchIndex_.find(branchId);
    return iterator == branchIndex_.end() ? nullptr : iterator->second;
}

std::shared_ptr<Customer> Bank::findCustomer(int customerId) const {
    const auto iterator = customerIndex_.find(customerId);
    return iterator == customerIndex_.end() ? nullptr : iterator->second;
}

std::shared_ptr<Employee> Bank::findEmployee(int employeeId) const {
    const auto iterator = employeeIndex_.find(employeeId);
    return iterator == employeeIndex_.end() ? nullptr : iterator->second;
}

std::shared_ptr<Account> Bank::findAccount(long long accountNumber) const {
    const auto iterator = accountIndex_.find(accountNumber);
    return iterator == accountIndex_.end() ? nullptr : iterator->second;
}

std::shared_ptr<Loan> Bank::findLoan(int loanId) const {
    const auto iterator = loanIndex_.find(loanId);
    return iterator == loanIndex_.end() ? nullptr : iterator->second;
}

std::shared_ptr<ATMCard> Bank::findCard(long long cardNumber) const {
    const auto iterator = cardIndex_.find(cardNumber);
    return iterator == cardIndex_.end() ? nullptr : iterator->second;
}

const std::vector<std::shared_ptr<Branch>>& Bank::branches() const {
    return branches_;
}

const std::vector<std::shared_ptr<Customer>>& Bank::customers() const {
    return customers_;
}

const std::vector<std::shared_ptr<Employee>>& Bank::employees() const {
    return employees_;
}

const std::vector<std::string>& Bank::notificationLog() const {
    return notificationLog_;
}

std::string Bank::bankSummary() const {
    std::ostringstream stream;
    stream << "Bank " << bankName_ << " (#" << bankId_ << ")\n";
    stream << "Notification strategy: " << activeNotificationChannel() << "\n";
    stream << "Branches: " << branches_.size() << ", Customers: " << customers_.size()
           << ", Employees: " << employees_.size() << ", Accounts: " << accountIndex_.size()
           << ", Loans: " << loanIndex_.size() << ", Cards: " << cardIndex_.size() << "\n";
    stream << "\nBranch Details:\n";
    for (const auto& branch : branches_) {
        stream << " - " << branch->summary() << "\n";
    }
    return stream.str();
}

std::string Bank::customerReport(int customerId) const {
    auto customer = findCustomer(customerId);
    if (!customer) {
        throw SmartBankException("Customer report requested for missing customer.");
    }
    std::ostringstream stream;
    stream << "Customer Report: " << customer->fullName() << " (#" << customer->customerId() << ")\n";
    stream << "Contact: " << customer->mobileNumber() << " | " << customer->email() << "\n";
    stream << "Accounts:\n";
    for (const auto& account : customer->accounts()) {
        stream << " - " << account->summary() << "\n";
    }
    stream << "Loans:\n";
    if (customer->loans().empty()) {
        stream << " - None\n";
    } else {
        for (const auto& loan : customer->loans()) {
            stream << " - " << loan->summary() << "\n";
        }
    }
    stream << "ATM Cards:\n";
    if (customer->cards().empty()) {
        stream << " - None\n";
    } else {
        for (const auto& card : customer->cards()) {
            stream << " - " << card->summary() << "\n";
        }
    }
    return stream.str();
}

void Bank::notifyCustomer(const std::shared_ptr<Customer>& customer,
                          const std::string& subject,
                          const std::string& message) {
    if (!customer || !notifier_) {
        return;
    }
    notificationLog_.push_back(notifier_->send(*customer, subject, message));
}

int Bank::nextTransactionId() {
    return ++transactionCounter_;
}

int Bank::nextLoanId() {
    return ++loanCounter_;
}

long long Bank::nextAccountNumber() {
    return ++accountCounter_;
}

long long Bank::nextCardNumber() {
    return ++cardCounter_;
}

}  // namespace smartbank
