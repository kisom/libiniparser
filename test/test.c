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
#include <string.h>

#include <iniparser/iniparser.h>


/*
 * The use of the unlimited string functions (strcmp, etc...)  is deliberate in
 * the test. The test doesn't run in production, and it makes sure that the
 * library (which does use limited string functions) works as expected.
 */


/*
 * Return 1 if actual and expected match, 0 otherwise.
 */ 
static int
check_string(const char *expected, const char *actual)
{
	if (NULL == expected) {
		return (NULL == actual);
	}

	if (NULL == actual) {
		printf("actual is NULL");
		return 0;
	}

	return (0 == strcmp(expected, actual));
}


static int
check_line(int section, int set, const char *name, const char *value, int eof,
    iniparser_line_s *line, iniparser_file_s *file)
{
	int	ret = 0;

	ret = iniparser_readline(file, line);
	if (eof) {
		if (1 != ret) {
			fprintf(stderr,
			    "expected EOF, but didn't see it (saw %d)\n", ret);
			ret = 0;
			goto exit;
		}
	}
	else if (0 != ret) {
		fprintf(stderr,
		    "Expected line, but didn't see one. (ret=%d)\n", ret);
		ret = 0;
		goto exit;
	}

	ret = 0;
	if (section != line->is_section) {
		goto exit;
	}

	if (set != line->is_set) {
		goto exit;
	}
	else if (0 == set) {
		ret = 1;
		goto exit;
	}

	if (!check_string(name, line->name)) {
		goto exit;
	}

	if (!check_string(value, line->value)) {
		goto exit;
	}

	ret = 1;

exit:
	if (!ret) {
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line->is_section, line->is_set, line->name, line->value);
	}
	return ret;
}

static int
test_wikipedia_sample(void)
{
	iniparser_file_s	*file = NULL;
	iniparser_line_s	 line;
	int			 ret = -1;

	iniparser_line_init(&line);
	
	ret = iniparser_open("testdata/imaginary.ini", &file);
	if (0 != ret) {
		ret = iniparser_open("test/testdata/imaginary.ini", &file);
		if (0 != ret) {
			fprintf(stderr, "Failed to open test file.\n");
			perror("iniparser_open");
			return 0;
		}
	}

	if (!check_line(1, 1, "owner", NULL, 0, &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	if (!check_line(0, 1, "name", "John Doe", 0, &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	if (!check_line(0, 1, "organization", "Acme Widgets Inc.", 0,
	    &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	if (!check_line(1, 1, "database", NULL, 0, &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	if (!check_line(0, 1, "server", "192.0.2.62", 0, &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	if (!check_line(0, 1, "port", "143", 0, &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	if (!check_line(0, 1, "file", "\"payroll.dat\"", 0, &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	if (!check_line(0, 0, NULL, NULL, 1, &line, file)) {
		fprintf(stderr, "Line doesn't match.\n");
		fprintf(stderr, "Expected {%d, %d, %s, NULL}.\n",
		    1, 1, "owner");
		fprintf(stderr, "Actual {%d, %d, %s, %s}.\n",
		    line.is_section, line.is_set, line.name, line.value);
		goto exit;
	}

	ret = 1;
exit:
	if (0 != iniparser_close(file)) {
		fprintf(stderr, "Failed to close test file.\n");
	}
	
	iniparser_line_destroy(&line);
	return ret == 1;
}


int
main(void)
{
	if (0 != iniparser_init()) {
		fprintf(stderr, "[!] Failed to initialise parser.\n");
		perror("iniparser_init");
		return EXIT_FAILURE;
	}

	if (!(test_wikipedia_sample())) {
		fprintf(stderr, "Wikipedia sample failed.\n");
		perror("test_wikipedia_sample");
		return EXIT_FAILURE;
	}
	
	return 0;
}

