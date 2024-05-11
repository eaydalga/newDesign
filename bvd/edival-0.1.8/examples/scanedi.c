/*
 *  Copyright (C) 2007 Michael Edgar
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//gcc scanedi.c -std=c99 -I../lib -L../lib -lm -ledival -o scanedi

#include <edival.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFF_SIZE 16777216

void handleDocumentStart(void *myData, enum EDI_DocumentType type)
{
	//fprintf(stdout, "<document>\n");
}

void handleDocumentEnd(void *myData)
{
	//fprintf(stdout, "</document>\n");
}

void handleSegmentStart(void *myData, const char *tag)
{
	//fprintf(stdout, "\tSeg: %3s ->", tag);
}

void handleSegmentEnd(void *myData, const char *tag)
{
	//fprintf(stdout, " <- End %s\n", tag);
}

void handleCompositeStart(void *myData)
{
	//fprintf(stdout, "{");
}

void handleCompositeEnd(void *myData)
{
	//fprintf(stdout, "}");
}

void handleElement(void *myData, EDI_DataElement element)
{
	switch(element->type){
		case EDI_DATA_INTEGER:
		case EDI_DATA_BINARY_SIZE:
			fprintf(stdout, "[%d]", element->data.integer);
			break;
		case EDI_DATA_DECIMAL:
			fprintf(stdout, "[%Lf]", element->data.decimal);
			break;
		default:
			//fprintf(stdout, "[%s]", element->data.string);
			break;
	}
}

void handleJunk(void *myData, const char *val)
{
    fprintf(stderr, "Junk: >>{%s}<<\n", val);
}

int main(int argc, char **argv)
{
	int bytes_read, file_stat_size;
	double file_size = 0;
	double speed = 0;
	int input, i, length;
	void *buff = NULL;
	unsigned long j = 0;
	unsigned int size;
	struct timeval tv;
	double start_time, end_time, run_time;
	struct stat statbuf;
	EDI_Parser p = NULL;
	enum EDI_Status status;
	FILE *fin;

	if(argc < 2){
		fprintf(stderr, "Not enough arguments, exiting.\n");
		exit(1);
	}
	
	gettimeofday(&tv, NULL);
	start_time = tv.tv_sec + (tv.tv_usec * 0.000001);

	if((input = open(argv[1], O_RDONLY)) < 0){
		fprintf(stderr, "Couldn't open\n");
		exit(1);
	}
	if (fstat (input, &statbuf) < 0){
		fprintf(stderr, "Couldn't stat\n");
		exit(1);
	}
        
	p = EDI_ParserCreate();
	
	EDI_SetDocumentStartHandler(p, handleDocumentStart);
	EDI_SetDocumentEndHandler(p, handleDocumentEnd);
	EDI_SetSegmentStartHandler(p, handleSegmentStart);
	EDI_SetSegmentEndHandler(p, handleSegmentEnd);
	EDI_SetCompositeStartHandler(p, handleCompositeStart);
	EDI_SetCompositeEndHandler(p, handleCompositeEnd);
	EDI_SetElementHandler(p, handleElement);
	EDI_SetNonEDIDataHandler(p, handleJunk);
	EDI_SetUserData(p, p);

	buff = EDI_GetBuffer(p, BUFF_SIZE);
	length = read(input, buff, BUFF_SIZE);
	while(length > 0){
		EDI_Bool final = (length < BUFF_SIZE) ? EDI_TRUE : EDI_FALSE;
		status = EDI_ParseBuffer(p, length, final);
		if(status != EDI_STATUS_OK){
			fprintf(stderr, "Error: %d\n", EDI_GetErrorCode(p));
		}
		buff = EDI_GetBuffer(p, BUFF_SIZE);
		length = read(input, buff, BUFF_SIZE);
	}
	EDI_ParserFree(p);
	close(input);

	gettimeofday(&tv, NULL);
	end_time = tv.tv_sec + (tv.tv_usec * 0.000001);
	file_size = statbuf.st_size;
	fprintf(stderr, "Processed %d Bytes of data\n", (int)file_size);
	run_time = end_time - start_time;
	fprintf(stderr, "Runtime: %f seconds\n", run_time);
	speed = (file_size/1048576)/run_time;
	fprintf(stderr, "Average processing speed was %f MB/sec\n", speed);
	return 0;
}
