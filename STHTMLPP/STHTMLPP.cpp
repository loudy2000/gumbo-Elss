// STHTMLPP.cpp: 定义控制台应用程序的入口点。
//
#pragma warning (disable : 4996)
#include "stdafx.h"
#include <iostream>
extern "C" {
	#include "sds.h"
	#include<Windows.h>  
}
#include<vector>
#include "gumbo.h"
using namespace std;

#define CALLTYPE _stdcall
#define HTMLAPI
typedef struct HTML{

	HTMLAPI GumboOutput* CALLTYPE HTML_Create(const char* buffer)
	{
		return gumbo_parse(buffer);
	}
	HTMLAPI void CALLTYPE HTML_Destory(GumboOutput* output)
	{
		gumbo_destroy_output(&kGumboDefaultOptions, output);
	}
	HTMLAPI GumboNode* CALLTYPE HTML_GetRootNode(GumboOutput* output)
	{
		return output->root;
	}
	HTMLAPI GumboNode* CALLTYPE HTML_GetDocumentNode(GumboOutput* output)
	{
		return output->document;
	}

	//////////////////////////////////////////////////////////////
	HTMLAPI GumboNode* CALLTYPE HTMLNode_GetParent(GumboNode* node)
	{
		return node->parent;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetIndexInParent(GumboNode* node)
	{
		return node->index_within_parent;
	}
	HTMLAPI GumboTag CALLTYPE HTMLNode_GetTag(GumboNode* node)
	{
		return node->v.element.tag;
	}
	HTMLAPI const char* CALLTYPE HTMLNode_GetOriginalTag(GumboNode* node)
	{
		return node->v.element.original_tag.data;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetOriginalTagLength(GumboNode* node)
	{
		return node->v.element.original_tag.length;
	}
	HTMLAPI const char* CALLTYPE HTMLNode_GetOriginalEndTag(GumboNode* node)
	{
		return node->v.element.original_end_tag.data;
	}
	HTMLAPI unsigned int CALLTYPE HTMLNode_GetOriginalEndTagLength(GumboNode* node)
	{
		return node->v.element.original_end_tag.length;
	}

	HTMLAPI const char* CALLTYPE HTMLNode_GetText(GumboNode* node)
	{
		return node->v.text.text;
	}
	HTMLAPI GumboNodeType CALLTYPE HTMLNode_GetType(GumboNode* node)
	{
		return node->type;
	}
	HTMLAPI int CALLTYPE HTMLNode_GetChildCount(GumboNode* node)
	{
		return node->v.element.children.length;
	}
	HTMLAPI GumboNode* CALLTYPE HTMLNode_GetChild(GumboNode* node, size_t i)
	{
		return static_cast<GumboNode*>(node->v.element.children.data[i]);
	}
	HTMLAPI int CALLTYPE HTMLNode_GetAttributeCount(GumboNode* node)
	{
		return node->v.element.attributes.length;
	}
	HTMLAPI GumboAttribute* CALLTYPE HTMLNode_GetAttribute(GumboNode* node, size_t i)
	{
		return static_cast<GumboAttribute*>(node->v.element.attributes.data[i]);
	}

	HTMLAPI GumboAttribute* CALLTYPE HTMLNode_GetAttributeByName(GumboNode* node, const char* name)
	{
		return gumbo_get_attribute(&node->v.element.attributes, name);
	}

	/////////////////////////////////////////////////////////////////////////////////
	HTMLAPI const char* CALLTYPE HTMLAttribute_GetValue(GumboAttribute* attr)
	{
		return attr->value;
	}
	HTMLAPI const char* CALLTYPE HTMLAttribute_GetName(GumboAttribute* attr)
	{
		return attr->name;
	}
	/*GumboOutput*(*HTML_Create)(char*);
	GumboNode*(*HTML_GetRootNode)(GumboOutput*);
	GumboNode*(*HTML_GetDocumentNode)(GumboOutput*);
	GumboNode*(*HTMLNode_GetParent)(GumboNode*);
	unsigned int(*HTMLNode_GetIndexInParent)(GumboNode*);
	GumboTag(*HTMLNode_GetTag)(GumboNode*);
	const char*(*HTMLNode_GetOriginalTag)(GumboNode*);
	unsigned int(*HTMLNode_GetOriginalTagLength)(GumboNode*);
	
	const char*(*HTMLNode_GetOriginalEndTag)(GumboNode*);
	unsigned int(*HTMLNode_GetOriginalEndTagLength)(GumboNode*);
	const char*(*HTMLNode_GetText)(GumboNode*);
	GumboNodeType(*HTMLNode_GetType)(GumboNode*);
	int(*HTMLNode_GetChildCount)(GumboNode*);
	GumboNode *(*HTMLNode_GetChild)(GumboNode*,size_t);
	GumboAttribute*(*HTMLNode_GetAttributeCount)(GumboNode*, size_t);
	GumboAttribute*(*HTMLNode_GetAttributeByName)(GumboNode*, const char*);
	char*(*HTMLAttribute_GetValue)(GumboAttribute*);
	char*(*HTMLAttribute_GetName)(GumboAttribute*);

	void(*HTML_Destory)(GumboOutput*);*/
}HTMLengine;

typedef struct HtmlNode
{
	sds innerHtml;
	sds innerText;
}HtmlNode;

#define CLASS "class"
#define ID "id"

string& trim(string &s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}

void ergodic(vector<GumboNode*> *momo, HTMLengine* engine, GumboNode* Node, string path, string NodeType, BOOL recursion = true/*申明了递归深度true无限递归反之一层*/, BOOL layer = true)
{
	GumboNodeType Type = engine->HTMLNode_GetType(Node);
	if (Type == GUMBO_NODE_ELEMENT)
	{
		if (NodeType=="class")
		{
			GumboAttribute* name = engine->HTMLNode_GetAttributeByName(Node, NodeType.c_str());
			if (name!=NULL)
			{
				if (string(name->value).compare(path) == 0)
				{
					momo->push_back(Node);
				}
			}
		}
		else if (NodeType == "id") {
			GumboAttribute* name = engine->HTMLNode_GetAttributeByName(Node, NodeType.c_str());
			if (name != NULL)
			{
				if (string(name->value) == path)
				{
					momo->push_back(Node);
				}
			}
		}
		else {
			const char* TagText = engine->HTMLNode_GetOriginalTag(Node);
			if (TagText != NULL)
			{
				string text(TagText);
				string text1 = text.substr(1, NodeType.length());
				if (string(text1) == NodeType)
				{
					momo->push_back(Node);
				}
			}	
		}
		if (recursion)//控制递归深度
		{
			int n = engine->HTMLNode_GetChildCount(Node);
			for (size_t i = 0; i < n; i++)
			{
				GumboNode* nod = engine->HTMLNode_GetChild(Node, i);
				ergodic(momo, engine, nod, path, NodeType, layer);
			}
		}
	} 
}

vector<GumboNode*> BLHQ(HTMLengine* engine, GumboNode* H,sds path)
{
	//".class > a > div > .s"
	//"#id > a > div > .s"
	//"div > a > div > .s"
	//首先解析表达式
	vector<string> Nodes;
	char *token;
	const char s[2] = ">";
	token = strtok(path, s);
	while (token != NULL) {
		string token1(token);
		Nodes.push_back(trim(token1));
		token = strtok(NULL, s);
	}
	vector<GumboNode*> as;
	for (size_t i = 0; i < Nodes.size(); i++)
	{
		char a = Nodes[i].c_str()[0];
		switch (a)
		{
		case '.'://遍历as节点数组寻找满足class节点
			if (as.size()>0)
			{
				vector<GumboNode*> as_class;
				size_t te_1 = Nodes[i].erase(0, 1).find("[");
				if (te_1 != -1)
				{
					int x = atoi(Nodes[i].substr(te_1 - 1, Nodes[i].find("]") - te_1).c_str());
					for (size_t ia = 0; ia < as.size(); ia++)
					{
						ergodic(&as_class, engine, as.at(ia), Nodes[i].substr(0, te_1), CLASS, true, false);
					}
					if (x <= as_class.size())
					{
						as.clear();
						as.push_back(as_class[x]);
					}
				}
				else {
					for (size_t i1 = 0; i1 < as.size(); i1++)
					{
						ergodic(&as_class, engine, as.at(i1), Nodes[i], CLASS, true, false);
					}
					as.assign(as_class.begin(), as_class.end());
				}
			}
			else {
				size_t te_1 = Nodes[i].erase(0, 1).find("[");
				vector<GumboNode*> as_class;
				if (te_1 != -1) {
					int x = atoi(Nodes[i].substr(te_1 - 1, Nodes[i].find("]") - te_1).c_str());
					ergodic(&as_class, engine, H, Nodes[i].substr(0, te_1), CLASS);
					if (x <= as_class.size())
					{
						as.clear();
						as.push_back(as_class[x]);
					}
				}
				else {
					ergodic(&as_class, engine, H, Nodes[i], CLASS);
					as.assign(as_class.begin(), as_class.end());
				}
			}
			break;
		case '#'://遍历as节点数组寻找满足id节点
			if (as.size()>0)
			{
				vector<GumboNode*> as_class;
				size_t te_1 = Nodes[i].erase(0, 1).find("[");
				if (te_1 != -1)
				{
					int x = atoi(Nodes[i].substr(te_1 - 1, Nodes[i].find("]") - te_1).c_str());
					for (size_t ia = 0; ia < as.size(); ia++)
					{
						ergodic(&as_class, engine, as.at(ia), Nodes[i].substr(0, te_1), ID, true, false);
					}
					if (x <= as_class.size())
					{
						as.clear();
						as.push_back(as_class[x]);
					}
				}
				else {
					for (size_t i1 = 0; i1 < as.size(); i1++)
					{
						ergodic(&as_class, engine, as.at(i1), Nodes[i], ID, true, false);
					}
					as.assign(as_class.begin(), as_class.end());
				}
			}
			else {
				size_t te_1 = Nodes[i].erase(0, 1).find("[");
				vector<GumboNode*> as_class;
				if (te_1 != -1) {
					int x = atoi(Nodes[i].substr(te_1 - 1, Nodes[i].find("]") - te_1).c_str());
					ergodic(&as_class, engine, H, Nodes[i].substr(0, te_1), ID);
					if (x <= as_class.size())
					{
						as.clear();
						as.push_back(as_class[x]);
					}
				}
				else {
					ergodic(&as_class, engine, H, Nodes[i], ID);
					as.assign(as_class.begin(), as_class.end());
				}
			}
			break;
		default://遍历as节点数组寻找满足id节点
			if (as.size()>0)
			{
				vector<GumboNode*> as_class;
				size_t te_1 = Nodes[i].find("[");
				if (te_1 != -1)
				{
					int x = atoi(Nodes[i].substr(te_1 - 1, Nodes[i].find("]") - te_1).c_str());
					for (size_t ia = 0; ia < as.size(); ia++)
					{
						ergodic(&as_class, engine, as.at(ia), Nodes[i].substr(0, te_1), Nodes[i].substr(0, te_1), true, false);
					}
					if (x <= as_class.size())
					{
						as.clear();
						as.push_back(as_class[x]);
					}
				}
				else {
					for (size_t i1 = 0; i1 < as.size(); i1++)
					{
						ergodic(&as_class, engine, as.at(i1), Nodes[i], Nodes[i], true, false);
					}
					//string classname = engine->HTMLNode_GetAttributeByName(as_class[0], "class")->value;
					as.assign(as_class.begin(), as_class.end());
				}
			}
			else {
				size_t te_1 = Nodes[i].find("[");
				int x = atoi(Nodes[i].substr(te_1 - 1, Nodes[i].find("]") - te_1).c_str());
				vector<GumboNode*> as_class;
				if (te_1 != -1) {
					ergodic(&as_class, engine, H, Nodes[i].substr(0, te_1), Nodes[i].substr(0, te_1));
					if (x <= as_class.size())
					{
						as.clear();
						as.push_back(as_class[x]);
					}
				}
				else {
					ergodic(&as_class, engine, H, Nodes[i], Nodes[i]);
					as.assign(as_class.begin(), as_class.end());
				}
			}
			break;
		}
	}
	return as;
}

HTMLengine CreateHtml()
{
	HTMLengine KK = {};
	return KK;
}
//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}
void STHTML(sds HTML,sds path)
{
	//初始化所有接口api
	HTMLengine html = CreateHtml();
	//加载解析一个html字符串
	GumboOutput *engine = html.HTML_Create(HTML);
	//获取html根节点
	GumboNode* gum = html.HTML_GetRootNode(engine);
	//根据自定义css选择器函数解析一个节点或者节点数组
	vector<GumboNode*> HTMLnode = BLHQ(&html, gum, path);
	
	//返回节点数组
	//切记不能销毁引擎之后再去读节点 会爆炸的
	cout << "表达式: "<< ".header[0] > div > #nav > ul > li > a > span" <<"\n";
	for (size_t i = 0; i < HTMLnode.size(); i++)
	{
		//cout << HTMLnode[i]->v.element.children.length << "\n";
		GumboNode* htmlcode = html.HTMLNode_GetChild(HTMLnode[i], 0);
		cout << "结果:" << U2G(htmlcode->v.text.text) << "\n";
	}
	//销毁引擎
	html.HTML_Destory(engine);
}
#include <fstream>
int main()
{
	const char* filename = "递归软件.html";

	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (!in) {
		std::cout << "File " << filename << " not found!\n";
		exit(EXIT_FAILURE);
	}

	std::string contents;
	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
	in.close();

	sds htmla = sdsnew(contents.c_str());
	sds patha = sdsnew("#list > dl > dd > a");

	//目前表达式很简单
	//.class > a > div > li
	//.class > #kk > div > li
	//#id > #kk > div > li
	//div > a[5] > li > p[1]
	STHTML(htmla, patha);
    return 0;
}

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
#include <Windows.h>
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}