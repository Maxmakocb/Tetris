#pragma once

#include <vector>

template <typename T>
class Node {
public:
	std::vector<Node<T>*> nodes;
	T value;
	
	Node()
	{
		//this->nodes = std::vector<Node<T>*>{};
	}

	template <typename T>
	Node(T value) {
		this->value = value;
		//this->nodes = std::vector<Node<T>*>{};
	}

	~Node() {
		for (int i = 0; i < int(this->nodes.size()); i++) {
			delete this->nodes[i];
		}
		this->nodes.clear();
	}

	template <typename T>
	Node(Node<T>& n) {
		this->value = n.value;
		// recursive copying, will copy every node in a tree.
		for (int i = 0; i < int(n.nodes.size()); i++) {
			auto node = *n.nodes[i];
			this->nodes.push_back(&node);
		}
	}
	template<typename T>
	Node(Node<T>&& n)
	{
		this->value = std::move(n.value);
		this->nodes = std::move(n.nodes);
		n.nodes = std::vector<Node<T>*>{};
	}

	template <typename T>
	bool addNode(T add, T after, bool init = true)
	{
		bool found = false;
		for (int i = 0; i < int(this->nodes.size()); i++) {
			if (this->nodes[i]->value == after) {
				this->nodes[i]->nodes.push_back(new Node(add));
				found = true;
			}
			found = this->nodes[i]->addNode<T>(add, after, false);
			if (found) {
				return true;
			}
		}

		if (init) {
			if (!found) {
				this->nodes.push_back(new Node(add));
			}
		}

		return found;
	}

template<typename T>
std::vector<std::vector<T>> toVectorArray()
	{
		std::vector<std::vector<T>> output = std::vector<std::vector<T>>{};
		for (auto node : this->nodes) {
			std::vector<T>* tree = new std::vector<T>;
			tree->push_back(node->value);
			node->treeToVectorArray(tree);
			output.push_back(*tree);
		}
		return output;
	}
private:
	template<typename T>
	void treeToVectorArray(std::vector<T>* arr)
	{
		for (auto node : this->nodes) {
			arr->push_back(node->value);
			node->treeToVectorArray<T>(arr);
		}
	}
};
