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
std::uint32_t register_array[12];
std::map<std::string, int> register_map //Assigns a register's string name to corresponding register_array index.
{
	{"R0", 0},
	{"R1", 1},
	{"R2", 2},
	{"R3", 3},
	{"R4", 4},
	{"R5", 5},
	{"R6", 6},
	{"R7", 7},
	{"R8", 8},
	{"R9", 9},
	{"R10", 10},
	{"R11", 11}
};

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
bool validateMemoryOperands(std::vector<std::string> operands, int line_number)
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

	if (checkRegister(operands[1]))
	{
		return true;
	}

	uint32_t address;
	try
	{
		address = static_cast<std::uint32_t>(std::stoul(operands[1], NULL, 16)); //Convert the string to an unsigned long and then cast to unsigned int of 32 bits.
		uint32_t start_address = 0x100;
		uint32_t end_address = start_address - sizeof(memory_array[0]) + sizeof(memory_array);
		if (address > end_address || address < start_address)
		{
			std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Memory Address Out of Range." << std::endl;
			return false;
		}
	}
	catch (const std::exception& ex)
	{
		std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Invalid Operand(s)." << std::endl;
		return false;
	}

	return true;
}
bool validateTwoOperands(std::vector<std::string> operands, int line_number)
{
	if (operands.size() != 2)
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
	if (operands.size() != operand_count)
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

bool validateShiftOperands(std::vector<std::string> operands, int line_number)
{
	if (operands.size() != 3)
	{
		std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
		return false;
	}

	if (!checkRegister(operands[0]) || !checkRegister(operands[1]))
	{
		std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Invalid Operand(s)." << std::endl;
		return false;
	}

	if (!checkImmediate(operands[2]))
	{
		return false;
	}
	
	return true;
}

int sign(uint32_t result) //Helper function for returning the sign of the result of an operation.
{
	int32_t signed_result = static_cast<int32_t>(result);
	if (signed_result < 0) { return -1; }
	if (signed_result > 0) { return 1; }
	return 0;
}

void processLoad(std::vector<std::string> operands)
{
	if (checkRegister(operands[1]))
	{
		register_array[register_map[operands[0]]] = memory_array[register_array[register_map[operands[1]]]];
	}
	else
	{
		uint32_t starting_address = 0x100;
		uint32_t address = static_cast<std::uint32_t>(std::stoul(operands[1], NULL, 16));
		int index = (address - starting_address) / sizeof(memory_array[0]);
		register_array[register_map[operands[0]]] = memory_array[index];
	}
}

void processStore(std::vector<std::string> operands)
{
	if (checkRegister(operands[1]))
	{
		uint32_t starting_address = 0x100;
		uint32_t address = register_array[register_map[operands[1]]];
		int index = (address - starting_address) / sizeof(memory_array[0]);
		memory_array[index] = register_array[register_map[operands[0]]];
	}
	else
	{
		uint32_t starting_address = 0x100;
		uint32_t address = static_cast<std::uint32_t>(std::stoul(operands[1], NULL, 16));
		int index = (address - starting_address) / sizeof(memory_array[0]);
		memory_array[index] = register_array[register_map[operands[0]]];
	}
}

void processCMP(std::vector<std::string> operands) //Sets the NZCV flags
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

	if (sign(operand1 - operand2) == -1) { n_flag = 1; }
	else { n_flag = 0; }

	if (sign(operand1 - operand2) == 0) { z_flag = 1; }
	else { z_flag = 0; }

	if (operand1 >= operand2) { c_flag = 1; }
	else { c_flag = 0; }

	if ((static_cast<int32_t>(operand1) > 0 && static_cast<int32_t>(operand2) < 0 && sign(operand1 - operand2) == -1) ||
		(static_cast<int32_t>(operand1) < 0 && static_cast<int32_t>(operand2) > 0 && sign(operand1 - operand2) == 1))
	{
		v_flag = 1;
	}
	else { v_flag = 0; }
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

uint32_t processArithmetic(std::vector<std::string> operands, std::string operation_type, bool update_flags)
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
	{
		uint32_t result = operand2 + operand3;
		if (update_flags)
		{
			n_flag = sign(operand2 + operand3) == -1;
			z_flag = sign(operand2 + operand3) == 0;
			c_flag = result > UINT32_MAX;
			v_flag = ((sign(operand2) ^ sign(result)) & ~(sign(operand2) ^ sign(operand3))) < 0;
		}
		return result;
	}
	else if (operation_type == "SUB")
	{
		uint32_t result = operand2 - operand3;
		if (update_flags)
		{
			n_flag = sign(operand2 - operand3) == -1;
			z_flag = sign(operand2 - operand3) == 0;
			c_flag = operand2 >= operand3;
			v_flag = ((sign(operand2) ^ sign(operand3)) & (sign(operand2) ^ sign(result))) < 0;
		}
		return result;
	}
	else if (operation_type == "AND")
	{
		uint32_t result = operand2 & operand3;
		if (update_flags)
		{
			n_flag = sign(operand2 & operand3) == -1;
			z_flag = sign(operand2 & operand3) == 0;
		}
		return result;
	}
	else if (operation_type == "ORR")
	{
		uint32_t result = operand2 | operand3;
		if (update_flags)
		{
			n_flag = sign(operand2 | operand3) == -1;
			z_flag = sign(operand2 | operand3) == 0;
		}
		return result;
	}
	else if (operation_type == "EOR")
	{
		uint32_t result = operand2 ^ operand3;
		if (update_flags)
		{
			n_flag = sign(operand2 ^ operand3) == -1;
			z_flag = sign(operand2 ^ operand3) == 0;
		}
		return result;
	}
	return 0;
}

void displayRegistersAndMemory()
{
	int register_size = sizeof(register_array) / sizeof(register_array[0]);
	int memory_size = sizeof(memory_array) / sizeof(memory_array[0]);

	std::cout << "Register Array:" << std::endl;
	for (int i = 0; i < register_size; i++)
	{
		if (i == 6) { std::cout << "\n"; }
		std::cout << "R" << std::dec << i << "=0x" << std::hex << std::uppercase << register_array[i] << ", ";
	}
	std::cout << "\nN: " << n_flag << ", Z: " << z_flag << ", C: " << c_flag << ", V: " << v_flag << std::endl;
	std::cout << "Memory Array:" << std::endl;
	for (int i = 0; i < memory_size; i++)
	{
		std::cout << "0x" << std::hex << std::uppercase << memory_array[i] << ", ";
	}
	std::cout << "\n";
}

bool checkFlags(std::string condition)
{
	if (condition == "GT")
	{
		if (n_flag == v_flag && z_flag == 0) { return true; }
		else { return false; }
	}
	else if (condition == "GE")
	{
		if (n_flag == v_flag) { return true; }
		else { return false; }
	}
	else if (condition == "LT")
	{
		if (n_flag != v_flag) { return true; }
		else { return false; }
	}
	else if (condition == "LE")
	{
		if (n_flag != v_flag && z_flag == 1) { return true; }
		else { return false; }
	}
	else if (condition == "EQ")
	{
		if (z_flag == 1) { return true; }
		else { return false; }
	}
	else if (condition == "NE")
	{
		if (z_flag == 0) { return true; }
		else { return false; }
	}

	return false;
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
			std::replace(line.begin(), line.end(), ',', ' ');

			std::stringstream ss(line);
			std::string operation;
			std::string operand;
			std::vector<std::string> operand_vec;

			ss >> operation;

			std::cout << "\n--> " << line << std::endl;
			//Arithmetic Operations
			if (operation.substr(0, 3) == "ADD" || operation.substr(0, 3) == "SUB" || operation.substr(0, 3) == "AND"
				|| operation.substr(0, 3) == "ORR" || operation.substr(0, 3) == "EOR")
			{
				bool affect_flags = false;
				if (operation.substr(3, 2) == "GT" || operation.substr(3, 2) == "GE" || operation.substr(3, 2) == "LT" ||
					operation.substr(3, 2) == "LE" || operation.substr(3, 2) == "EQ" || operation.substr(3, 2) == "NE")
				{
					if (operation.length() > 5)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					if (!checkFlags(operation.substr(3, 2)))
					{
						std::cout << "Conditional not met, instruction did not execute." << std::endl;
						displayRegistersAndMemory();
						continue;
					}
				}
				else if (operation.substr(3, 1) == "S")
				{
					if (operation.length() > 4)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					affect_flags = true;	
				}

				while (ss >> operand) //Seperate tokens by whitespace
				{
					operand_vec.push_back(operand);
				}

				if (validateArithmeticOperands(operand_vec, 3, line_number))
				{
					register_array[register_map[operand_vec[0]]] = processArithmetic(operand_vec, operation.substr(0, 3), affect_flags);
					displayRegistersAndMemory();
				}
			}
			else if (operation.substr(0, 3) == "MOV")
			{
				if (operation.substr(3, 2) == "GT" || operation.substr(3, 2) == "GE" || operation.substr(3, 2) == "LT" ||
					operation.substr(3, 2) == "LE" || operation.substr(3, 2) == "EQ" || operation.substr(3, 2) == "NE")
				{
					if (operation.length() > 5)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					if (!checkFlags(operation.substr(3, 2)))
					{
						std::cout << "Conditional not met, instruction did not execute." << std::endl;
						displayRegistersAndMemory();
						continue;
					}
				}

				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (validateTwoOperands(operand_vec, line_number))
				{
					register_array[register_map[operand_vec[0]]] = processMov(operand_vec);
					displayRegistersAndMemory();
				}
			}
			else if (operation.substr(0, 3) == "MVN")
			{
				if (operation.substr(3, 2) == "GT" || operation.substr(3, 2) == "GE" || operation.substr(3, 2) == "LT" ||
					operation.substr(3, 2) == "LE" || operation.substr(3, 2) == "EQ" || operation.substr(3, 2) == "NE")
				{
					if (operation.length() > 5)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					if (!checkFlags(operation.substr(3, 2)))
					{
						std::cout << "Conditional not met, instruction did not execute." << std::endl;
						displayRegistersAndMemory();
						continue;
					}
				}

				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (validateTwoOperands(operand_vec, line_number))
				{
					register_array[register_map[operand_vec[0]]] = ~processMov(operand_vec);
					displayRegistersAndMemory();
				}
			}
			else if (operation.substr(0, 3) == "CMP")
			{
				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (validateTwoOperands(operand_vec, line_number))
				{
					processCMP(operand_vec);
					displayRegistersAndMemory();
				}
			}
			else if (operation.substr(0, 3) == "LDR")
			{
				if (operation.substr(3, 2) == "GT" || operation.substr(3, 2) == "GE" || operation.substr(3, 2) == "LT" ||
					operation.substr(3, 2) == "LE" || operation.substr(3, 2) == "EQ" || operation.substr(3, 2) == "NE")
				{
					if (operation.length() > 5)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					if (!checkFlags(operation.substr(3, 2)))
					{
						std::cout << "Conditional not met, instruction did not execute." << std::endl;
						displayRegistersAndMemory();
						continue;
					}
				}

				while (ss >> operand)
				{
					operand.erase(std::remove(operand.begin(), operand.end(), '['), operand.end());
					operand.erase(std::remove(operand.begin(), operand.end(), ']'), operand.end());
					operand_vec.push_back(operand);
				}

				if (validateMemoryOperands(operand_vec, line_number))
				{
					processLoad(operand_vec);
					displayRegistersAndMemory();
				}
			}
			else if (operation.substr(0, 3) == "STR")
			{
				if (operation.substr(3, 2) == "GT" || operation.substr(3, 2) == "GE" || operation.substr(3, 2) == "LT" ||
					operation.substr(3, 2) == "LE" || operation.substr(3, 2) == "EQ" || operation.substr(3, 2) == "NE")
				{
					if (operation.length() > 5)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					if (!checkFlags(operation.substr(3, 2)))
					{
						std::cout << "Conditional not met, instruction did not execute." << std::endl;
						displayRegistersAndMemory();
						continue;
					}
				}

				while (ss >> operand)
				{
					operand.erase(std::remove(operand.begin(), operand.end(), '['), operand.end());
					operand.erase(std::remove(operand.begin(), operand.end(), ']'), operand.end());
					operand_vec.push_back(operand);
				}

				if (validateMemoryOperands(operand_vec, line_number))
				{
					processStore(operand_vec);
					displayRegistersAndMemory();
				}
			}
			else if (operation.substr(0, 3) == "LSL")
			{
				if (operation.substr(3, 2) == "GT" || operation.substr(3, 2) == "GE" || operation.substr(3, 2) == "LT" ||
					operation.substr(3, 2) == "LE" || operation.substr(3, 2) == "EQ" || operation.substr(3, 2) == "NE")
				{
					if (operation.length() > 5)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					if (!checkFlags(operation.substr(3, 2)))
					{
						std::cout << "Conditional not met, instruction did not execute." << std::endl;
						displayRegistersAndMemory();
						continue;
					}
				}

				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (validateShiftOperands(operand_vec, line_number))
				{
					uint32_t operand3;
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

					register_array[register_map[operand_vec[0]]] = register_array[register_map[operand_vec[1]]] << operand3;
					displayRegistersAndMemory();
				}
			}
			else if (operation.substr(0, 3) == "LSR")
			{
				if (operation.substr(3, 2) == "GT" || operation.substr(3, 2) == "GE" || operation.substr(3, 2) == "LT" ||
					operation.substr(3, 2) == "LE" || operation.substr(3, 2) == "EQ" || operation.substr(3, 2) == "NE")
				{
					if (operation.length() > 5)
					{
						std::cerr << "ERROR ON LINE: " << std::to_string(line_number) << ", Operation: " << operation << ", is Not Supported." << std::endl;
						continue;
					}
					if (!checkFlags(operation.substr(3, 2)))
					{
						std::cout << "Conditional not met, instruction did not execute." << std::endl;
						displayRegistersAndMemory();
						continue;
					}
				}

				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (validateShiftOperands(operand_vec, line_number))
				{
					uint32_t operand3;
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

					register_array[register_map[operand_vec[0]]] = register_array[register_map[operand_vec[1]]] >> operand3;
					displayRegistersAndMemory();
				}
			}
			else if (operation == "BEQ")
			{
				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (operand_vec.size() != 1)
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					continue;
				}

				if (z_flag == 1) { std::cout << "Branch Taken to: " << operand_vec[0] << std::endl; }
				else { std::cout << "Branch Not Taken to: " << operand_vec[0] << std::endl; }
			}
			else if (operation == "BNE")
			{
				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (operand_vec.size() != 1)
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					continue;
				}

				if (z_flag == 0) { std::cout << "Branch Taken to: " << operand_vec[0] << std::endl; }
				else { std::cout << "Branch Not Taken to: " << operand_vec[0] << std::endl; }
			}
			else if (operation == "BAL")
			{
				while (ss >> operand)
				{
					operand_vec.push_back(operand);
				}

				if (operand_vec.size() != 1)
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					continue;
				}

				std::cout << "Branch Taken to: " << operand_vec[0] << std::endl;
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

	std::string input;

	while (true) {
		std::cout << "Enter (y) to quit.";
		std::getline(std::cin, input);

		if (input == "y") { break; }
	}
};