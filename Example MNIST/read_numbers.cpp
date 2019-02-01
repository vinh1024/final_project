#include <stdio.h>
#include <string.h>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "byteswap.h"
#include "CNN/cnn.h"

using namespace std;

struct case_t
{
	tensor_t<float> data;
	tensor_t<float> out;
};

vector<case_t> read_data_label(char *PATH, int num_of_case, int num_in_person, int weight, int height, int num_of_person)
{
	vector<case_t> cases;

	case_t c {tensor_t<float>( height, weight, 1 ), tensor_t<float>( num_of_person, 1, 1 )};

	for (int i = 0; i < num_of_case; i++)
	{
		for (int j = 0; j < num_in_person; j++)
		{
			FILE *myFile;
			char *name;
			name = strcat((char*)i, "_");
			name = strcat(name, (char*)j);
            PATH = strcat(PATH, name);
			myFile = fopen(PATH, "r");
			float *p;

			for (int h = 0; h < height; h++)
			{
				for (int w = 0; w < weight; w++)
				{
					fscanf(myFile, "%f", p);
					c.data(height, weight, 0) = *p;
				}
			}
			for (int l = 0; l < num_of_case; l++)
			{
				c.out( l, 0, 0 ) = (int)name[0] == l ? 1.0f : 0.0f;
			}
		}
	}
	return cases;
}

int main()
{

    /*FILE *myFile;
    myFile = fopen("numbers.txt", "r");

    float numberArray[5];
    int i;

    for (i = 0; i < 5; i++)
    {
        fscanf(myFile, "%f", &numberArray[i]);
    }

    for (i = 0; i < 5; i++)
    {
        printf("Number is: %f\n\n", numberArray[i]);
    }
    fclose(myFile);
    */
    const char path[] = "Data";



    read_data_label(path, int num_of_case, int num_in_person, int weight, int height, int num_of_person)
    
    return 0;

}