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
int n_flag = 0;
int z_flag = 0;
int c_flag = 0;
int v_flag = 0;

bool checkImmediate(std::string immediate)
{
	if (immediate[0] == '#')
	{
		uint32_t imm;
		immediate.erase(0, 1);

		try
		{
			if (immediate[0] == '0' && immediate[1] == 'x') //Check if value is hexidecimal or decimal.
				imm = static_cast<std::uint32_t>(std::stoul(immediate, NULL, 16)); //Convert the string to an unsigned long and then cast to unsigned int of 32 bits.
			else
				imm = static_cast<std::uint32_t>(std::stoul(immediate));
		}
		catch (const std::exception& ex)
		{
			return false;
		}

		return true;
	}
	else return false;
}

bool checkRegister(std::string register_name) //Helper function to find if register exists.
{
	auto it = register_map.find(register_name);
	if (it == register_map.end())
	{
		return false;
	}
	else return true;
}

//Validate Operator Functions
bool validateTwoOperands(std::vector<std::string> operands, int line_number)
{
	if (operands.size() != 2) //Check for correct number of operands.
	{
		std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
		return false;
	}

	if (!checkRegister(operands[0]))
	{
		std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Invalid Operand(s)." << std::endl;
		return false;
	}

	if (checkRegister(operands[1]) || checkImmediate(operands[1]))
	{
		return true;
	}
	else return false;
}

bool validateArithmeticOperands(std::vector<std::string> operands, int operand_count, int line_number)
{
	if (operands.size() != operand_count) //Check for correct number of operands.
	{
		std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
		return false;
	}

	if (!checkRegister(operands[0]) || !checkRegister(operands[1]))
	{
		std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Invalid Operand(s)." << std::endl;
		return false;
	}

	if (checkRegister(operands[2]) || checkImmediate(operands[2]))
	{
		return true;
	}
	else return false;
}

void processCMP(std::vector<std::string> operands)
{
	uint32_t operand1;
	uint32_t operand2;

	operand1 = register_array[register_map[operands[0]]];

	if (operands[1][0] == '#') //OPCODE is an immediate.
	{
		operands[1].erase(0, 1);

		if (operands[1][0] == '0' && operands[1][1] == 'x') //Check if value is hexidecimal or decimal.
			operand2 = static_cast<std::uint32_t>(std::stoul(operands[1], NULL, 16)); //Convert the string to an unsigned long and then cast to unsigned int of 32 bits.
		else
			operand2 = static_cast<std::uint32_t>(std::stoul(operands[1]));
	}
	else //OPCODE is a register
	{
		operand2 = register_array[register_map[operands[1]]];
	}

	n_flag = sign()


}

uint32_t processMov(std::vector<std::string> operands)
{
	uint32_t operand1;

	if (operands[1][0] == '#') //OPCODE is an immediate.
	{
		operands[1].erase(0, 1);

		if (operands[1][0] == '0' && operands[1][1] == 'x') //Check if value is hexidecimal or decimal.
			operand1 = static_cast<std::uint32_t>(std::stoul(operands[1], NULL, 16)); //Convert the string to an unsigned long and then cast to unsigned int of 32 bits.
		else
			operand1 = static_cast<std::uint32_t>(std::stoul(operands[1]));
	}
	else //OPCODE is a register
	{
		operand1 = register_array[register_map[operands[1]]];
	}

	return operand1;
}

uint32_t processArithmetic(std::vector<std::string> operands, std::string operation_type)
{
	uint32_t operand2;
	uint32_t operand3;

	operand2 = register_array[register_map[operands[1]]];

	if (operands[2][0] == '#') //OPCODE is an immediate.
	{
		operands[2].erase(0, 1);

		if (operands[2][0] == '0' && operands[2][1] == 'x') //Check if value is hexidecimal or decimal.
			operand3 = static_cast<std::uint32_t>(std::stoul(operands[2], NULL, 16)); //Convert the string to an unsigned long and then cast to unsigned int of 32 bits.
		else
			operand3 = static_cast<std::uint32_t>(std::stoul(operands[2]));
	}
	else //OPCODE is a register
	{
		operand3 = register_array[register_map[operands[2]]];
	}

	if (operation_type == "ADD")
		return operand2 + operand3;
	if (operation_type == "SUB")
		return operand2 - operand3;
	if (operation_type == "AND")
		return operand2 & operand3;
	if (operation_type == "OR")
		return operand2 | operand3;
	if (operation_type == "XOR")
		return operand2 ^ operand3;
}

void displayRegistersAndMemory()
{
	int register_size = sizeof(register_array) / sizeof(register_array[0]);
	int memory_size = sizeof(memory_array) / sizeof(memory_array[0]);

	std::cout << "Register Array:" << std::endl;
	for (int i = 0; i < register_size; i++)
	{
		std::cout << "R" << i << "=0x" << std::hex << std::uppercase << register_array[i] << ", ";
	}
	std::cout << "\n";
	std::cout << "Memory Array:" << std::endl;
	for (int i = 0; i < memory_size; i++)
	{
		std::cout << memory_array[i] << ", ";
	}
	std::cout << "\n";
}

void displayStatusFlags()
{
	std::cout << "N: " << n_flag << ", Z: " << z_flag << ", C: " << c_flag << ", V: " << v_flag << std::endl;
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

			std::cout << "--> " << line << std::endl;

			ss >> operation;

			//Arithmetic Operations
			if (operation == "ADD" || operation == "SUB" || operation == "AND" || operation == "OR" || operation == "XOR")
			{
				while (ss >> operand) //Seperate tokens by whitespace
				{
					operand.erase(std::remove(operand.begin(), operand.end(), ','), operand.end()); //Remove commas
					operand_vec.push_back(operand);
				}

				if (validateArithmeticOperands(operand_vec, 3, line_number))
				{
					register_array[register_map[operand_vec[0]]] = processArithmetic(operand_vec, operation);
					displayRegistersAndMemory();
				}
			}
			else if (operation == "MOV")
			{
				while (ss >> operand)
				{
					operand.erase(std::remove(operand.begin(), operand.end(), ','), operand.end());
					operand_vec.push_back(operand);
				}

				if (validateTwoOperands(operand_vec, line_number))
				{
					register_array[register_map[operand_vec[0]]] = processMov(operand_vec);
					displayRegistersAndMemory();
				}
			}
			else if (operation == "CMP")
			{
				while (ss >> operand)
				{
					operand.erase(std::remove(operand.begin(), operand.end(), ','), operand.end());
					operand_vec.push_back(operand);
				}

				if (validateTwoOperands(operand_vec, line_number))
				{
					displayRegistersAndMemory();
					processCMP(operand_vec);
					displayStatusFlags();
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