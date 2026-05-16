# Rohinish Task 3 - SmartBank Enterprise System

This folder contains a complete C++ implementation of the **SmartBank Enterprise System** described in the repository root `README.md`.

## Features implemented

- Multi-branch bank hierarchy
- Customer and employee management
- Account hierarchy:
  - `SavingsAccount`
  - `CurrentAccount`
  - `FixedDepositAccount`
- Factory Pattern for account creation
- Strategy Pattern for notifications:
  - `SMSNotification`
  - `EmailNotification`
- Secure transaction handling:
  - deposits
  - withdrawals
  - transfers
  - ATM withdrawals
- Loan request and review workflow with EMI calculation
- ATM card issuance, PIN validation, and blocking after repeated failures
- Custom exceptions:
  - `InsufficientBalanceException`
  - `InvalidPINException`
  - `AccountBlockedException`
  - `LoanRejectedException`

## Project structure

```text
Rohinish_task3/
├── CMakeLists.txt
├── README.md
├── include/
│   └── SmartBank.hpp
└── src/
    ├── SmartBank.cpp
    └── main.cpp
```

## Build

From the repository root:

```bash
cmake -S Rohinish_task3 -B Rohinish_task3/build
cmake --build Rohinish_task3/build
```

## Run

```bash
./Rohinish_task3/build/smartbank_demo
```

## What the demo shows

The executable performs an end-to-end simulation:

1. Creates a bank with multiple branches
2. Registers customers and employees
3. Opens savings, current, and fixed deposit accounts
4. Runs deposits, withdrawals, and transfers
5. Requests and reviews loans
6. Issues an ATM card and validates PIN-based withdrawals
7. Demonstrates exception handling for invalid operations
8. Switches notification mode from SMS to Email
9. Prints bank and customer reports
