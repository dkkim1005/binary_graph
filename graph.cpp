#include <iostream>
#include <queue>
#include <stack>
#include <deque>

namespace graph {

template <typename TYPE>
struct binary_graph_format
{
	explicit binary_graph_format(TYPE const& INPUT)
	: ELEMENT(INPUT), parent(nullptr), left(nullptr), right(nullptr) {}

	binary_graph_format(TYPE const& INPUT, binary_graph_format<TYPE>* parent_node)
	: ELEMENT(INPUT), parent(parent_node), left(nullptr), right(nullptr) {}

	~binary_graph_format()
	{
		set_del_left();
		set_del_right();
	}

	bool insert_left(TYPE const& INPUT) 
	{
		bool result = false;
		if(left == nullptr) 
		{
			left = new binary_graph_format<TYPE>(INPUT, this);
			result = true;
		}
		return result;
	}

	bool insert_right(TYPE const& INPUT) 
	{
		bool result = false;
		if(right == nullptr)
		{
			right = new binary_graph_format<TYPE>(INPUT, this);
			result = true;
		}
		return result;
	}

	binary_graph_format<TYPE>* parent_node() const {
		return parent;
	}

	binary_graph_format<TYPE>* left_node() const {
		return left;
	}

	binary_graph_format<TYPE>* right_node() const {
		return right;
	}
	
	TYPE get() const {
		return ELEMENT;
	}

	void insert(TYPE const& INPUT) {
		ELEMENT = INPUT;
	}

	void set_del_left() 
	{
		if(left != nullptr) 
		{
			delete left;
			left = nullptr;
		}
	}

	void set_del_right() 
	{
		if(right != nullptr)
		{
			delete right;
			right = nullptr;
		}
	}

private:
	TYPE ELEMENT;
	binary_graph_format<TYPE>* parent;
	binary_graph_format<TYPE>* left;
	binary_graph_format<TYPE>* right;
};


enum state_filling {zero = 0, half = 1};

template <typename TYPE>
struct binary_graph
{
	explicit binary_graph(TYPE const INPUT)
	: topNode(new binary_graph_format<TYPE>(INPUT)), state(zero) {
		endnodes.push_back(topNode);
	}

	virtual ~binary_graph() {
		if(topNode != nullptr) {
			delete topNode;
			topNode = nullptr;
		}
	}

	virtual void push(TYPE const INPUT)
	{
		binary_graph_format<TYPE>* node = endnodes.front();

		switch(state)
		{
			case zero :
				node -> insert_left(INPUT);
				endnodes.push_back(node -> left_node());
				state = half;
				break;
			case half :
				node -> insert_right(INPUT);
				endnodes.push_back(node -> right_node());
				endnodes.pop_front();
				state = zero;
				break;
		}
	}

	binary_graph_format<TYPE>* origin() const {
		return topNode;
	}

protected:
	state_filling state;
	binary_graph_format<TYPE>* topNode;
	std::deque<binary_graph_format<TYPE>* > endnodes;
};


template <typename TYPE>
struct heap_graph : public binary_graph<TYPE> 
{
	using binary_graph<TYPE>::push;
	using binary_graph<TYPE>::endnodes;
	using binary_graph<TYPE>::topNode;
	using binary_graph<TYPE>::state;

	explicit heap_graph(TYPE const INPUT)
	: binary_graph<TYPE>(INPUT)
	{}

	virtual ~heap_graph() {}

	virtual void push(TYPE const INPUT)
	{
		binary_graph<TYPE>::push(INPUT);
		auto* childNode = endnodes.back();
		auto* parentNode = childNode -> parent_node();

		while(true)
		{
			if(parentNode == nullptr) {
				break;
			}

			TYPE&&  childElement = childNode -> get(),
				parentElement = parentNode -> get();

			if(childElement > parentElement)
			{
				childNode -> insert(parentElement);
				parentNode -> insert(childElement);
			}
			else {
				break;
			}

			childNode = parentNode;
			parentNode = childNode -> parent_node();
		}
	}

	TYPE pop()
	{
		const TYPE& topElement = topNode -> get();

		if(endnodes.size() == 1) {
			return topElement;
		}

		const TYPE& bottomElement = (endnodes.back()) -> get();

		binary_graph_format<TYPE>* parentNode = (endnodes.back()) -> parent_node();
		binary_graph_format<TYPE>* childNode = nullptr;

		switch(state)
		{
			case zero :
				parentNode -> set_del_right();
				state = half;
				endnodes.push_front(parentNode);
				break;
			case half :
				parentNode -> set_del_left();
				state = zero;
				break;
		}
	
		endnodes.pop_back();

		topNode -> insert(bottomElement);

		parentNode = topNode;
		childNode = direction(parentNode);

		while(true)
		{
			if(childNode == nullptr) {
				break;
			}

			TYPE&& parentElement = parentNode -> get(),
				childElement = childNode -> get();

			if(childElement > parentElement)
			{
				parentNode -> insert(childElement);
				childNode -> insert(parentElement);
			}
			else {
				break;
			}

			parentNode = childNode;
			childNode = direction(parentNode);
		}

		return topElement;
	}

private:
	binary_graph_format<TYPE>* direction(binary_graph_format<TYPE>* parent)
	{
		binary_graph_format<TYPE> *left = parent -> left_node(),
					  *right = parent -> right_node();

		if(left != nullptr and right != nullptr) {
			TYPE leftElement = left -> get(),
			     rightElement = right ->get();
			if(leftElement > rightElement) {
				return left;
			}
			else {
				return right;
			}
		}
		else if(left != nullptr and right == nullptr) {
			return left;
		}
		else {
			return nullptr;
		}
	}
};


template <typename CONTAINER>
void heap_sort(const CONTAINER& STORAGE)
{
	double dinit = *(STORAGE.begin());
	heap_graph<double> heap_structure(dinit);

	for(auto const& elem : STORAGE) {
		heap_structure.push(elem);
	}

	for(int i=0; i<STORAGE.size(); ++i) {
		std::cout<<heap_structure.pop()<<" ";
	}
	std::cout<<std::endl;
}


template <typename TYPE>
bool breadth_first_search(const binary_graph<TYPE>& BinGraph, TYPE const ELEMENT)
{
	bool result = false;

	std::queue<binary_graph_format<TYPE>* > dataSearch;

	binary_graph_format<TYPE>* node_upper = nullptr;
	binary_graph_format<TYPE>* node_below = nullptr;

	dataSearch.push(BinGraph.origin());

	while(true)
	{
		node_upper = dataSearch.front();
		dataSearch.pop();

		node_below = node_upper -> left_node();

		if(node_below != nullptr) {
			dataSearch.push(node_below);
		}

		node_below = node_upper -> right_node();

		if(node_below != nullptr) {
			dataSearch.push(node_below);
		}

		if(node_upper -> get() == ELEMENT) 
		{
			result = true;
			goto END_SEARCH;
		}
		else if(dataSearch.size() == 0) 
		{
			result = false;
			goto END_SEARCH;
		}
	}

END_SEARCH:

	return result;
}


template <typename TYPE>
bool depth_first_search(const binary_graph<TYPE>& BinGraph, TYPE const ELEMENT)
{
	bool result = false;

	std::stack<binary_graph_format<TYPE>* > dataSearch;

	binary_graph_format<TYPE>* node_upper = nullptr;
	binary_graph_format<TYPE>* node_below = nullptr;

	dataSearch.push(BinGraph.origin());

	while(true)
	{
		node_upper = dataSearch.top();
		dataSearch.pop();

		node_below = node_upper -> right_node();

		if(node_below != nullptr) {
			dataSearch.push(node_below);
		}

		node_below = node_upper -> left_node();

		if(node_below != nullptr) {
			dataSearch.push(node_below);
		}

		if(node_upper -> get() == ELEMENT) 
		{
			result = true;
			goto END_SEARCH;
		}
		else if(dataSearch.size() == 0) {
			goto END_SEARCH;
		}
	}

END_SEARCH:

	return result;
}

} // namespace graph

int main(int argc, char* argv[])
{
	graph::binary_graph<double> G(1.0);
	G.push(2);
	G.push(3);
	G.push(4);
	G.push(5);
	G.push(6);
	G.push(7);
	G.push(11);
	G.push(12);
	G.push(13);
	G.push(14);

	std::cout<<graph::breadth_first_search(G, 5.)<<std::endl;
	std::cout<<graph::breadth_first_search(G, 7.)<<std::endl;
	std::cout<<graph::breadth_first_search(G, 11.)<<std::endl;
	std::cout<<graph::breadth_first_search(G, 12.)<<std::endl;

	std::cout<<graph::depth_first_search(G, 5.)<<std::endl;
	std::cout<<graph::depth_first_search(G, 7.)<<std::endl;
	std::cout<<graph::depth_first_search(G, 11.)<<std::endl;
	std::cout<<graph::depth_first_search(G, 12.)<<std::endl;

	graph::heap_graph<double> H(1.);
	H.push(2.);
	H.push(3.);
	H.push(4.);
	H.push(5.);
	H.push(-5.);
	H.push(-7.);

	std::cout<<H.pop()<<" ";
	std::cout<<H.pop()<<" ";
	std::cout<<H.pop()<<" ";
	std::cout<<H.pop()<<" ";
	std::cout<<H.pop()<<" ";
	std::cout<<H.pop()<<" ";
	std::cout<<H.pop()<<" ";
	std::cout<<std::endl;

	return 0;
}
