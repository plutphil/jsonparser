
#include <iostream>
#include"JSON.hpp"
int main()
{
	auto json=jsonfromfile("test.json");
	auto k = json.root->get("web-app")->get("servlet")->get(0)->get("servlet-name");
	if (auto str = dynamic_cast<String*>(k)) {
		printf("\nstring: '%s'\n", str->val.c_str());
	}
}
