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


#include <sys/types.h>

#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iniparser/iniparser.h"


#define SECTION_STRING	"^\\[ *([a-zA-Z0-9_-]+) *\\]$"
#define SKIPLINE_STRING	"^[ \t]*$"
#define COMMENT_STRING	"^[ \t]*[#;]"
#define KEYVALUE_STRING	"^[ \t]*([a-zA-Z0-9_-]+)[ \t]*=(.*)$"


static regex_t	section_regex;
static regex_t	skipline_regex;
static regex_t	comment_regex;
static regex_t	keyvalue_regex;



void
iniparser_line_init(iniparser_line_s *line)
{
	line->is_section = 0;
	line->name = NULL;
	line->value = NULL;
}


void
iniparser_line_destroy(iniparser_line_s *line)
{
	free(line->name);
	free(line->value);
	line->name = NULL;
	line->value = NULL;
	line->is_section = 0;
	line->is_set = 0;
}


int
iniparser_open(const char *path, iniparser_file_s **file)
{
	iniparser_file_s	*f = NULL;
	int			 allocated = 0;
	int			 ret = -1;
	
	/*
	 * If file is NULL, we can allocate space for it.
	 */
	if (NULL == file) {
		goto exit;
	}

	f = *file;
	if (NULL == f) {
		if (NULL == (f = calloc(1, sizeof(*f)))) {
			return -1;
		}
		allocated = 1;
	}

	f->source = fopen(path, "r");
	if (NULL == f->source) {
		goto exit;
	}

	f->lineptr = NULL;
	f->linelen = 0;
	f->readlen = 0;
	*file = f;
	ret = 0;

exit:
	if (allocated && ret) {
		free(f);
		*file = NULL;
	}

	return ret;
}


int
iniparser_close(iniparser_file_s *file)
{
	int	ret;
	
	if (NULL != file->lineptr) {
		free(file->lineptr);
		file->lineptr = NULL;
	}

	file->linelen = 0;
	file->readlen = 0;
	ret = fclose(file->source);
	file->source = NULL;
	return ret;
}

static char *
extract_match(const char *line, regmatch_t *pmatch)
{
	ssize_t		 len;
	regoff_t	 eo = pmatch->rm_eo;
	regoff_t	 so = pmatch->rm_so;
	char		*s = NULL;

	/* Technically, these are signed. They should be checked. */
	if ((pmatch->rm_eo < 0) || (pmatch->rm_eo < 0)) {
		return NULL;
	}

	/* trim space from the end. */
	while ((eo > pmatch->rm_so) && (0x20 == line[eo-1])) {
		eo--;
	}

	/* trim space from the beginning. */
	while ((so < eo) && (0x20 == line[so])) {
		so++;
	}
	
	len = eo - so;
	
	if (len <= 0) {
		return NULL;
	}
	
	if (NULL == (s = calloc((size_t)len+1, sizeof(*s)))) {
		return NULL;
	}

	memcpy(s, line + so, (size_t)len);
	return s;
}


int
iniparser_readline(iniparser_file_s *file, iniparser_line_s *line)
{
	/*
	 * The longest matching regex has two matches.
	 */
	regmatch_t	pmatch[3];

	iniparser_line_destroy(line);
	if (NULL != file->lineptr) {
		free(file->lineptr);
		file->lineptr = NULL;
		file->linelen = 0;
		file->readlen = 0;
	}

	file->readlen = getline(&(file->lineptr), &(file->linelen),
	    file->source);
	if (file->readlen < 1) {
		if (0 != feof(file->source)) {
			return 1;
		}

		return -1;
	}

	if (0xa == file->lineptr[file->readlen-1]) {
		file->lineptr[file->readlen-1] = 0;
	}

	if (REG_NOMATCH != regexec(&section_regex, file->lineptr, 2, pmatch, 0)) {
		line->is_section = 1;
		line->name = extract_match(file->lineptr, &pmatch[1]);
		if (NULL == line->name) {
			return -1;
		}
		line->is_set = 1;
		return 0;
	}

	/* If a skipline or comment is detected, skip to the next line. */
	if (REG_NOMATCH != regexec(&skipline_regex, file->lineptr,
	    0, NULL, 0)) {
		return iniparser_readline(file, line);
	}

	if (REG_NOMATCH != regexec(&comment_regex, file->lineptr,
	    0, NULL, 0)) {

		return iniparser_readline(file, line);
	}

	/* Try to parse a key-value pair. */
	if (REG_NOMATCH != regexec(&keyvalue_regex, file->lineptr,
	    3, pmatch, 0)) {
		line->is_section = 0;
		line->name = extract_match(file->lineptr, &pmatch[1]);
		line->value = extract_match(file->lineptr, &pmatch[2]);
		if (NULL == line->name) {
			return -1;
		}
		line->is_set = 1;
		return 0;
	}

	/* Reaching this point means there's an invalid line in the file. */
	return -1;
}


void
iniparser_destroy()
{
	regfree(&section_regex);
	regfree(&skipline_regex);
	regfree(&comment_regex);
	regfree(&keyvalue_regex);
}


int
iniparser_init()
{
	int	ret = -1;

	ret = regcomp(&section_regex, SECTION_STRING, REG_EXTENDED);
	if (0 != ret) {
		goto exit;
	}

	ret = regcomp(&skipline_regex, SKIPLINE_STRING, REG_NOSUB|REG_EXTENDED);
	if (0 != ret) {
		goto exit;
	}

	ret = regcomp(&comment_regex, COMMENT_STRING, REG_NOSUB|REG_EXTENDED);
	if (0 != ret) {
		goto exit;
	}

	ret = regcomp(&keyvalue_regex, KEYVALUE_STRING, REG_EXTENDED);
	if (0 != ret) {
		goto exit;
	}

	ret = 0;

exit:
	if (ret) {
		iniparser_destroy();
	}

	return ret;
}
