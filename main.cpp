#include <cstdint>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

std::uint32_t memory_array[5];
std::uint32_t register_array[8];

int n_flag;
int z_flag;
int c_flag;
int v_flag;

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
				while (ss << operand)
				{
					operand_vec.push_back(operand);
				}

				if (operand_vec.size() != 3)
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
				else
				{

				}
			}
			else if (operation == "SUB")
			{
				if (ss >> operand1 >> operand2 >> operand3)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else if (operation == "CMP")
			{
				if (ss >> operand1 >> operand2)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else if (operation == "MOV")
			{
				if (ss >> operand1 >> operand2)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			//Bitwise Operations
			else if (operation == "AND")
			{
				if (ss >> operand1 >> operand2 >> operand3)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else if (operation == "OR")
			{
				if (ss >> operand1 >> operand2 >> operand3)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else if (operation == "XOR")
			{
				if (ss >> operand1 >> operand2 >> operand3)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			//Data Transfer
			else if (operation == "LOAD")
			{
				if (ss >> operand1 >> operand2)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else if (operation == "STORE")
			{
				if (ss >> operand1 >> operand2)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			//Branching
			else if (operation == "BAL")
			{
				if (ss >> operand1)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else if (operation == "BEQ")
			{
				if (ss >> operand1)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else if (operation == "BNE")
			{
				if (ss >> operand1)
				{

				}
				else
				{
					std::cerr << "Error: Invalid Operand Count on Line: " << std::to_string(line_number) << std::endl;
					break;
				}
			}
			else
			{
				std::cerr << "Error: Invalid Operation on Line: " << std::to_string(line_number) << std::endl;
				break;
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