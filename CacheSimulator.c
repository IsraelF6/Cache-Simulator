#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct Cache
{
   int Tag;
   int Valid;
   int Dirty;
   int LRU;
} myStruct;


int main(int argc, char *argv[])
{  
  int numSets;
  int blockSize;
  int associativity;
  int offsetBits;
  int indexBits;
  int tagBits;
  char type[5];
  char address[10];
  
  int temp[5];				// store command line arguments

  // argv[odd] = "-s" or "-b" or "-n"
  // argv[even] = numerical values
  temp[0] = atoi(argv[2]);
  temp[2] = atoi(argv[4]);
  temp[4] = atoi(argv[6]);

  int i;
  for(i=1; i<argc; i++)
  {
    if (strcmp(argv[i],"-s") == 0)
    {
      numSets = temp[i-1]; 
    }
    else if (strcmp(argv[i],"-b") == 0)
    {
      blockSize = temp[i-1];
    }
    else if (strcmp(argv[i],"-n") == 0)
    {
      associativity = temp[i-1];
    }
    else
    {
      //do nothing
    }
  }
  
  offsetBits = log2(blockSize);
  indexBits = log2(numSets);
  tagBits = 32 - offsetBits - indexBits;


  printf("Block size: %d\n", blockSize);
  printf("Number of sets: %d\n", numSets);
  printf("Associativity: %d\n", associativity);
  printf("Number of offset bits: %d\n", offsetBits);
  printf("Number of index bits: %d\n", indexBits);
  printf("number of tag bits: %d\n\n", tagBits);
  
 

  myStruct WriteThrough[numSets][associativity];
  myStruct WriteBack[numSets][associativity];
  char linearray[50];


  int val;
  int k;
  int j;
  int temp1, temp2;
  temp1 = indexBits + offsetBits;
  temp2 = offsetBits;
 

  int wtHits = 0;				// null the cache and other data
  int wtMisses = 0;
  int wtMemref = 0;
  int wtLRU = 0;
  int wbHits = 0;
  int wbMisses = 0;
  int wbMemref = 0;
  int wbLRU = 0;
  
  for (i=0; i<numSets; i++)
  {
	  int j;
	  for (j=0; j<associativity; j++)
	  {
		  WriteBack[i][j].Valid = 0;
		  WriteBack[i][j].Dirty = 0;
		  WriteThrough[i][j].Valid = 0;			// null cache
	  }
  }
  
  int totalRef=0;
  while(fgets(linearray,50,stdin) != NULL)
  {
    totalRef++; 
   
    if (totalRef==50)
      break;
    
    sscanf(linearray,"%[^ ] %[^\n]",type,address);
    val = atoi(address);

    for (j=0; val>=pow(2,temp1); j++)		// j = tag
    {
      val = val - pow(2,temp1);			// 2^(index+offset)
    }
 
    for (k=0; val>=pow(2,temp2); k++)		// k = index, val = offset
    {
      val = val - pow(2,temp2);			// 2^(offset)
    }

	
	int Index = k;
	int Tag = j;
	int LeastLRU;
	int oldestBlock;
	int inCache;
	int blockFound;
	
    //printf("Index: %d\n", k);     
    //printf("Tag : %d\n", j);

    /* reading from cache */
    if (sscanf(linearray,"R %[^\n]",address) == 1)
    {  
		
		inCache = 0;
		//Write-Through
		for (i=0; i<associativity; i++)
		{
		if (WriteThrough[Index][i].Valid == 0)			// check for empty slot in specified index
			{
				myStruct temp;
				
				temp.Tag = Tag;
				temp.LRU = wtLRU++;
				temp.Valid = 1;
				WriteThrough[Index][i] = temp;
				wtMisses++;
				wtMemref++;
				inCache = 1;
				break;							// reference has been placed in cache
			}
			
			// already in cache
			else if ((WriteThrough[Index][i].Tag == Tag) && (WriteThrough[Index][i].Valid == 1))
			{
				//printf("/????????????");
				WriteThrough[Index][i].LRU = wtLRU++;
				wtHits++;
				inCache = 1;	
				break;
			}
		}
		
		if (inCache == 0)					// not in cache and no empty slots
		{
			LeastLRU = WriteThrough[Index][0].LRU;
			oldestBlock = 0;
			for (i=0; i<associativity; i++)
			{
				if (WriteThrough[Index][i].LRU < LeastLRU)
				{
					LeastLRU = WriteThrough[Index][i].LRU;
					oldestBlock = i;
				}
			}
			
			myStruct temp;
			
			temp.Tag= Tag;
			temp.LRU = wtLRU++;
			temp.Valid = 1;
			WriteThrough[Index][oldestBlock] = temp;
			wtMisses++;
			wtMemref++;
			inCache = 1;
			}
			
			
			
		inCache = 0;
		// Write-Back
		for (i=0; i<associativity; i++)
		{
			if ((WriteBack[Index][i].Tag == Tag) && (WriteBack[Index][i].Valid == 1))
			{
				WriteBack[Index][i].LRU = wbLRU++;
				wbHits++;
				inCache = 1;
				break;
			}
		}
		
		if (inCache == 0)
		{
			blockFound = 0;
			for (i=0; i<associativity; i++)
			{
				if (WriteBack[Index][i].Valid == 0)
				{
					myStruct temp;
					
					temp.Tag = Tag;
					temp.LRU = wbLRU++;
					temp.Valid = 1;
					temp.Dirty = 0;
					WriteBack[Index][i] = temp;
					wbMisses++;
					wbMemref++;
					blockFound = 1;
					break;
				}
			}
			
			if (blockFound == 0)
			{
				LeastLRU = WriteBack[Index][0].LRU;
				oldestBlock = 0;
				for (i=0; i<associativity; i++)
				{
					if (WriteBack[Index][i].LRU < LeastLRU)
					{
						LeastLRU = WriteBack[Index][i].LRU;
						oldestBlock = i;
					}
				}
				
				if (WriteBack[Index][oldestBlock].Dirty = 1)
				{
					wbMemref++;
				}
			
				myStruct temp;
			
				temp.Tag= Tag;
				temp.LRU = wbLRU++;
				temp.Valid = 1;
				temp.Dirty = 0;
				WriteBack[Index][oldestBlock] = temp;
				wbMisses++;
				wbMemref++;
				blockFound = 1;
			}
		}
		
    }

    /* writing to cache */
    else if (sscanf(linearray,"W %[^\n]",address) == 1)
    {
		inCache = 0;
		//Write-Through
		for (i=0; i<associativity; i++)
		{
			if ((WriteThrough[Index][i].Tag = Tag) && (WriteThrough[Index][i].Valid == 1))
			{
				WriteThrough[Index][i].LRU = wtLRU++;
				wtHits++;
				wtMemref++;
				inCache = 1;
				break;
			}
		}
	  
		if (inCache == 0)
		{
			wtMisses++;
			wtMemref++;
		}
	  
		//Write-Back
		{
			inCache = 0;
			for (i=0; i<associativity; i++)
			{
				if ((WriteBack[Index][i].Tag == Tag) && (WriteBack[Index][i].Valid == 1))
				{
					WriteBack[Index][i].LRU = wbLRU++;
					WriteBack[Index][i].Dirty = 1;
					wbHits++;
					inCache = 1;
				}
			}
			
			if (inCache == 0)
			{
				blockFound = 0;
				for (i=0; i<associativity; i++)
				{
					if (WriteBack[Index][i].Valid == 0)
					{
						myStruct temp;
						
						temp.Tag = Tag;
						temp.LRU = wbLRU++;
						temp.Valid = 1;
						temp.Dirty = 1;
						WriteBack[Index][i] = temp;
						wbMisses++;
						wbMemref++;
						blockFound = 1;
						break;
					}
				}
				
				if (blockFound == 0)
				{
					LeastLRU = WriteBack[Index][0].LRU;
					oldestBlock = 0;
					for (i=0; i<associativity; i++)
					{
						if (WriteBack[Index][i].LRU < LeastLRU)
						{
							LeastLRU = WriteBack[Index][i].LRU;
							oldestBlock = i;
						}
					}
				
					if (WriteBack[Index][oldestBlock].Dirty == 1)
					{
						wbMemref++;
					}
				
					myStruct temp;
					
					temp.Tag = Tag;
					temp.LRU = wbLRU++;
					temp.Valid = 1;
					temp.Dirty = 1;
					WriteBack[Index][oldestBlock] = temp;
					wbMisses++;
					wbMemref++;
				}
			}
		}
	}

  }

	printf("***************************************\n");
    printf("Write-Through with No Write Allocate\n");
    printf("***************************************\n");
    printf("Total # of references: %d\n",totalRef);
    printf("Hits: %d\n",wtHits);
    printf("Misses: %d\n",wtMisses);
    printf("Memory References: %d\n",wtMemref);
	
	printf("***************************************\n");
    printf("Write-Back with No Write Allocate\n");
    printf("***************************************\n");
    printf("Total # of references: %d\n",totalRef);
    printf("Hits: %d\n",wbHits);
    printf("Misses: %d\n",wbMisses);
    printf("Memory References: %d\n",wbMemref);
  
  }

