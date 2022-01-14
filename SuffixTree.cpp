#include <iostream>
#include <string>
#include <memory>
#include <map>

using namespace std;

class Edge;

class Node
{
public:
	map<char, shared_ptr<Edge>> childs;
	shared_ptr<Node> suffix_link;

	Node();
};

class Edge
{
public:
	unsigned L;
	unsigned R;
	shared_ptr<Node> dest;

	Edge(int L, int R, shared_ptr<Node> dest);
	int length();
};

Node::Node() :
childs(),
suffix_link(nullptr)
{
}

Edge::Edge(int L, int R, shared_ptr<Node> dest) :
L(L),
R(R),
dest(dest)
{
}

int Edge::length()
{
	return R - L;
}

shared_ptr<Node> build_tree(string s)
{
	auto root = make_shared<Node>();
	int reminded = 0;

	auto current_node = root;
	shared_ptr<Edge> current_edge = nullptr;
	int current_pos = 0;

	shared_ptr<Node> new_node = nullptr;

	s += '\0';

	for (unsigned pos = 0; pos < s.length(); pos++)
	{
		reminded++;

		while (reminded)
		{
			if (current_edge == nullptr)
			{
				if (current_node->childs.find(s[pos]) == current_node->childs.end())
				{
					auto leaf = make_shared<Node>();
					current_node->childs[s[pos]] = make_shared<Edge>(pos, s.length(), leaf);

					if (new_node != nullptr)
					{
						new_node->suffix_link = current_node;
						new_node = nullptr;
					}
				}
				else
				{
					current_edge = current_node->childs[s[pos]];
					current_pos = 1;
					if (current_pos == current_edge->length())
					{
						current_node = current_edge->dest;
						current_edge = nullptr;
						current_pos = 0;
					}
					break;
				}
			}
			else
			{
				if (s[current_pos] == s[pos])
				{
					current_pos++;
					if (current_pos == current_edge->length())
					{
						current_node = current_edge->dest;
						current_edge = nullptr;
						current_pos = 0;
					}
					break;
				}
				else
				{
					auto tmp_node = new_node;

					new_node = make_shared<Node>();
					auto e1 = make_shared<Edge>(current_edge->L,
						current_edge->L + current_pos,
						new_node);
					auto e2 = make_shared<Edge>(current_edge->L + current_pos,
						current_edge->R,
						current_edge->dest);
					current_node->childs[s[e1->L]] = e1;
					new_node->childs[s[e2->L]] = e2;

					auto leaf = make_shared<Node>();
					new_node->childs[s[pos]] = make_shared<Edge>(pos, s.length(), leaf);

					if (tmp_node != nullptr)
						tmp_node->suffix_link = new_node;



					if (current_node == root)
					{
						char c = s[current_edge->L + 1];
						current_pos--;
						if (current_pos == 0)
							current_edge = nullptr;
						else
							current_edge = current_node->childs[c];
					}
					else
					{
						char c = s[current_edge->L];
						if (current_node->suffix_link == nullptr)
							current_node = root;
						else
							current_node = current_node->suffix_link;
						while (current_node->childs[c]->length() <= current_pos)
						{
							current_edge = current_node->childs[c];
							current_node = current_edge->dest;
								current_pos -= current_edge->length();
							c = s[current_edge->R];
						}
						if (current_pos == 0)
						{
							current_edge = nullptr;

							if (new_node)
							{
								new_node->suffix_link = current_node;
								new_node = nullptr;
							}
						}
						else
						{
							current_edge = current_node->childs[c];
						}
					}
				}
			}

			reminded--;
		}
	}

	return root;
}


bool find_substring(shared_ptr<Node> root, string s, string pattern)
{
	auto node = root;
	shared_ptr<Edge> edge = nullptr;
	int pos = 0;

	for (unsigned i = 0; i < pattern.length(); i++)
	{
		if (edge == nullptr)
		{
			if (node->childs.find(pattern[i]) == node->childs.end())
				return false;

			edge = node->childs[pattern[i]];
			pos = 0;
		}

		if (s[edge->L + pos] != pattern[i])
			return false;
		pos++;

		if (pos == edge->length())
		{
			node = edge->dest;
			edge = nullptr;
		}
	}
	return true;
}

void main()
{
	string s;
	cout << "Input string: ";
	getline(cin, s);
	auto root = build_tree(s);

	for (;;)
	{
		string pattern;
		cout << "Input substring: ";
		getline(cin, pattern);

		if (pattern.length() == 0)
			return;

		if (find_substring(root, s, pattern))
			cout << "The substring is included in the string" << endl;
		else
			cout << "The substring is missing in the string" << endl;
	}
	cout << "Press any key to close program" << endl;
	cin.get();
}