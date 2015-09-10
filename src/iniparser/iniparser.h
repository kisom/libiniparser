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

#ifndef __LIBINIPARSER_INIPARSER_H
#define __LIBINIPARSER_INIPARSER_H


typedef struct {
	FILE	*source;
	char	*lineptr;
	size_t	 linelen;
	ssize_t	 readlen;
} iniparser_file_s;

typedef struct {
	uint8_t	 is_section;
	uint8_t	 is_set;
	char	*name;
	char	*value;
} iniparser_line_s;


int	iniparser_init(void);
void	iniparser_destroy(void);
int	iniparser_open(const char *, iniparser_file_s **);
int	iniparser_close(iniparser_file_s *);
int	iniparser_readline(iniparser_file_s *, iniparser_line_s *);
void	iniparser_line_init(iniparser_line_s *);
void	iniparser_line_destroy(iniparser_line_s *);


#endif
