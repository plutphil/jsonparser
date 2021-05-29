#pragma once
#include<string>
#include<fstream>
#include <iostream>
#include<sstream>
#include<map>
#include<stack>
#include<vector>
using namespace std;
const int maxdepth= 30;

string strmult(int i,string str="  ") {
	string out = "";
	for (size_t i = 0; i < i; i++)
	{
		out += str;
	}
	return out;
}
class Object {
public:
	virtual string toStringSave(int depth = 1){
		return string("OBJ");
	}
	virtual Object* operator [](int i) {
		return NULL;
	}
	virtual Object* operator [](string str) {
		return NULL;
	}
};
class MapEntry :public Object {
public:
	string key="";
	Object* obj=NULL;
	MapEntry(string key) {
		this->key = key;
	}
	string toStringSave(int depth = 1) {
		if (depth > maxdepth)return "TOODEEP";
		if (obj == NULL)return "NULL";
		return ""+obj->toStringSave(depth+1)+"";
	}
};
class CurlyBracket :public Object{
public:
	vector<MapEntry*> entries;
	map<string,MapEntry*> mymap;
	string toStringSave(int depth = 1) {
		if (depth > maxdepth)return "TOODEEP";
		string out="{";
		for (auto e : entries) {
			out += "\""+e->key+"\"";
			out += ":";
			out += e->obj->toStringSave(depth + 1)+",";
		}
		if(!entries.empty())
			out = out.substr(0, out.length() - 1);
		return out+"}";
	}
	Object* operator [](int i) {
		return 0;
	}
	Object* operator [](string str) {
		if (mymap.count(str)) {
			return mymap[str];
		}
		return NULL;
	}
}; 
class ArrayBracket :public Object {
public:
	vector<Object*> entries;
	string toStringSave(int depth = 1) {
		if (depth > maxdepth)return "TOODEEP";
		string out = "[";
		for (auto e : entries) {
			
			out += e->toStringSave(depth + 1) + ",";
		}
		if (!entries.empty())
			out = out.substr(0, out.length() - 1);
		return out + "]";
	}
	Object* operator [](int i) {
		if (i >= 0 && i < entries.size()) {
			return entries[i];
		}
		return NULL;
	}
};
class Value :public Object {

};
class String :public Value {
public:
	string val;
	String(string val) {
		this->val = val;
	}
	string toStringSave(int depth = 1) {

		return "\"" + val + "\"";
	}
}; 
class Boolean :public Value {
public:
	bool val;
	Boolean(bool val) {
		this->val = val;
	}
	string toStringSave(int depth = 1) {
		return  val ?"true":"false";
	}
};
class Number :public Value {
public:
	double val;
	Number(double val) {
		this->val = val;
	}
	string toStringSave(int depth = 1) {
		return to_string(val);
	}
};
class JSON {
public:
	JSON() {

	}
	JSON(string text) {
		feed(text);
	}
	void feed(string text) {
		for (size_t i = 0; i < text.length(); i++)
		{
			feedchar(text[i]);
		}
		if(root!=NULL)
		printf("%s\n", root->toStringSave(0).c_str());
		while (!obstack.empty()) {
			//printf("%s\n", obstack.top()->toStringSave(0).c_str());
			obstack.pop();
		}
	}
	Object* root=NULL;
	stack<Object*> obstack;
	//stack<Object*> brstack;
	void add(Object* o) {
		if (root == NULL) {
			root = o;
		}
		if (obstack.empty()) {
			obstack.push(o);
			return;
		}
		if (auto cb = dynamic_cast<CurlyBracket*>(obstack.top())) {
			if (auto str = dynamic_cast<String*>(o)) {
				auto ph = new MapEntry(str->val);
				push(ph);
				cb->entries.push_back(ph);
				cb->mymap[str->val] =ph;
			}
			else {
				return;
			}
			
		}
		else if (auto ph = dynamic_cast<MapEntry*>(obstack.top())) {
			pop();
			ph->obj=o;
			if (auto cb = dynamic_cast<CurlyBracket*>(o)) {
				push(cb);
			}
			else if (auto ab = dynamic_cast<ArrayBracket*>(o)) {
				push(ab);
			}
		}
		else if (auto ab = dynamic_cast<ArrayBracket*>(obstack.top())) {
			if (auto str = dynamic_cast<Value*>(o)) {
				ab->entries.push_back(str);
			}
			else if (auto cb = dynamic_cast<CurlyBracket*>(o)) {
				ab->entries.push_back(cb);
				push(cb);
			}
			else if (auto ab1 = dynamic_cast<ArrayBracket*>(o)) {
				ab->entries.push_back(ab1);
				push(ab1);
			}
			
		}
	}
	void push(Object* o) {
		obstack.push(o);
	}
	void pop() {
		if (!obstack.empty()) {
			obstack.pop();
		}
	}
	bool instring = false;
	bool inkeyword = false;
	bool innumber = false;
	char lastchar = ' ';
	string strbuf = "";
	string keyword = "";
	string number = "";
	void feedchar(char c) {
		if (instring) {
			if (lastchar != '\\') {
				if (c == '"') {
					//printf("str:\"%s\"\n", strbuf.c_str());
					add(new String(strbuf));
					strbuf = "";
					instring = false;
					return;
				}
			}
			lastchar = c;
			strbuf += c;
		}
		else {
			if (c == '"') {
				instring = true;
			}
			else if (isdigit(c) || c == '.') {
				innumber = true;
				number += c;
			}
			else if (isalpha(c)) {
				inkeyword = true;
				keyword += c;
			}
			else {
				if (inkeyword) {
					inkeyword = false;
					//printf("kw:'%s'\n", keyword.c_str());
					if (keyword == "true") {
						add(new Boolean(true));
					}
					else if(keyword == "false"){
						add(new Boolean(true));
					}
					else {
						printf("unknown keyword!!!:'%s'\n", keyword.c_str());
					}
					keyword = "";
				}
				if (innumber) {
					innumber = false;
					add(new Number(atof(number.c_str())));
					number = "";
				}
				if (c == '{') {
					auto br = new CurlyBracket();
					add(br);
				}
				else if (c == '[') {
					auto br = new ArrayBracket();
					add(br);
				}
				else if (c == ':') {

				}
				else if (c == '}') {
					//if(!obstack.empty())
					while (!obstack.empty()) {
						if (auto cb = dynamic_cast<CurlyBracket*>(obstack.top())) {
							pop();
							break;
						}
						pop();
					}

				}
				else if (c == ']') {
					while (!obstack.empty()) {
						if (auto cb = dynamic_cast<ArrayBracket*>(obstack.top())) {
							pop();
							break;
						}
						pop();
					}
				}
			}
		}
	}
};

JSON jsonfromfile(string file) {
	std::ifstream ifs(file);
	std::stringstream buffer;
	buffer << ifs.rdbuf();
	ifs.close();
	return JSON(buffer.str());
}