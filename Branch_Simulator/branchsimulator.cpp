/*
 * branchSimulator.cpp
 *
 *  Created on: Dec 19, 2017
 *      Author: Monica
 */


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
#include <bitset>
#include <cstring>

using namespace std;

//this struct has the values of k and LSBs(m)
struct config
{
    int LSBs;
    int k;
};


int prediction=0, misprediction=0, correct_prediction=0, total =0;
int PHT[65534][20];
char m[19], k[19];

//function to read from config file
void readFromConfigFile(char* fileName)
{
    FILE * pFile;
    int flag=1;

        pFile = fopen (fileName , "r");
        if (pFile == NULL) perror ("Error opening file");
        else 
		{
            while(!feof(pFile)) 
			{
                if ( (fgets (m , 19 , pFile) != NULL) && flag==1)   // taking the value of m
                {
                    flag=0;
                }
                if ( (fgets (k , 19 , pFile) != NULL) && flag==0)   // taking the value of k
                {
                    flag=2;
                }
            }
			fclose (pFile);
        }
}


void updatePHT(unsigned long col, unsigned long row, int TNT_bit)   // updating the pattern history table
{

   if (TNT_bit == 0){               // if current branch is not taken

        if (PHT[row][col]==0)   //strongly not taken
        {
            PHT[row][col]=0;    // 00 becomes 00
            correct_prediction++;
            prediction = 0;
        }
        else if(PHT[row][col]==1)   // weakly not taken
            {
            PHT[row][col]=0;    // 01 becomes 00
            correct_prediction++;
            prediction = 0;
        }
        else if(PHT[row][col]==2)   // weakly taken
        {
            PHT[row][col]=0;    // 10 becomes 00
            misprediction++;
            prediction = 1;
        }
        else if (PHT[row][col]==3)  // stringly taken
        {
            PHT[row][col]=2;    // 11 becomes 10
            misprediction++;
            prediction = 1;
        }
    }
    else    // if current branch is taken
    {
        if (PHT[row][col]==0)   // strongly not taken
        {
            PHT[row][col]=1;    // 00 becomes 01
            misprediction++;
            prediction = 0;
        }
        else if(PHT[row][col]==1)   // weakly not taken
        {
            PHT[row][col]=3;    // 01 becomes 11
            misprediction++;
            prediction = 0;
        }
        else if(PHT[row][col]==2)   // weakly taken
        {
            PHT[row][col]=3;    // 10 becomes 11
            correct_prediction++;
            prediction = 1;
        }
        else if(PHT[row][col]==3)   // strongly taken
        {
            PHT[row][col]=3;    // 11 becomes 11
            correct_prediction++;
            prediction = 1;
        }
    }
}

int main(int argc, char* argv[])
{
    config branchconfig;
    ifstream branch_params;
    FILE * traceFILE;
    branch_params.open(argv[1]);
    char * getline;

    readFromConfigFile(argv[1]);
    branchconfig.LSBs = atoi(m);    // storing the value in struct
    branchconfig.k = atoi(k);   // storing the value in struct
    long x = pow(2,branchconfig.LSBs);  // these are the number of rows off 2D matrix
    int y = pow(2,branchconfig.k);  // these are the number of columns of 2D matrix

    // to initialize the value of all saturation counters to 11 (3)
    for(int l =0;l<x;l++)
    {
        for(int m = 0; m<y;m++)
        {
            PHT[l][m]=3;       // ititialize to STRONGLY TAKEN
        }
    }

    ifstream traces;
    ofstream tracesout;
    string outname;
    outname = string(argv[2]) + ".out";
    traces.open(argv[2]);
    tracesout.open(outname.c_str());

    int m = 32-branchconfig.LSBs; // to get the start value of LSBs
    int bit;
    string hexa;
    unsigned long int integer_addr;
    bitset<32> bit_addr;
    bitset<32> index;
    int bit_buffer= 1, option;

    // this is to initialize the value of the BHR according to the value of k
    switch (branchconfig.k)
    {
        case 0 :
        option = 0;
        break;

        case 1:
        option = 1;
        break;

        case 2:
        option = 3;
        break;

        case 3:
        option = 7;
        break;

        case 4:
        option = 15;
        break;

        default:
        cout << "k value out of range";

    }

    FILE * pFile;
    char *pch;
    bit= 1;
    char line[12];

    pFile = fopen (argv[2] , "r");
    if (pFile == NULL) perror ("Error opening file");
    else 
	{
        while(!feof(pFile)) 
		{
            if ( (fgets (line ,12 , pFile) != NULL))
            {
                total ++;
                pch = strtok(line," ");        // taking address
                integer_addr= std::strtoul(pch, 0, 16);

                while (pch != NULL)
                {
                pch = strtok (NULL, " ");       // taking bit
                if (pch == NULL)
                    break;
                bit = std::strtoul(pch, 0, 16);;
                }

                bit_addr = bitset <32> (integer_addr);
                index = (bitset<32>((bit_addr.to_string().substr(m,31))).to_ulong());
                int array_row_index = index.to_ulong();

                switch (branchconfig.k)
                {
                    case 0:
                    option = 0;
                    break;

                    case 1:
                    option = bit_buffer;  // T/NT bit of previous branch
                    bit_buffer = bit; // store current T/NT bit to use it in the next instruction
                    break;

                    case 2:
                    if (bit_buffer == 1)  // if previous T/NT bit is 1
                        option = (option>>1)| 2 ; // first it is right shifted by 1 to remove LSB then it is ORed with 10 to add the previous T/NT bit to the MSB
                    else
                        option = (option>>1)| 0 ; // if previous T/NT bit was 0 then there it is right shifted and 00 is ORed to append 0 to MSB position
                    bit_buffer = bit; // store current T/NT bit to use it in the next instruction
                    break;

                    case 3:
                    if (bit_buffer == 1)
                        option = (option>>1)| 4 ; // first it is right shifted by 1 to remove LSB then it is ORed with 100 to add the previous T/NT bit to the MSB
                    else
                        option = (option>>1)| 0 ; // if previous T/NT bit was 0 then there it is right shifted and 000 is ORed to append 0 to MSB position
                    bit_buffer = bit; // store current T/NT bit to use it in the next instruction

                    break;


                    case 4:
                    if (bit_buffer == 1)
                        option = (option>>1)| 8 ; // first it is right shifted by 1 to remove LSB then it is ORed with 1000 to add the previous T/NT bit to the MSB
                    else
                        option = (option>>1)| 0 ; // if previous T/NT bit was 0 then there it is right shifted and 0000 is ORed to append 0 to MSB position
                    bit_buffer = bit; // store current T/NT bit to use it in the next instruction

                    break;

                    default:
                    cout << "k value not between 0 and 4";
                    break;
                }

                updatePHT(option, array_row_index, bit); // giving the column number, row number and the current T/NT bit

	            tracesout<<prediction<<endl;
            }

        }
        cout << "Mispredicted branches: " << misprediction << "\n";
        cout << "Correctly predicted branches: " << correct_prediction << "\n";
        cout<< "total" << total << "\n";
        float rate = (float(misprediction) / float(total)) * 100 ;
        cout << "Total number of branches: " << rate << " \n";
        fclose (pFile);
    }
return 0;
}


