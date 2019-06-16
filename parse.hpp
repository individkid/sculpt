/*
*    parse.hpp function for converting string to Command
*    Copyright (C) 2019  Paul Coelho
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PARSE_HPP
#define PARSE_HPP

#include "pools.hpp"

class Parse : public Pools
{
public:
	Parse(const char *file, int line) : Pools(file,line) {}
	int get(const char *&ptr, Update *&update);
	int get(const char *&ptr, Render *&render);
	int get(const char *&ptr, int file, Command *&command);
	int get(const char *&ptr, int file, Sound *&sound);
	int get(const char *&ptr, int file, Query *&query);
	int get(const char *ptr, int file, Command *&command, Data *&window,
		Query *&query, Data *&polytope, Sound *&sound, Data *&system, Data *&script);
	char *concat(const char *left, const char *right) {return ::concat(chars,left,right);}
	char *concat(char *left, const char *right) {return ::concat(chars,left,right);}
	char *concat(const char *left, char *right) {return ::concat(chars,left,right);}
	char *concat(char *left, char *right) {return ::concat(chars,left,right);}
	char *concat(const char *left, char right) {return ::concat(chars,left,right);}
	char *concat(char *left, char right) {return ::concat(chars,left,right);}
	char *concat(char left, const char *right) {return ::concat(chars,left,right);}
	char *concat(char left, char *right) {return ::concat(chars,left,right);}
	char *prefix(const char *str, int len) {return ::prefix(chars,str,len);}
	char *prefix(char *str, int len) {return ::prefix(chars,str,len);}
	char *postfix(const char *str, int len) {return ::postfix(chars,str,len);}
	char *postfix(char *str, int len) {return ::postfix(chars,str,len);}
	char *setup(const char *str) {return ::setup(chars,str);}
	char *setup(char *str) {return ::setup(chars,str);}
	char *setup(char chr) {return ::setup(chars,chr);}
	char *setup(int len) {char *str = chars.get(len+1); for (int i = 0; i < len+1; i++) str[i] = 0; return str;}
	const char *cleanup(char *str) {return ::cleanup(chars,str);}
	int identifier(const char *&str, int &val);
	int number(const char *&str, int &val);
	int scalar(const char *&str, float &val);
	int literal(const char *&str, const char *pat);
	int text(const char *&str, char *&val);
	int word(const char *&str, char *&txt);
	int identifiers(const char *&str, int siz, int *&val);
	int numbers(const char *&str, int siz, int *&val);
	int scalars(const char *&str, int siz, float *&val);
	int alloc(Data *&ptr);
	int deloc(int siz, int *val);
	int deloc(int siz, float *val);
	int deloc(char *val);
	int deloc(Data *ptr);
};

#endif
