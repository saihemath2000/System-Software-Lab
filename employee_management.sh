#!/bin/bash

# Function to display the menu
function display_menu() {
    echo "Menu:"
    echo "1. Create Employee Record"
    echo "2. Delete Employee Record"
    echo "3. Search Employee Record"
    echo "4. Display Employee Details"
    echo "5. Sort Records"
    echo "6. List All Records"
    echo "7. Exit"
}

# Function to create a new employee record
function create_record() {
    read -p "Enter Employee Name: " name
    read -p "Enter Employee Number: " emp_number
    read -p "Enter Telephone Number: " telephone
    # Check if the employee number already exists
    if grep -q "^.*:$emp_number:.*$" employee_records.txt; then
        echo "Employee Number $emp_number already exists. Cannot create duplicate records."
    else
        # Append the new record to the file
        echo "$name:$emp_number:$telephone" >> employee_records.txt
        echo "Employee Record created successfully!"
    fi
}

# Function to delete an employee record
function delete_record() {
    read -p "Enter the Employee Number to delete: " emp_number
    # Use temporary file to store the updated records
    grep -v "^.*:$emp_number:.*$" employee_records.txt > temp.txt
    mv temp.txt employee_records.txt
    echo "Employee Record deleted successfully!"
}

# Function to search for an employee record
function search_record() {
    read -p "Enter the Employee Number to search: " emp_number
    grep "^.*:$emp_number:.*$" employee_records.txt
}

# Function to display a specific employee's details
function display_employee_details() {
    search_record
}

# Function to sort the records
function sort_records() {
    # sort -t ':' -k 2 employee_records.txt > temp.txt
    sort -t ':' -k2n employee_records.txt > temp.txt
    mv temp.txt employee_records.txt
    echo "Record Sorted Successfully!!!"
}

# Function to list all records
function list_records() {
    # cat employee_records.txt
    # s/pattern/replacement/flags
    # flag- g : global means that all occurrences of the pattern in each line should be replaced. 
    # If this flag is omitted, only the first occurrence in each line would be replaced.
    sed 's/:/ /g' employee_records.txt

}

# Main program loop
while true; do
    display_menu
    read -p "Enter your choice (1-7): " choice

    case $choice in
        1)
            create_record
            ;;
        2)
            delete_record
            ;;
        3)
            search_record
            ;;
        4)
            display_employee_details
            ;;
        5)
            sort_records
            ;;
        6)
            list_records
            ;;
        7)
            echo "Exiting..."
            exit 0
            ;;
        *)
            echo "Invalid choice. Please enter a valid option (1-7)."
            ;;
    esac

    echo # Add a new line for better formatting
done
