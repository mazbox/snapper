#include <snappy.h>
#include <stdio.h>
#include <string>
using namespace std;



bool readFile(string path, int &inSize, char **data) {
	// read file in
	FILE *f = fopen(path.c_str(), "rb");
	if(f==NULL) {
		printf("Couldn't open file '%s' for reading\n", path.c_str());
		return false;
	}

	fseek(f, 0, SEEK_END);
	inSize = ftell(f);
	rewind(f);
	*data = new char[inSize];
	if(fread(*data, 1, inSize, f)!=inSize) {
		fclose(f);
		delete [] *data;
		printf("Error reading in file\n");
		return false;
	}
	fclose(f);
	return true;
}


bool writeFile(string path, char *data, int length) {
	FILE *f = fopen(path.c_str(), "wb");
	if(f==NULL) {
		printf("Couldn't open file '%s' for writing\n", path.c_str());
		return false;
	}	
	fwrite(data, 1, length, f);
	fclose(f);
	return true;
}


int main(int argc, char **argv) {
	if(argc!=4) {
		printf("Usage: %s [-x/-c] [infile] [outfile]\n\n\t-x  Extract\n\n\t-c  Compress\n\n", argv[0]);
		return 1;
	}
	string choice = argv[1];

	if(choice!="-x" && choice!="-c") {
		printf("First parameter should be -c or -x depending on whether you want to compress or extract\n");
		return 1;
	}
	int inSize = 0;
	char *buffer = NULL;
	if(!readFile(argv[2], inSize, &buffer)) {
		return 1;
	}

	size_t outSize = 0;
	char* outBuff = NULL;

	if(choice=="-c") {
		
		outBuff = new char[snappy::MaxCompressedLength(inSize)];

		snappy::RawCompress(buffer,
					inSize,
					outBuff,
					&outSize);
						
		
		printf("Compressed to %.2f%% of original size\n", (float)outSize*100.f/(float)inSize);
		
		

	} else {
		if(!snappy::GetUncompressedLength(buffer, inSize, &outSize)) {
			printf("Error reading compressed file\n");

           return 1;
        }

        outBuff = new char[outSize];
        if(!snappy::RawUncompress(buffer, inSize, outBuff)) {
        	printf("Error decompressing file\n");
        	return 1;
        }
        printf("Decompressed to %.2f%% of compressed size\n", (float)outSize*100.f/(float)inSize);
	}

	if(!writeFile(argv[3], outBuff, outSize)) {
		return 1;
	}

		
	delete [] outBuff;
	delete [] buffer;
	
	return 0;
}