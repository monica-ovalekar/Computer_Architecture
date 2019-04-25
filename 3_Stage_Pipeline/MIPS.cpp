#include<array>
#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define ADDU 1
#define SUBU 3
#define AND 4
#define OR  5
#define NOR 7
#define MemSize 65536 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

class RF
{
    public:
        bitset<32> ReadData1;
        bitset<32> ReadData2;

     	RF()
    	{
			Registers.resize(32);
			Registers[0] = bitset<32> (0);
        }

        void ReadWrite(unsigned int RdReg1, unsigned int RdReg2, unsigned int WrtReg, unsigned int WrtData, unsigned int WrtEnable)
        {
			if (WrtEnable == 1) // Write in the register file
			{
				Registers[WrtReg] = WrtData;
			}
			else
			{
				ReadData1 = Registers[RdReg1];
				ReadData2 = Registers[RdReg2];
			}
         }

		void OutputRF()
        {
			ofstream rfout;
			rfout.open("RFresult.txt",std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"A state of RF:"<<endl;
				for (unsigned int j = 0; j<32; j++)
				{
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open file";
			rfout.close();
        }
	private:
            vector<bitset<32> >Registers;
};

class ALU
{
      public:
             unsigned int ALUOperation (unsigned int ALUOP, unsigned int oprand1, unsigned int oprand2)
             {
                 bitset<32> ALUres(0);
                 unsigned int ALUresult = 0;
                 bitset <32> Op1 (oprand1);
                 bitset <32> Op2 (oprand2);

                 switch (ALUOP)
                 {
                 	case 1:
                 	ALUresult = oprand1 + oprand2;
                 	break;

                 	case 3:
                 	ALUresult = oprand1 - oprand2;
                 	break;

                 	case 4:
                 	ALUres = Op1 & Op2;
					ALUresult = ALUres.to_ulong();
                 	break;

                 	case 5:
                 	ALUres = Op1 | Op2;
					ALUresult = ALUres.to_ulong();
                 	break;

                 	case 7:
                 	ALUres = ~(Op1 | Op2);
					ALUresult = ALUres.to_ulong();
                 	break;
				 }
                 return ALUresult;
             }
};

class INSMem
{
      public:
          
          INSMem()
          {       
			IMem.resize(MemSize);
			ifstream imem;
			string line;
			unsigned int i=0;
			imem.open("imem.txt");
			if (imem.is_open())
			{
			while (getline(imem,line))
				{
				IMem[i] = bitset<8>(line);
				i++;
				}

			}
			else cout<<"Unable to open file";
			imem.close();
          }

          unsigned int ReadMemory (unsigned int ReadAddr)
          {
               unsigned int Instruction;
               bitset<8> ins3, ins2, ins1, ins0;  // ins3 is the MSB and ins0 is the LSB
               string INS3, INS2, INS1, INS0;
               ins3 = IMem[ReadAddr];   // ins3 to ins0 are in bitset form of 8 bits each
               ins2 = IMem[ReadAddr + 1];
               ins1 = IMem[ReadAddr + 2];
               ins0 = IMem[ReadAddr + 3];

               INS3 = ins3.to_string();  // ins3 to ins0 are converted to unsighned long integer format
               INS2 = ins2.to_string();
               INS1 = ins1.to_string();
               INS0 = ins0.to_string();
               INS3 += INS2;
               INS3 += INS1;
               INS3 += INS0;

               bitset<32> InstrMem(INS3);  // ins3 is chaged to 32 bits bitset value
               Instruction = InstrMem.to_ulong();
               return Instruction;
          }

      private:
           vector<bitset<8> > IMem;

};

class DataMem
{
      public:
          bitset<32> readdata;
          DataMem()
          {
             DMem.resize(MemSize);
             ifstream dmem;
                  string line;
                  unsigned int i=0;
                  dmem.open("dmem.txt");
                  if (dmem.is_open())
                  {
                  while (getline(dmem,line))
                       {
                        DMem[i] = bitset<8>(line);
                        i++;
                       }
                  }
                  else cout<<"Unable to open file";
                  dmem.close();
          }
          unsigned int MemoryAccess (unsigned int Addr, unsigned int WriteD, unsigned int readmem, unsigned int writemem)
          {
               bitset <32> ReadD;
               unsigned int b, readdata;
               bitset<32> WriteData (WriteD);
               bitset<8> WR3, WR2, WR1, WR0;  // WR3 contains MSB and WR0 contains the LSB, and WriteData is fragmented
               if (writemem == 1)
               {
                  for (b=0; b<8; b++)
                  {
                    WR0[b] = WriteData[b];
                  }

                  for (b=8; b<16; b++)
                  {
                    WR1[b-8] = WriteData[b];
                  }

                  for (b=16; b<24; b++)
                  {
                    WR2[b-16] = WriteData[b];
                  }

                  for (b=24; b<32; b++)
                  {
                    WR3[b-24] = WriteData[b];
                  }
                  DMem[Addr] = bitset<8>(WR3);
                  DMem[Addr + 1] = bitset<8>(WR2);
                  DMem[Addr + 2] = bitset<8>(WR1);
                  DMem[Addr + 3] = bitset<8>(WR0);
                  readdata = 0;
               }

               if (readmem == 1)
               {
                
					// bitset <32> READDATA = DMem[Addr];
				   bitset<8> dmm3, dmm2, dmm1, dmm0;  // dmm3 is the MSB and dmm0 is the LSB
				   string DMM3, DMM2, DMM1, DMM0;
				   dmm3 = DMem[Addr];   // dmm3 to dmm0 are in bitset form of 8 bits each
				   dmm2 = DMem[Addr + 1];
				   dmm1 = DMem[Addr + 2];
				   dmm0 = DMem[Addr + 3];

				   DMM3 = dmm3.to_string();  // dmm3 to dmm0 are converted to unsighned long integer format
				   DMM2 = dmm2.to_string();
				   DMM1 = dmm1.to_string();
				   DMM0 = dmm0.to_string();
				   DMM3 += DMM2;
				   DMM3 += DMM1;
				   DMM3 += DMM0;

				   ReadD = bitset<32> (DMM3);
				   readdata = ReadD.to_ulong();
               }

               return readdata;
          }

          void OutputDataMem()
          {
				ofstream dmemout;
				dmemout.open("dmemresult.txt");
				if (dmemout.is_open())
				{
					for (unsigned int j = 0; j< 1000; j++)
					{
						dmemout << DMem[j]<<endl;
					}
				}
				else cout<<"Unable to open file";
				dmemout.close();

          }

      private:
           vector<bitset<8> > DMem;

};

void dumpResults(bitset<32> pc, bitset<5> WrRFAdd, bitset<32> WrRFData, bitset<1> RFWrEn, bitset<32> WrMemAdd, bitset<32> WrMemData, bitset<1> WrMemEn)
{
		ofstream fileout;

		fileout.open("Results.txt",std::ios_base::app);
		if (fileout.is_open())
		{
			fileout <<pc<<' '<<WrRFAdd<<' '<<WrRFData<<' '<<RFWrEn<<' '<<WrMemAdd<<' '<<WrMemData<<' '<<WrMemEn << endl;
		}
		else cout<<"Unable to open file";
		fileout.close();

}


int main()
{
	bitset<3> ALUOpCode(0); bitset<32> Opr1(0); bitset<32> Opr2(0);
	bitset<32> ReadWrAddress(0); bitset<32> WriteDataMem(0); bitset<1> ReadDataMemEn(0); bitset<1> WriteDataMemEn(0);
	bitset<5> ReadReg1(0); bitset<5> ReadReg2(0); bitset<5> WriteReg(0); bitset<32> WrData(0); bitset<1> WriteEnable(0); bitset<32> ReadMemData(0);
	bitset<32> ReadRegD1; bitset<32> ReadRegD2;
	unsigned int pc=0, JUMPADDRESS, BranchAddr, ReadData, RFdata, RFRead, ALUAns, Instr, equal = 0;
	bitset<32> instr(0);
	bitset<32> Result(0);
	string INSTR, OPCODE, ReadRegData1, Immediate, SignExtImm;
        
    ALU myALU;
    INSMem myInsMem;
    DataMem myDataMem;
    RF myRF;
    unsigned int JCounter = 0;

    while (1) //each loop body corresponds to one clock cycle.
	{	 

		//Stage 3 Write to RF and DMem
        if((OPCODE == "000100") && (equal == 1))  // for beq
        {
          pc = BranchAddr;
          equal = 0;
        }

        //For Read and Write Memory Enable signals
        if (OPCODE == "101011") //store word
        {
          WriteDataMemEn = bitset<1> (1);
          ReadDataMemEn = bitset<1> (0);
        }

        else if (OPCODE == "100011") // load word
        {
          WriteDataMemEn = bitset<1> (0);
          ReadDataMemEn = bitset<1> (1);
        }
        else
        {
          WriteDataMemEn = bitset<1> (0);
          ReadDataMemEn = bitset<1> (0);
        }

        WriteDataMem = bitset<32> (ReadRegData1);
        ReadWrAddress = Result;

        // Write to Mem or read from mem
        ReadData = myDataMem.MemoryAccess (ReadWrAddress.to_ulong(), WriteDataMem.to_ulong(), ReadDataMemEn.to_ulong(), WriteDataMemEn.to_ulong());
        ReadMemData = bitset<32> (ReadData);
        
        // Write to Mem
        if (OPCODE == "100011") // load word
        {
          WrData = ReadMemData;
          WriteEnable = bitset <1> (1);
        }
        else if (OPCODE == "000000")  // a R type instruction
        {
          WrData = Result;
          // cout << " R type" << endl;
          WriteEnable = bitset <1> (1);
        }
        else
        {
          WriteEnable = bitset <1> (0); // enabled only for load and R instructions
        }
      
		myRF.ReadWrite(ReadReg1.to_ulong(), ReadReg2.to_ulong(), WriteReg.to_ulong(), WrData.to_ulong(), WriteEnable.to_ulong());		    


        //Stage 2 Read RF, Decode and Execute

        //Decode stage
        INSTR = instr.to_string();
        OPCODE = INSTR.substr(0,6); // fetches the opcode
        
        if (INSTR == "11111111111111111111111111111111")
        {
          break;  // since previous instruction is already written and completed
        }
        
        //if it is a jump instruction
        if (OPCODE == "000010")
        {
          bitset <32> PC (pc);  // to bitset
          string InstrAddr = PC.to_string(); // to string
          string JAddr = InstrAddr.substr(0,4);  // first 4 MSBs
          string address = INSTR.substr(6,26);  // address in instruction
          JAddr += address;
          JAddr += "00";
          bitset <32> jaddr (JAddr);
          unsigned int JumpAddress = jaddr.to_ulong();
          pc = JumpAddress;  // pc + 4 already done

          ReadReg1 = bitset<5> (0);
          ReadReg2 = bitset<5> (0);
          WriteReg = bitset<5> (0); // Destination register for R type insruction
          WriteEnable = bitset<1> (0);
          
        }

        //To read registers
        if ((OPCODE != "000000") && (OPCODE != "000010") && (OPCODE != "000100")) //I type
        {
          //for I type
          ReadReg1 = bitset<5> (INSTR.substr(6,5));
          ReadReg2 = bitset<5> (0);
          WriteReg = bitset<5> (INSTR.substr(11,5)); // Destination register for I type insruction
          WriteEnable = bitset<1> (0);

        }
        else if (OPCODE == "000000" || (OPCODE != "000100"))
        {
          //for R type and beq
          ReadReg1 = bitset<5> (INSTR.substr(6,5));
          ReadReg2 = bitset<5> (INSTR.substr(11,5));
          WriteReg = bitset<5> (INSTR.substr(16,5)); // Destination register for R type insruction
          WriteEnable = bitset<1> (0);
        }

        string ReadRegData1 = myRF.ReadData1.to_string();
        string ReadRegData2 = myRF.ReadData2.to_string();

        //Decode
        if ((OPCODE != "000000") && (OPCODE != "000010") && (OPCODE != "000100" && (OPCODE != "000010"))) //I type
        {

          if (INSTR[16] == '1')   // sign extending immediate value
          {
            SignExtImm = "1111111111111111";
          }
          else
          {
            SignExtImm = "0000000000000000";
          }
          Immediate = INSTR.substr(16,16);
          SignExtImm += Immediate;
          Opr2 = bitset<32> (SignExtImm); 
        }

        else if (OPCODE == "000000")  // R type
        {
          WriteReg = bitset<5> (INSTR.substr(16,5)); // Destination register for R type insruction
          Opr2 = bitset<32> (ReadRegData2);
        }


        //to get the opcode
        if ((OPCODE == "101011") || (OPCODE == "100011")) //for load and store instructions, addition is to be done
        {
          ALUOpCode = bitset <3> (1);
        }

        else if (OPCODE == "000000")  // R type
        {
          ALUOpCode = bitset <3> (INSTR.substr(29,3));
        }

        else if (OPCODE != "101011" && OPCODE != "100011" && OPCODE != "000000" && OPCODE != "000010" && OPCODE != "000100") // for instructions which are not load, store, R type, jump or beq
        {
          ALUOpCode = bitset <3> (INSTR.substr(3,3));
        }

        Opr1  = bitset <32> (ReadRegData1); // It is always the read register 1
        
        // if it is a beq instruction
        if(OPCODE == "000100")
        {
          if (ReadRegData1 == ReadRegData2)
          {
            equal = 1;
            if (INSTR[16] == '1')   // sign extending immediate value
            {
              SignExtImm = "11111111111111";
            }
            else
            {
              SignExtImm = "00000000000000";
            }
            Immediate = INSTR.substr(16,16);
            SignExtImm += Immediate;
            SignExtImm += "00";
            bitset <32> SXM (SignExtImm);
            unsigned int SignExtImmediate = SXM.to_ulong();
            BranchAddr  = pc + SignExtImmediate;  //pc + 4 is already done
          }
        }


        // Execute
        ALUAns = myALU.ALUOperation (ALUOpCode.to_ulong(), Opr1.to_ulong(), Opr2.to_ulong());
        Result = bitset <32> (ALUAns);

        //Stage 1 Fetch
        //Fetch stage
	    Instr = myInsMem.ReadMemory(pc);
		cout<< pc << endl;
		instr = bitset <32> (Instr);
		// At the end of each cycle, fill in the corresponding data into "dumpResults" function to output files.
		// The first dumped pc value should be 0.
		dumpResults(instr, WriteReg, WrData, WriteEnable, ReadWrAddress, WriteDataMem, WriteDataMemEn);
		pc = pc + 4;
    }
    myRF.OutputRF(); // dump RF;
    myDataMem.OutputDataMem(); // dump data mem
    return 0;
}
