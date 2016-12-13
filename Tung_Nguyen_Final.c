// *** Tung D. Nguyen Section 02***

// CS 350, Fall 2016
// Final Project!!!!!!

#include <stdio.h>
#include <stdlib.h>	  // For error exit()
#include <string.h>

//CPU declaration 

	typedef short int Word;  //declare a typedef Word, 16 bits short int
	typedef unsigned short int Address; //the LC3 address is 16 bits
	typedef char ConditionCode; //Condition code < > =
	typedef int opcode; //opcode 0-15
	typedef int Flag; //boolean flag
	typedef int Reg; //Register number

// declare LC-3 CPU typestruct, just like in the SDC 
// Routines that need the CPU should be
// passed a pointer to the CPU value

// Define memory length to be 100 
	#define MEMLEN 65536 //Since the address is 16 bits 
	#define NREG 8 
	typedef struct{
		int mem[MEMLEN]; //memory 
		int loc_array[MEMLEN];//store the location of where value are stored
		Word origin; //origin location 
		Word reg[NREG]; //register 
		Address pc; // program counter 
		ConditionCode cc; //postive , negative,  equal 
		Flag running; // check if CPU is running 
		Word ir; // Instruction registers 
		opcode opcode; //operation code 
		int count; //the count is use for calculating the number of instruction read in

	}CPU;

	//prototypes
	int main(int argc, char *argv[]);
	void initialize_control_unit(CPU *cpu);
	void initialize_memory(int argc, char *argv[], CPU *cpu);


	void dump_control_unit(CPU *cpu);
	void dump_registers(CPU *cpu);
	void dump_memory(CPU *cpu);


	FILE *get_datafile(int argc, char *argv[]);
	void help_message(void);
	//printing out the instruction
	void print_instr(int instr,CPU *cpu,signed int loc);
	int read_execute_command(CPU *cpu);
	//The second PHASE!!!!!!!!! 
	void many_instruction_cycles(int nbr_cycles, CPU *cpu);
	void one_instruction_cycle(CPU *cpu);
	int execute_command(char cmd_char, CPU *cpu);
	//the method that read character in for TRAP
	int read_char();

	//main method 
	int main(int argc, char *argv[])
	{
		printf("LC3 final project phase 1");
		printf("Student: Tung D Nguyen. Lab section: 01");
		CPU cpu_value, *cpu = &cpu_value;
        initialize_control_unit(cpu);
        dump_control_unit(cpu);
        initialize_memory(argc, argv, cpu);
        dump_memory(cpu);

        //asking for input 
        // Run the command loop
		char *prompt = ">";
		printf("\nBeginning execution; type h for help\n%s", prompt);

		int done = read_execute_command(cpu);
		while (!done) {
			printf("%s", prompt);
			done = read_execute_command(cpu);
		}
        return 0;

	}
	//initialize CPU so that it will setup the basic value of a cpu
	void initialize_control_unit(CPU *cpu)
	{
		 for(int i=0;i<NREG;i++)
		 {
		 	cpu -> reg[i] = 0;
		 }

		 for(int i=0;i<MEMLEN;i++)
		 {
		 	cpu -> mem[i] = 0;
		 }

		 cpu -> cc = 'Z';
		 cpu -> opcode = 0;
		 cpu -> running = 1;
		 cpu -> pc =0;
		 cpu -> ir =0;
	}

	//dumping control unit 
	void dump_control_unit(CPU *cpu) {
		printf("LC3 simulator\n");
		printf("PC:		%d\n",cpu -> pc);
		printf("IR:		%d\n",cpu -> ir);
		printf("RUNNING:	%d\n",cpu -> running);
		printf("CC:		%c\n",cpu -> cc);
		dump_registers(cpu);
	}

	//dumping registers 
	void dump_registers(CPU *cpu)
	{
		printf("\n     R0: x%x\n", cpu -> reg[0]);
		printf("\n     R1: x%x\n", cpu -> reg[1]);
		printf("\n     R2: x%x\n", cpu -> reg[2]);
		printf("\n     R3: x%x\n", cpu -> reg[3]);
		printf("\n     R4: x%x\n", cpu -> reg[4]);
		printf("\n     R5: x%x\n", cpu -> reg[5]);
		printf("\n     R6: x%x\n", cpu -> reg[6]);
		printf("\n     R7: x%x\n", cpu -> reg[7]);

		  
	}

	//initialize memory address 
	void initialize_memory(int argc, char *argv[], CPU *cpu)
	{
		//int count to  count the number of intstruction. 
		cpu->count = 0;
		//Save all the location that generated in array [] location
		//The use the count to traverse through all the location memory
		FILE *datafile = get_datafile(argc, argv);
		 // Will read the next line (words_read = 1 if it started
        // with a memory value). Will set memory location loc to
        // value_read
        //define a buffer that the value will read into
        #define DATA_BUFFER_LEN 100

		//the vairable initialOrigin will check if the origin is initiallize and execute if does not
		char buffer[DATA_BUFFER_LEN];
        int value_read, words_read, loc = 0, initialOrigin = 0;

        char *read_success;    // NULL if reading in a line fails.
        // the line will then be store in the readsuccess
        read_success = fgets(buffer, DATA_BUFFER_LEN, datafile); 
        while(read_success != NULL)
        {
        	//Try to avoid the empty line or the comment section
        	if(buffer[0] == '\n' || buffer[0] == ';')
        	{
        		read_success = fgets(buffer, DATA_BUFFER_LEN, datafile);

        	}
        	else
        	{
        		words_read = sscanf(buffer, "%x", &value_read);
        		//words_read = 1 => there are value that saved to value_read
        		if(words_read==1)
        		{
        			//if not initial => initial origin, memory location that has the origin instruction
        			if(initialOrigin == 0)
        			{
        				cpu -> origin = value_read;
        				cpu -> pc = value_read;
           				//loc point to the origin to start saving to memory location
        				loc = value_read;
	        			initialOrigin = 1;
        			}
        			//else save the value read to the memory location, then increment the memory location
        			else
        			{
        				cpu -> mem[loc] = value_read;
        				//save location to the array
        				cpu->loc_array[cpu->count]=loc;
        				cpu->count ++;
        				loc++;
        				//if location is 
        				if(loc == MEMLEN)
        				{
        					loc = 0;
        				}
        			}
        		//after saving done, point to the next line to read
        		read_success = fgets(buffer, DATA_BUFFER_LEN, datafile);




        		}	
        	}
        	
        }

	}
void dump_memory(CPU *cpu) {
        printf("\nDumping the memory values:\n"); 
        int location=0;


        //looping through the array of location memory that saved
        //and print out the instruction in that location using print_instr
        for(int i = 0;i<cpu->count;i++)
        {
        	location = cpu->loc_array[i];
        	//we avoid printing the 0 value memory location
        	if(cpu -> mem[location]!= 0)
        	{	
        		print_instr(cpu->mem[location],cpu,location);
        		printf("\n");
        	}
        }
        // for each location, if the value is nonzero, then
        // print the location and value (as an integer),
        // and call print_instr on the value to print out
        // the value as an instruction
}

//printing the instruction and the opcode for each of the cpu memory location
void print_instr(int instr, CPU *cpu,signed int loc)
{

			//save the value of instruction to an temporary address, then
			//we will execute the instruction by manipulating the temporary address
			Address temp_instr = instr;
			
			//masks to save flag bit to check the sign of the instruction
			Address imm5Flag = temp_instr & 0x0020;
			Address jsrFlag = temp_instr & 0x0800;
			Address brFlag = temp_instr & 0x0E00;

			
			//shifts to prepare to save part of the instruction
			Address shift1 = (temp_instr << 4); 
			Address shift2 = (temp_instr << 7);
			Address shift3 = (temp_instr << 8);
			Address shift4 = (temp_instr << 13);
			
			//shifts to start saving value to parts of the isntruction
			Address opcode = (temp_instr >> 12);
			Address dst = (shift1 >> 13);
			Address src1 = (shift2 >> 13);
			Address src2 = (shift4 >> 13);
			Address base = (shift2 >> 13);
			Address trap = (shift3 >> 8);
			
			//variables for signed parts of the bitstring (where +- is important)
			signed short imm5;
			signed short PCoffset9;
			signed short PCoffset11;
			signed short LDRoffset;
			
			//shifts to remove excess bits
			signed short shift5 = (temp_instr << 5);
			signed short shift7 = (temp_instr << 7);
			signed short shift10 = (temp_instr << 10);
			signed short shift11 = (temp_instr << 11);
			
			//shifts to set value according to shifts
			imm5 = (shift11 >> 11);
			PCoffset9 = (shift7 >> 7);
			PCoffset11 = (shift5 >> 5);
			LDRoffset = (shift10 >> 10);
			//output according to opcode
			//base on the opcode we will have corresponding value and outputting instruction 
			//each of these instruction are hard-coded base on the op-code 
			switch ((opcode))
			{
				case  0:
					if (brFlag == 0x0800)
					{
						printf("x%x\t%x\tBRN\t%d\n", instr, loc, PCoffset9);
					}
					else if (brFlag == 0x0400)
					{
						printf("x%x\t%x\tBRZ\t%d\n", instr, loc, PCoffset9);
					}
					else if (brFlag == 0x0200)
					{
						printf("x%x%x\tBRP\t%d\n", instr, loc, PCoffset9);
					}
					else if (brFlag == 0x0C00)
					{
						printf("x%x\t%x\tBRNZ\t%d\n", instr, loc, PCoffset9);
					}
					else if (brFlag == 0x0A00)
					{
						printf("x%x\t%x\tBRNP\t%d\n", instr, loc, PCoffset9);
					}
					else if (brFlag == 0x0600)
					{
						printf("x%x\t%x\tBRZP\t%d\n", instr, loc, PCoffset9);
					}
					else if (brFlag == 0x0E00)
					{
						printf("x%x\t%x\tBR\t%d\n", instr, loc, PCoffset9);
					}
					else
					{
						printf("x%x\t%x\tNOP\n", instr,loc);
					}
					PCoffset9 += loc;
					break;

				case 1:
					if (imm5Flag == 0x0020)
					{
						printf("x%x\t %x\tADD\tR%d, R%d, %d\n", instr, loc, dst, src1, imm5);
					}
					else if(imm5Flag == 0x0000)
					{
						printf("x%x\t%x\tADD\tR%d, R%d, R%d\n", instr, loc, dst, src1, src2);
					}
						
					break;
				case 2:
					printf("x%x\t%x\tLD\tR%d, %d\n", instr, loc, dst, PCoffset9);
					PCoffset9 += loc;
					break;
				case 3:
					//dst is same as src for ST
					printf("x%x\t%x\tST\tR%d, %d\n", instr, loc, dst, PCoffset9);
					PCoffset9 += loc;
					break;
				case 4:
					if (jsrFlag == 0x0800)
					{
						printf("x%x\t%x\tJSR\t%d\n", instr, loc, PCoffset11);
						PCoffset9 += loc;
					}
					else if(jsrFlag == 0x0000)
					{
						printf("x%x\t%x\tJSRR\tR%d\n", instr, loc, base);
					}
					break;
				case 5:
					if (imm5Flag == 0x0020)
					{
						printf("x%x\t%x\tAND\tR%d, R%d, %d\n", instr, loc, dst, src1, imm5);
					}
					else if(imm5Flag == 0x0000)
					{
						printf("x%x\t%x\tAND\tR%d, R%d, R%d\n", instr, loc, dst, src1, src2);
					}
					break;
				case 6:
					printf("x%x\t%x\tLDR\tR%d, R%d, %d\n", instr, loc, dst, src1, LDRoffset);
					break;
				case 7:
					//dst is same as src for STR
					printf("x%x\t%x\tSTR\tR%d, R%d, %d\n", instr, loc, dst, src1, LDRoffset);
					break;
				case 8:
					printf("x%x\t%x\tRTI\n", instr, loc);
					break;
				case 9:
					printf("x%x\t%x\tNOT\tR%d, R%d\n", instr, loc, dst, src1);
					break;
				case 10:
					printf("x%x\t%x\tLDI\tR%d, %d\n", instr, loc, dst, PCoffset9);
					PCoffset9 += loc;
					break;
				case 11:
					printf("x%x\t%x\tSTI\tR%d, %d\n", instr, loc, dst, PCoffset9);
					PCoffset9 += loc;
					break;
				case 12:
					if (src1 == 7)
					{
						printf("x%x\t%x\tRET\n", instr, loc);
					}
					else
					{
						printf("x%x\t%x\tJMP\tR%d\n", instr, loc, src1);
					}
					break;
				//there is no operation in case 13
				case 13:
					printf("x%x\t%d\terr\n", cpu->mem[instr], cpu->mem[instr]);
					break;
				case 14:
					printf("x%x\t%x\tLEA\tR%d, %d\n", instr, loc, dst, PCoffset9);
					PCoffset9 += loc;
					break;
				case 15:
					if (trap == 0x20)
					{					
						printf("x%x\t%x\tTRAP\tGETC\n", instr, loc);
					}
					else if (trap == 0x21)
					{
						printf("x%x\t%x\tTRAP\tOUT\n", instr, loc);
					}
					else if (trap == 0x22)
					{
						printf("x%x\t%x\tTRAP\tPUTS\n", instr, loc);
					}
					else if (trap == 0x23)
					{
						printf("x%x\t%x\tTRAP\tIN\n", instr, loc);
					}
					else if (trap == 0x25)
					{
						printf("x%x\t%x\tTRAP\tHALT\n", instr, loc);

					}
					else
					{
						printf("x%x\t%x\tTRAP\t%d\n", instr, loc, trap);
					}
					break;
				default:
					printf("BAD opcode: %d; quitting\n", opcode);
					cpu->running = 0;
					break;
			}			
	
	

}

// Get the data file to initialize memory with.  If it was
// specified on the command line as argv[1], use that file
// otherwise use default.sdc.  If file opening fails, complain
// and terminate program execution with an error.

FILE *get_datafile(int argc, char *argv[]) {
        char *default_datafile_name = "default.hex";
        char *datafile_name = NULL;

        // set datafile name to argv[1] or default
        datafile_name = default_datafile_name; 
        //if there exist argv[1], change the file name
        if(argv[1]!=NULL)
        {
        	datafile_name=argv[1];
        }
        FILE *datafile = fopen(datafile_name, "r");

        // if the open failed, complain and call
        // exit(EXIT_FAILURE); to quit the entire program
        if(datafile==NULL)
        {
        	printf("Cannot open the file!!!\n");
        	exit(EXIT_FAILURE);
        }
        return datafile;
}

// Read a simulator command from the keyboard (q, h, ?, d, number,
// or empty line) and execute it.  Return true if we hit end-of-input
// or execute_command told us to quit.	Otherwise return false.
//
int read_execute_command(CPU *cpu) {
// Buffer to read next command line into
#define CMD_LINE_LEN 80
    char cmd_line[CMD_LINE_LEN];
    char *read_success;     // NULL if reading in a line fails.

    int nbr_cycles;     // Number of instruction cycles to execute
    char cmd_char;      // Command 'q', 'h', '?', 'd', or '\n'
    int done = 0;       
	
	int read, check= 0;

    read_success = fgets(cmd_line, CMD_LINE_LEN, stdin);
	
	read = cmd_line[0];
	check = sscanf(cmd_line, "%d", &read);
    // *** STUB ***  Quit if read_success indicates end-of-file
	if (read_success == EOF)
	{
		done = 1;
	}
	
	// *** STUB ***
	// while not done,
	//Handle q, h, ?, d commands, integer (nbr of instruction cycles),
	//or empty line (one instruction cycle)
	//Read next command line	
	if (check <= 0)
	{
		cmd_char = *read_success;		
		done = execute_command(cmd_char, cpu);
	}
	else
	{
		nbr_cycles = read;
		many_instruction_cycles(nbr_cycles, cpu);
	}

    return done;
}

// Execute a nonnumeric command; complain if it's not 'h', '?',
// 'd', 'q' or '\n'. Return true for the q command, false otherwise.
//
int execute_command(char cmd_char, CPU *cpu) {
	if (cmd_char == '?' || cmd_char == 'h') {
		help_message();
	}
	//quit if type q
	else if(cmd_char=='q')
	{
		printf("quitting");
		exit(1);
		return 1;
	}
	else if(cmd_char=='d')
	{
		dump_control_unit(cpu);
	}
	else if(cmd_char=='\n')
	{

		one_instruction_cycle(cpu);
	}
	else
	{
		printf("not a nonnumeric command\n");
	}
	return 0;
}

//many_instruction cycles implemented, the many instruction cycle use to execute as many the instruction
// as the user want, as long as it not exceed 100 cycles, 
// the cycle will run one_instruction_cycle at a time 
//If the computer is not running, it wont do the e
void many_instruction_cycles(int nbr_cycles, CPU *cpu) {
	int INSANE_NBR_CYCLES = 100;

	// *** STUB ****
	if(nbr_cycles>=INSANE_NBR_CYCLES)
	{
		printf("the number is too large\n");
		nbr_cycles=99;
	}
	else if(nbr_cycles<=0)
	{
		printf("number off cycle too small\n");
		return;
	}
	
	if(cpu -> running != 1 )
	{
		printf("HALTED! CPU is not running\n");
		return;
	}

	for(int i=0;i< nbr_cycles;i++)
	{
		if(cpu ->running != 1)
		{
			printf("the CPU is not running\n");
			return;
		}
		else
		{

			one_instruction_cycle(cpu);
		}
	}


}

// Execute one instruction cycle
//
void one_instruction_cycle(CPU *cpu){

	//if the CPU is not running, say HALTED and stop 
	if(cpu->running == 0)
	{
		printf("The CPU is not running\n");
		return; 
	}

	//if the pc is out of range, complain and stop 
	if(!(cpu -> pc >= 0 && cpu -> pc <= 0xFFFF))
	{
		printf("Out of range");
		return;
	}
	if (cpu->mem[cpu->pc] == 0)
	{
		printf("%d", cpu->mem[cpu->pc]);
		cpu->running = 0;
	}

	//manipulating the temporary memory(just like in print_str)
	Address temp_memory = cpu->mem[cpu->pc];
		
	int instr = cpu->pc;
	cpu -> ir = cpu->mem[cpu->pc]; 
	(cpu -> pc)++;
	//
	//masks to save flag bit to check the sign of the instruction
	Address imm5Flag = temp_memory & 0x0020;
	Address jsrFlag = temp_memory & 0x0800;
	Address brFlag = temp_memory & 0x0E00;

			
	//shifts to prepare to save part of the instruction
	Address shift1 = (temp_memory << 4); 
	Address shift2 = (temp_memory << 7);
	Address shift3 = (temp_memory << 8);
	Address shift4 = (temp_memory << 13);

	//implemeting type of operand from the instruction
	//The cpu opcode is set after this 
	signed short opcode = (temp_memory >> 12);
	cpu->opcode = opcode;

	signed short dst = (shift1 >> 13);
	signed short base = (shift2 >> 13);
	signed short trap = (shift3 >> 8);
	signed short src1 = (shift2 >> 13);
	signed short src2 = (shift4 >> 13);
	
	//shifts to remove excess bits
	signed short shift5 = (temp_memory << 5);
	signed short shift7 = (temp_memory << 7);
	signed short shift10 = (temp_memory << 10);
	signed short shift11 = (temp_memory<< 11);

	//variables for signed parts of the bitstring (where +- is important)
	signed short imm5 = (shift11 >> 11);
	signed short PCoffset9 = (shift7 >> 7);
	signed short PCoffset11 = (shift5 >> 5);
	signed short LDRoffset = (shift10 >> 10);

	//setting the variable to use later in the instruction execution
	//the current_offset is for saving the PCoffset before we change it in an instruction 
	int current_offset = 0;
	//temp_R7 is use for when making a jump, we need to modify the value of R7 so we need to save the real value of it somewhere else
	Address temp_R7 = 0;

	//Let's jump into executing the instruction!!!! 
	//Note that PCOffset9 is already the PC after incrementing
		switch ((opcode))
		{
			case  0:
			//case 0, let PCoffset9 = destination when the cpu go when the condition is correct, so it is + cpu->pc + 1 already
				current_offset=PCoffset9;
				PCoffset9 += (cpu->pc);
				//under this are the case that the brFlag will have, each corespond to the condition 
				if (brFlag == 0x0800)
				{
					printf("x%x\tBRN\t%d\n", cpu->mem[instr],current_offset);


					if (cpu->cc=='N')
					{
						cpu->pc = PCoffset9;
						printf("Branch to %x\n",cpu->pc);
					}
					else
					{
						printf("no branch\n");
					}
				}
				else if (brFlag == 0x0400)
				{
					printf("x%x\tBRZ\tx%x\n",cpu->mem[instr], PCoffset9);
					if (cpu->cc == 'Z')
					{
						cpu->pc = PCoffset9;
						printf("Branch to %x\n",cpu->pc);
					}
					else
					{
						cpu->pc+=1;
					}
				}
				else if (brFlag == 0x0200)
				{
					printf("x%x\tBRP\t%d\n",cpu->mem[instr],current_offset);
					if (cpu->cc =='P')
					{
						cpu->pc = PCoffset9;
						printf("Branch to %x\n",cpu->pc);
					}
					else
					{
						printf("no branch");
					}
				}
				else if (brFlag == 0x0C00)
				{
					PCoffset9+=1;
					printf("x%x\t%x\tBRNZ\tx%x\n", cpu->mem[instr], cpu->mem[instr],PCoffset9);
					if ((cpu->cc=='Z')||(cpu->cc=='N'))
					{
						cpu->pc = PCoffset9-1;
						printf("Branch to %x\n",cpu->pc);
					}
					else
					{
						cpu->pc+=1;
					}
				}
				else if (brFlag == 0x0A00)
				{
					printf("x%x\t%x\tBRNP\t%d\n", cpu->mem[instr], cpu->mem[instr], PCoffset9);
					if ((cpu->cc=='N')||(cpu->cc=='P'))
					{
						cpu->pc = PCoffset9;
					}
				}
				else if (brFlag == 0x0600)
				{
					printf("x%x\t%x\tBRZP\t%d\n", cpu->mem[instr], cpu->mem[instr], PCoffset9);
					if ((cpu->cc=='Z')||(cpu->cc=='P'))
					{
						cpu->pc = PCoffset9;
						printf("Branch to %x\n",cpu->pc);
					}
				}
				else if (brFlag == 0x0E00)
				{
					printf("x%x\t%x\tBR\t%d\n", cpu->mem[instr], cpu->mem[instr], PCoffset9);
					cpu->pc = PCoffset9;
					printf("Branch to %x\n",cpu->pc);
				}
				else
				{
					printf("x%x\t%x\tNOP\n", cpu->mem[instr], cpu->mem[instr]);
				}
				break;
			//case ADD, if the flag is 0020 then it is the sum of register + immediate value, else it is the sum of 2 regs 
			case 1:
				if (imm5Flag == 0x0020)
				{
					printf("x%x\tADD\tR%d, R%d, %d\n",cpu->mem[instr], dst, src1, imm5);
					cpu->reg[dst] = cpu->reg[src1] + imm5;
					printf("Add immediate, value of register  destination is %x\n",cpu->reg[dst]);
					if(cpu->reg[dst]==0)
					{
						cpu->cc='Z';
					}

					else if(cpu->reg[dst]>0)
					{
						cpu->cc='P';
					}
					else if(cpu->reg[dst]<0)
					{
						cpu->cc='N';
					}
				}
				else if(imm5Flag == 0x0000)
				{
					printf("x%x\t %x\tADD\tR%d, R%d, R%d\n", cpu->mem[instr], cpu->mem[instr], dst, src1, src2);
					cpu->reg[dst] = cpu->reg[src1] + cpu->reg[src2];
					printf("Add 2 registers, value of reg at destination is %x\n",cpu->reg[dst]);
					if(cpu->reg[dst]==0)
					{
						cpu->cc='Z';
					}

					else if(cpu->reg[dst]>0)
					{
						cpu->cc='P';
					}
					else if(cpu->reg[dst]<0)
					{
						cpu->cc='N';
					}

				}	
				break;
			//Load 
			case 2:
				printf("x%x\tLD\tR%d, %d\n",cpu->mem[instr], dst, PCoffset9);
				cpu->reg[dst] = cpu->mem[abs(PCoffset9+cpu->pc)];
				printf("load reg %d with %x\n",dst,cpu->reg[dst]);
				if(cpu->mem[abs(PCoffset9+cpu->pc)]==0)
				{
					cpu->cc='Z';
				}

				else if(cpu->mem[abs(PCoffset9+cpu->pc)]>0)
				{
					cpu->cc='P';
				}
				else if(cpu->mem[abs(PCoffset9+cpu->pc)]<0)
				{
					cpu->cc='N';
				}


				break;
			//store value
	 		case 3:

				printf("x%x\tST\tR %d, %d\n",cpu->mem[instr], dst, PCoffset9);
				cpu->mem[(cpu->pc)+PCoffset9] = cpu->reg[dst];
				printf("store value from R%d to memory at x%x is x%x",dst,cpu->pc+PCoffset9,cpu->reg[dst]);
				break;

			//JSR, JSRflag is to check if it is R7
			case 4:
				if (jsrFlag == 0x0800)
				{
					PCoffset11 += cpu->pc+1;
					printf("x%x\tJSR\t%d\n", cpu->mem[instr], current_offset);
					cpu->reg[7] = cpu->pc;
					cpu->pc = PCoffset11;
					printf("Jump to %x",cpu->pc);
				}
				else if(jsrFlag == 0)
				{
					printf("x%x\tJSRR\tR%d\n",cpu->mem[instr], base);
					temp_R7 = cpu->reg[base];
					cpu->reg[7] = cpu->pc;
					cpu->pc = temp_R7;
					printf("Jump to %x",cpu->pc);

				}
				break;
			case 5:
				if (imm5Flag == 0x0020)
				{
					printf("x%x\t% d\tAND\tR%d, R%d, %d\n", cpu->mem[instr], cpu->mem[instr], dst, src1, imm5);
					cpu->reg[dst] = cpu->reg[src1] && imm5; 
					printf("And between with immediate value and an register, R%d is now %d",dst,cpu->reg[dst]);
				}
				else if(imm5Flag == 0x0000)
				{
					printf("x%x\t% d\tAND\tR%d, R%d, R%d\n", cpu->mem[instr], cpu->mem[instr], dst, src1, src2);
					cpu->reg[dst] = cpu->reg[src1] && cpu->reg[src2];
					printf("And between 2 registers, R%d is now %d",dst,cpu->reg[dst]);

				}

				if (cpu->reg[dst] == 0)
				{
					cpu->cc = 'Z';
				}
				else if (cpu->reg[dst] > 0)
				{
					cpu->cc = 'P';
				}
				else if (cpu->reg[dst] < 0)
				{
					cpu->cc = 'N';
				}
				break;
			case 6:
				printf("x%x\tLDR\tR%d, R%d, %d\n",cpu->mem[instr], dst, src1, LDRoffset);
				unsigned short int tem = cpu->reg[src1] + (unsigned)LDRoffset;
				cpu->reg[dst] = cpu->mem[tem];
				if (cpu->reg[dst] == 0)
				{
					cpu->cc = 'Z';
				}
				else if (cpu->reg[dst] > 0)
				{
					cpu->cc = 'P';
				}
				else if (cpu->reg[dst] < 0)
				{
					cpu->cc = 'N';
				}
				printf("R%d is now %x",dst,cpu->reg[dst]);
				break;
			case 7:
				printf("x%x\tSTR\tR%d, R%d,%d\n",cpu->mem[instr], dst, src1, LDRoffset);
				unsigned short int tem2 = cpu->reg[src1] + (unsigned)LDRoffset;
				cpu->mem[tem2] = cpu->reg[dst];
				printf("store value to %x ",tem2);
				break;
			case 8:
				printf("x%x\t% d\tRTI\n", cpu->mem[instr], cpu->mem[instr]);
				break;
			case 9:
			//in this case we will save the value to somewhere else, then calculate it and save it back to the destination
				printf("x%x\tNOT\tR%d, R%d\n",cpu->mem[instr], dst, src1);
				signed short int temp_value = cpu->reg[src1];
				signed short int final_value = (-1*temp_value)-1;
				cpu->reg[dst]=final_value;

				if (final_value == 0)
				{
					cpu->cc = 'Z';
				}
				else if (final_value > 0)
				{
					cpu->cc = 'P';
				}
				else if (final_value < 0)
				{
					cpu->cc = 'N';
				}				
				break;
			case 10:

				printf("x%x\t% d\tLDI\tR%d, x%x\n", cpu->mem[instr], cpu->mem[instr], dst, PCoffset9+cpu->pc);
				cpu->reg[dst] = cpu->mem[cpu->mem[PCoffset9+cpu->pc]];
				if (cpu->reg[dst] == 0)
				{
					cpu->cc = 'Z';
				}
				else if (cpu->reg[dst] > 0)
				{
					cpu->cc = 'P';
				}
				else if (cpu->reg[dst] < 0)
				{
					cpu->cc = 'N';
				}	
				printf("Load to R%d the value x%x\n",dst,cpu->mem[cpu->mem[PCoffset9+cpu->pc]]);

				break;
			case 11:
				printf("x%x\t% d\tSTI\tR%d, %d\n", cpu->mem[instr], cpu->mem[instr], dst, PCoffset9+cpu->pc);
				cpu->mem[cpu->mem[PCoffset9+cpu->pc]] = cpu->reg[dst];
				if (cpu->reg[dst] == 0)
				{
					cpu->cc = 'Z';
				}
				else if (cpu->reg[dst] > 0)
				{
					cpu->cc = 'P';
				}
				else if (cpu->reg[dst] < 0)
				{
					cpu->cc = 'N';
				}	
				printf("Store to x%x the value x%x\n",PCoffset9+cpu->pc,cpu->reg[dst]);
				break;
			case 12:
				//src1 same as base for JMP
				if (src1 == 7)
				{
					printf("x%x\tRET\n",cpu->mem[instr]);
					cpu->pc = cpu->reg[src1];
				}
				else
				{
					printf("x%x\tJMP\tR%d\n",cpu->mem[instr], src1);
					cpu->pc = cpu->reg[src1];
				}
				break;
			case 13:
				printf("x%x\t%d\terr\n", cpu->mem[instr], cpu->mem[instr]);
				break;
			case 14:
				printf("x%x\tLEA\tR%d, %d\n",cpu->mem[instr], dst, PCoffset9);
				cpu->reg[dst] = cpu->pc+PCoffset9;
				if (PCoffset9 == 0)
				{
					cpu->cc = 'Z';
				}
				else if (PCoffset9 > 0)
				{
					cpu->cc = 'P';
				}
				else if (PCoffset9 < 0)
				{
					cpu->cc = 'N';
				}


				break;
		//TRAP
			case 15:
				cpu->reg[7]=cpu->pc;
				if (trap == 0x20)
				{
					printf("x%x\tTRAP\tGETC\n", cpu->mem[instr]);
					cpu->cc='N';

					cpu->reg[0] = read_char();
				}
				else if (trap == 0x21)
				{
					printf("x%x\tTRAP\tOUT\n",cpu->mem[instr]);
					printf("%c\n", cpu->reg[0]);

				}

				//for the PUTS point to R0, then read from their until meet '\0' 
				else if (trap == 0x22)
				{
					printf("x%x\tTRAP\tPUTS\n",cpu->mem[instr]);

					cpu->cc='N';
					unsigned short int pointer = cpu->reg[0];
					unsigned short int value;

					do{
						value = cpu->mem[pointer++];
						printf("%c",value);
					}while(value !='\0');


				}
				else if (trap == 0x23)
				{
					printf("x%x\tTRAP\tIN\n",cpu->mem[instr]);
					printf("Please input");
					
					cpu->reg[0] = read_char();
					cpu->cc='N';



				}
				else if (trap == 0x25)
				{
					printf("x%x\t% d\tTRAP\tHALT\n", cpu->mem[instr], cpu->mem[instr]);
					cpu->running=0;
					cpu->cc='P';
					printf("HALTING\n");
				}
				else
				{
					printf("x%x\t% d\tTRAP\tx%.2hX\n", cpu->mem[instr], cpu->mem[instr], trap);
				}
				break;
			default:
				printf("BAD OPCODE: %d; quitting\n", opcode);
				cpu->running = 0;
				break;
		}		



}

//method reach char 

int read_char()
{
	char buffer[3] = "";
	fgets(buffer, sizeof buffer, stdin);
	return buffer[0];
}
	//help message 
	//printing out the help message 
	void help_message(void){
		printf("Type ? or h for help\n");
		printf("Type d for printing the cpu and memory addres\n");
		printf("Type an integer to indicate the number of instructions you want to run\n");
		printf("Type nothing to execute one cycle\n");
		printf("Type q to quit\n");
	}
