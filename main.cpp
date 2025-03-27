#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <limits>

//Memory Init
std::uint32_t memory_array[5];

//Reegister Init
std::uint32_t register_array[8];
std::map<std::string, int> register_map //Assigns a register's string name to corresponding register_array index.
{
	{"R0", 0},
	{"R1", 1},
	{"R2", 2},
	{"R3", 3},
	{"R4", 4},
	{"R5", 5},
	{"R6", 6},
	{"R7", 7}
};

//Status Flag Init
int n_flag;
int z_flag;
int c_flag;
int v_flag;

//Validate Operator Functions
bool validateArithmeticOperands(std::vector<std::string> operands, int operand_count, int line_number)
{
	if (operands.size() != operand_count) //Check for correct number of operands.
	{
		std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
		return false;
	}

	auto it = register_map.find(operands[0]); //Check if destination OPCODE is not a valid register.
	if (it == register_map.end())
	{
		std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Inavlid Destination Register." << std::endl;
		return false;
	}

	it = register_map.find(operands[1]); //Check if OPCODE 2 is not a valid register.
	if (it == register_map.end())
	{
		std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operand 2 Must be Valid Register." << std::endl;
		return false;
	}

	it = register_map.find(operands[2]); //Check OPCODE 3 is valid register.
	if (it != register_map.end())
	{
		return true;
	}
	else if (operands[2][0] == '#')
	{
		uint32_t imm;
		operands[2].erase(0, 1);

		try
		{
			if (operands[2][0] == '0' && operands[2][1] == 'x') //Check if value is hexidecimal or decimal.
				imm = static_cast<std::uint32_t>(std::stoul(operands[2], NULL, 16)); //Convert the string to an unsigned long and then cast to unsigned int of 32 bits.
			else
				imm = static_cast<std::uint32_t>(std::stoul(operands[2]));
		}
		catch (const std::exception& ex)
		{
			std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operand 3 is Invalid: " << ex.what() << std::endl;
			return false;
		}
	}
	else
	{
		std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << "Operand 3 is Invalid." << std::endl;
		return false;
	}
	return true;
}

void displayRegistersAndMemory()
{
	int register_size = sizeof(register_array) / sizeof(register_array[0]);

	std::cout << "Register Array:" << std::endl;
	for (int i = 0; i < register_size; i++)
	{
		std::cout << "R" << i << "=" << register_array[i] << ", ";
	}
	std::cout << "\nMemory Array:" << std::endl;
}

int main()
{
	//Decode
	std::fstream instruction_file;
	std::string file_path = "pp2_input.txt";

	instruction_file.open(file_path);
	if (instruction_file.is_open())
	{
		int line_number = 1;
		std::string line;

		while (getline(instruction_file, line))
		{
			std::stringstream ss(line);
			std::string operation;
			std::string operand;
			std::vector<std::string> operand_vec;

			ss >> operation;

			//Arithmetic Operations
			if (operation == "ADD")
			{
				while (ss >> operand) //Seperate tokens by whitespace
				{
					operand.erase(std::remove(operand.begin(), operand.end(), ','), operand.end()); //Remove commas
					operand_vec.push_back(operand);
				}

				if (validateArithmeticOperands(operand_vec, 3, line_number))
				{
					uint32_t operand2;
					uint32_t operand3;

					operand2 = register_array[register_map[operand_vec[1]]];

					if (operand_vec[2][0] == '#') //OPCODE is an immediate.
					{
						operand_vec[2].erase(0, 1);

						if (operand_vec[2][0] == '0' && operand_vec[2][1] == 'x') //Check if value is hexidecimal or decimal.
							operand3 = static_cast<std::uint32_t>(std::stoul(operand_vec[2], NULL, 16)); //Convert the string to an unsigned long and then cast to unsigned int of 32 bits.
						else
							operand3 = static_cast<std::uint32_t>(std::stoul(operand_vec[2])); 
					}
					else //OPCODE is a register
					{
						operand3 = register_array[register_map[operand_vec[2]]];
					}

					register_array[register_map[operand_vec[0]]] = operand2 + operand3;

					displayRegistersAndMemory();
				}
			}
			else
			{
				std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
				continue;
			}

			line_number++;
		}

		instruction_file.close();
	}
	else
	{
		std::cerr << "Error: Could not open file: " << file_path << std::endl;
	}
};