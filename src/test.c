/*
 * Copyright (c) 2015 Kyle Isom <kyle@tyrfingr.is>
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "iniparser/iniparser.h"


int
main(int argc, char *argv[])
{
	iniparser_file_s	*file = NULL;
	iniparser_line_s	 line;
	int			 i;
	int			 ret;

	ret = iniparser_init();
	if (0 != ret) {
		fprintf(stderr, "init failed: %d\n", ret);
		goto exit;
	}

	argc--;
	argv++;

	for (i = 0; i < argc; i++) {
		printf("Processing %s\n", argv[i]);
		ret = iniparser_open(argv[i], &file);
		if (0 != ret) {
			perror("_open");
			fprintf(stderr, "retval: %d\n", ret);
			goto exit;
		}
		iniparser_line_init(&line);

		while (1) {
			ret = iniparser_readline(file, &line);
			/* -1 is returned on error. */
			if (-1 == ret) {
				perror("_readline");
				fprintf(stderr, "retval: %d\n", ret);
				goto exit;
			}
			/* 1 means EOF. */
			else if (1 == ret) {
				ret = 0;
				break;
			}

			if (line.is_section) {
				printf("Now in section '%s'\n", line.name);
			}
			else {
				printf("Read key '%s' with value '%s'\n",
				    line.name, line.value);
			}

			iniparser_line_destroy(&line);		
		}

		iniparser_close(file);
		free(file);
		file = NULL;
		iniparser_line_destroy(&line);		
	}

exit:
	iniparser_line_destroy(&line);	
	if (argc > 0) {
		iniparser_destroy();
	}

	return ret==0;
}
