#include "json.hpp"
#include <iostream>
#include <string>


std::string N(std::istream& is);
json V(std::istream& is);
std::string K(std::istream& is);
json S(std::istream& is, json& rhs);
json O(std::istream& is);
json A(std::istream& is);
std::pair<std::string, json> P(std::istream& is);
void M(std::istream& is, json& rhs);
void E(std::istream& is,json& rhs);
void ignore_char(std::istream& is);
void printJson(std::ostream& lhs, json const& rhs);


#define NULL_TYPE 0
#define DOUBLE_TYPE 1
#define STRING_TYPE 2
#define BOOL_TYPE 3
#define OBJECT_TYPE 4
#define ARRAY_TYPE 5

struct json::impl {
	
	int type;

	std::string string_value;
	double number_value;
	bool bool_value;
	

	

	struct ListNode {
		json value;
		ListNode* next;

		ListNode(const json& val) : value(val), next(nullptr) {}
		ListNode() { next = nullptr; }
	};
	ListNode* list_head;
	ListNode* list_tail;


	struct DictNode {
		
		std::pair<std::string,json> value;
		DictNode* next;

		DictNode(const std::string& k, const json& val) :DictNode()
		{
			std::pair<std::string, json> dict = {k,val};
			this->value = dict;
		}

		DictNode()
		{
			next = nullptr;
		}
	};

	DictNode* dict_head;
	DictNode* dict_tail;



	impl() : type(NULL_TYPE), list_head(nullptr), list_tail(nullptr), dict_head(nullptr), dict_tail(nullptr)
	{
		bool_value = false;
		number_value = 0;
	}

	~impl() {
		
		this->deleteList();
		this->deleteDict();
	}

	void deleteList()
	{
		// Clean up the linked list of list elements
		ListNode* current_list_node = list_head;
		while (current_list_node) {
			ListNode* temp = current_list_node;
			current_list_node = current_list_node->next;
			delete temp;
		}

		list_head = nullptr;
	}

	void deleteDict()
	{

		// Clean up the linked list of dictionary elements
		DictNode* current_dict_node = dict_head;
		while (current_dict_node) {
			DictNode* temp = current_dict_node;
			current_dict_node = current_dict_node->next;
			delete temp;
		}
		
		dict_head = nullptr;
		dict_tail = nullptr;
	}


};

std::istream& operator>>(std::istream& lhs, json& rhs)
{
	rhs = S(lhs, rhs);
	return lhs;

}




/* -----  PARTE DI PARSING ---- */
json S(std::istream& is, json& rhs)
{
	//S → V

	if (!is.eof())
	{
		ignore_char(is);

		if (is.peek() == '{' || is.peek() == '[' || is.peek() == 'n' || is.peek() == 't' || is.peek() == 'f'  || is.peek() == '"' ||
			is.peek() <= 57 || is.peek() >= 48 || is.peek() == '.')
		{
			rhs = std::move(V(is));
		}
		else
		{
			throw json_exception();
		}
		
	}
	
	if (!is.eof())
	{
		throw json_exception();
	}

	return rhs;
}



json O(std::istream& is)
{
	// O →  { M } | { } 

	ignore_char(is);
	json rhs;
	rhs.set_dictionary();


	if (is.peek() == '{')
	{
		is.ignore();

		ignore_char(is);



		if (is.peek() != '}')
		{
			M(is,rhs);
			ignore_char(is);
		}

		if (is.peek() == '}')
		{
			is.ignore();
		}
		else
		{
			throw json_exception();
		}
	}

	

	return rhs;

}



void M(std::istream& is, json& rhs)
{
	//M → P | P, M

	while (true)
	{
		ignore_char(is);

		if (is.peek() == '"')
		{
			rhs.insert(P(is));
			ignore_char(is);
		}
		else
		{
			throw json_exception();
		}

		if (is.peek() == ',')
		{
			is.ignore();
		}
		else
		{
			return;
		}
	}
	/*
	ignore_char(is);

	if (is.peek() == '"')
	{

		rhs.insert(P(is));

		ignore_char(is);

		while (true)
		{
			ignore_char(is);
			if (is.peek() == ',')
			{
				is.ignore();
				M(is,rhs);
			}
			else
			{
				return;
			}
		}
	}
	else
	{
		throw json_exception();
	}
	*/


}

/*
*

	GRAMMATICA DA SEGUIRE
		S → V
		O →  { M } | { }
		A → [ E ] | [ ]
		M → P | P , M
		E → V | V , E
		P → "K" : V
		K → empty | char | char + K
		V → null | true | false | N | "K" | O | A
		N → numbers | - numbers | + numbers | e | pi

*/



json A(std::istream& is)
{
	// A → [ E ] | [ ]

	ignore_char(is);
	json rhs;
	rhs.set_list();

	if (is.peek() == '[')
	{
		is.ignore();

		ignore_char(is);

		if (is.peek() != ']')
		{
			
			E(is,rhs);
			ignore_char(is);
		}

		if (is.peek() == ']')
		{
			is.ignore();
		}
		else
		{
			throw json_exception();
		}
	}

	return rhs;

}

void E(std::istream& is, json& rhs)
{
	// E → V | V , E

	

	while (true)
	{
		ignore_char(is);

		rhs.push_back(std::move(V(is)));

		ignore_char(is);

		if (is.peek() == ',')
		{
			
			ignore_char(is);
			is.ignore();
			ignore_char(is);
		}
		else
		{
			return;
		}
	}

}

std::pair<std::string, json> P(std::istream& is)
{
	// P → "K" : V

	ignore_char(is);
	json rhs;
	std::string key;

	if (is.peek() == '"')
	{
		is.ignore();
		key = K(is);

		ignore_char(is);

		if (is.peek() == ':')
		{
			is.ignore();

			ignore_char(is);

			rhs = std::move(V(is));
		}
		else
		{
			throw json_exception();
		}

	}
	else
	{
		throw json_exception();
	}

	std::pair<std::string, json> dict_node = { key,rhs };

	return dict_node;
}

json V(std::istream& is)
{
	json rhs;

	ignore_char(is);

	// V → null | true | false | N | "K" | O | A

	if (is.peek() == 'n')
	{
		std::string s = "";

		while (is.peek() != '}' && is.peek() != ']' && is.peek() != '"' && is.peek() != ',' && !is.eof())
		{

			s += is.peek();
			is.ignore();
			ignore_char(is);
		}

		if (s != "null")
		{
			throw json_exception();
		}
		else
		{
			rhs.set_null();
		}

		ignore_char(is);

	}
	else if (is.peek() == 't' || is.peek() == 'f')
	{
		std::string s = "";

		while (is.peek() != '}' && is.peek() != ']' && is.peek() != '"' && is.peek() != ',' && !is.eof())
		{
			s += is.peek();
			is.ignore();
			ignore_char(is);
		}

		if (s != "true" && s != "false")
		{
			throw json_exception();
		}
		else
		{
			if (s == "true")
			{
				rhs.set_bool(true);
			}
			else
			{
				rhs.set_bool(false);
			}
		}

		ignore_char(is);

	}
	else if ((is.peek() <= 57 && is.peek() >= 48) || is.peek() == '+' || is.peek() == '-' || is.peek() == '.')
	{
		rhs.set_number(	std::stod(N(is)));
		ignore_char(is);
	}
	else if (is.peek() == '"')
	{
		is.ignore();
		ignore_char(is);

		rhs.set_string(K(is));

		ignore_char(is);
	}
	else if (is.peek() == '{')
	{
		rhs.set_dictionary();
		rhs = std::move(O(is));
		ignore_char(is);

	}
	else if (is.peek() == '[')
	{
		rhs.set_list();
		rhs = std::move(A(is));
		ignore_char(is);

	}
	else
	{
		throw json_exception();
	}


	return rhs;
}

std::string K(std::istream& is)
{

	char c = ' ';
	c = is.peek();
	is.ignore();
	std::string s = "";
	char last_char = ' ';

	while (!(c == '"' &&  last_char != '\\') && !is.eof())
	{
		last_char = c;
		s += c;
		c = is.peek();
		is.ignore();
	}

	if (is.eof() && c != '"')
		throw json_exception();

	return s;
}

std::string N(std::istream& is)
{
	char c = ' ';
	c = is.peek();
	is.ignore();
	std::string s = "";

	while (((c >= 48 && c <= 57) || c == '.' || c== '-' || c == '+') && !is.eof())
	{
		s += c;
		c = is.peek();
		is.ignore();
	}

	// da inserire i controlli sul parsing 
	// ecezzioni
	if (c == ',' || c == ']' || c == '}')
	{
		is.putback(c);
	}

	ignore_char(is);
	c = is.peek();

	if (!((c >= 48 && c <= 57) || c == '.' || c == '-' || c == '+' || c == '}' || c == ']' || c == ',') && !is.eof())
	{		
		throw json_exception();

	}
		
	return s;
}

void ignore_char(std::istream& is)
{
	char c = ' ';

	c = is.peek();

	while (!is.eof() && (c == '\n' || c == '\t' || c == '\r' || c == '\a' || c == '\b' || 
            c == '\f' || c == '\v' || c == ' '))
	{
		is.ignore();
		c = is.peek();
	}
}

json::json()
{
	this->pimpl = new impl;
}

json::json(const json& other) :json()
{
	*this = other;
}

json::~json()
{
	delete pimpl;
}

json::json(json&& js):json()
{
	//copio tutto
	*this = std::move(js);
}

json& json::operator=(json const& js)
{
	if (this == &js)
		return *this;

	// cancelllo tutto

	delete this->pimpl;

	this->pimpl = new impl();

	this->pimpl->type = js.pimpl->type;
	this->pimpl->string_value = js.pimpl->string_value;
	this->pimpl->bool_value = js.pimpl->bool_value;
	this->pimpl->number_value = js.pimpl->number_value;

	if (js.pimpl->type == ARRAY_TYPE)
	{
		json::impl::ListNode* head = js.pimpl->list_head;
		while (head != nullptr)
		{
			this->push_back(head->value);
			head = head->next;
		}
	}

	if (js.pimpl->type == OBJECT_TYPE)
	{
		json::impl::DictNode* head_dict = js.pimpl->dict_head;

		while (head_dict != nullptr)
		{
			this->insert(head_dict->value);
			head_dict = head_dict->next;
		}
	}
	

	return *this;
}


json& json::operator=(json&& js)
{
	// cancello tutto

	if (this == &js)
		return *this;

	// cancelllo tutto

	delete this->pimpl;

	this->pimpl = new impl();

	this->pimpl->type = js.pimpl->type;
	this->pimpl->string_value = js.pimpl->string_value;
	this->pimpl->bool_value = js.pimpl->bool_value;
	this->pimpl->number_value = js.pimpl->number_value;


	json::impl::ListNode* head = js.pimpl->list_head;
	while (head != nullptr)
	{
		this->push_back(head->value);
		head = head->next;
	}

	json::impl::DictNode* head_dict = js.pimpl->dict_head;

	while (head_dict != nullptr)
	{
		this->insert(head_dict->value);
		head_dict = head_dict->next;
	}

	return *this;
}

void json::insert(std::pair<std::string, json> const& js)
{
	if (!this->is_dictionary())
		throw json_exception();

	json::impl::DictNode* node = this->pimpl->dict_head;
	
	if (node == nullptr)
	{
		json::impl::DictNode* new_node = new json::impl::DictNode(js.first,js.second);
		this->pimpl->dict_head = new_node;
		this->pimpl->dict_tail = new_node;
	}
	else
	{
		json::impl::DictNode* new_node = new json::impl::DictNode(js.first, js.second);
		this->pimpl->dict_tail->next = new_node;
		this->pimpl->dict_tail = new_node;
	}

}

void json::set_string(std::string const& str)
{
	this->pimpl->deleteDict();
	this->pimpl->deleteList();

	this->pimpl->type = STRING_TYPE;
	this->pimpl->string_value = str;
}

void json::set_bool(bool bl)
{
	this->pimpl->deleteDict();
	this->pimpl->deleteList();

	this->pimpl->type = BOOL_TYPE;
	this->pimpl->bool_value = bl;
}

void json::set_number(double num)
{
	this->pimpl->deleteDict();
	this->pimpl->deleteList();

	this->pimpl->type = DOUBLE_TYPE;
	this->pimpl->number_value = num;
}

void json::set_null() 
{
	this->pimpl->deleteDict();
	this->pimpl->deleteList();

	this->pimpl->type = NULL_TYPE;
}
void json::set_list()
{
	this->pimpl->deleteDict();

	this->pimpl->type = ARRAY_TYPE;
}
void json::set_dictionary()
{
	this->pimpl->deleteDict();
	
	this->pimpl->type = OBJECT_TYPE;
}
void json::push_front(json const& js)
{
	if (!this->is_list())
	{
		throw json_exception();
	}

	json::impl::ListNode* head = this->pimpl->list_head;
	
	if (head == nullptr)
	{
		json::impl::ListNode* new_node = new json::impl::ListNode(js);
		this->pimpl->list_head = new_node;
		this->pimpl->list_tail = new_node;
	}
	else
	{
		json::impl::ListNode* new_node = new json::impl::ListNode(js);
		new_node->next = this->pimpl->list_head;
		this->pimpl->list_head = new_node;
	}
}

void json::push_back(json const& js)
{
	if (!this->is_list())
		throw json_exception();

	json::impl::ListNode* head = this->pimpl->list_head;

	if (head == nullptr)
	{
		json::impl::ListNode* new_node = new json::impl::ListNode(js);
		this->pimpl->list_head = new_node;
		this->pimpl->list_tail = new_node;
	}
	else
	{
		json::impl::ListNode* new_node = new json::impl::ListNode(js);
		this->pimpl->list_tail->next = new_node;
		this->pimpl->list_tail = new_node;
	}
}


bool json::is_list() const
{
	return this->pimpl->type == ARRAY_TYPE;
}
bool json::is_dictionary() const
{
	return this->pimpl->type == OBJECT_TYPE;
}
bool json::is_string() const
{
	return this->pimpl->type == STRING_TYPE;
}
bool json::is_number() const
{
	return this->pimpl->type == DOUBLE_TYPE;
}
bool json::is_bool() const
{
	return this->pimpl->type == BOOL_TYPE;
}
bool json::is_null() const
{
	return this->pimpl->type == NULL_TYPE;
}


double& json::get_number()
{
	if (this->is_number())
		return this->pimpl->number_value;
	else
		throw json_exception();
}

double const& json::get_number() const
{
	if (this->is_number())
		return this->pimpl->number_value;
	else
		throw json_exception();
}

bool& json::get_bool()
{
	if (this->is_bool())
		return this->pimpl->bool_value;
	else
		throw json_exception();
}

bool const& json::get_bool() const
{
	if (this->is_bool())
		return this->pimpl->bool_value;
	else
		throw json_exception();
}

std::string& json::get_string()
{
	if (this->is_string())
		return this->pimpl->string_value;
	else
		throw json_exception();
}

std::string const& json::get_string() const
{
	if (this->is_string())
		return this->pimpl->string_value;
	else
		throw json_exception();
}

struct json::const_list_iterator {

	using iterator_category = std::forward_iterator_tag;
	using value_type = const json;
	using pointer = json const*;
	using reference = json const&;

	const_list_iterator(json::impl::ListNode* ptr);

	reference operator*() const;
	pointer operator->() const;
	const_list_iterator& operator++();
	const_list_iterator operator++(int);
	bool operator==(const_list_iterator const& rhs) const;
	bool operator!=(const_list_iterator const& rhs) const;

private:
	json::impl::ListNode const* ptr;

};


struct json::list_iterator {

	using iterator_category = std::forward_iterator_tag;
	using value_type = json;
	using pointer = json*;
	using reference = json&;

	list_iterator(json::impl::ListNode* ptr);

	reference operator*() const;
	pointer operator->() const;
	list_iterator& operator++();
	list_iterator operator++(int);
	bool operator==(list_iterator const& rhs) const;
	bool operator!=(list_iterator const& rhs) const;


private:
	json::impl::ListNode* ptr;

};


json::list_iterator json::begin_list()
{
	if (!this->is_list())
	{
		throw json_exception();
	}

	return { this->pimpl->list_head };

}

json::const_list_iterator json::begin_list() const
{
	if (!this->is_list())
	{
		throw json_exception();
	}

	return { this->pimpl->list_head };
}


json::list_iterator json::end_list()
{
	if (!this->is_list())
	{
		throw json_exception();
	}

	return { nullptr };
}

json::const_list_iterator json::end_list() const
{
	if (!this->is_list())
	{
		throw json_exception();
	}

	return { nullptr };
}


json::list_iterator::list_iterator(json::impl::ListNode* ptr)
{
	this->ptr = ptr;
}

json::list_iterator::reference json::list_iterator::operator*() const
{
	return this->ptr->value;
}

json::list_iterator::pointer json::list_iterator::operator->() const
{
	return &(this->ptr->value);
}

json::list_iterator& json::list_iterator::operator++()
{
	ptr = ptr->next;
	return *this;
}

json::list_iterator json::list_iterator::operator++(int)
{
	list_iterator copy = *this;
	ptr = ptr->next;
	return copy;
}



json::const_list_iterator::const_list_iterator(json::impl::ListNode* ptr)
{
	this->ptr = ptr;
}

json::const_list_iterator::reference json::const_list_iterator::operator*() const
{
	return this->ptr->value;
}

json::const_list_iterator::pointer json::const_list_iterator::operator->() const
{
	return &(this->ptr->value);
}

json::const_list_iterator& json::const_list_iterator::operator++()
{
	ptr = ptr->next;
	return *this;
}

json::const_list_iterator json::const_list_iterator::operator++(int)
{
	const_list_iterator copy = *this;
	ptr = ptr->next;
	return copy;
}

bool json::const_list_iterator::operator==(const_list_iterator const& rhs) const
{
	return ptr == rhs.ptr;
}

bool json::const_list_iterator::operator!=(const_list_iterator const& rhs) const
{
	return ptr != rhs.ptr;
}

bool json::list_iterator::operator==(list_iterator const& rhs) const
{
	return ptr == rhs.ptr;
}

bool json::list_iterator::operator!=(list_iterator const& rhs) const
{
	return ptr != rhs.ptr;
}



struct json::const_dictionary_iterator {

	using iterator_category = std::forward_iterator_tag;
	using value_type = const std::pair<std::string, json>;
	using pointer = std::pair<std::string, json> const*;
	using reference = std::pair<std::string, json> const&;

	const_dictionary_iterator(json::impl::DictNode* ptr);

	reference operator*() const;
	pointer operator->() const;
	const_dictionary_iterator& operator++();
	const_dictionary_iterator operator++(int);
	bool operator==(const_dictionary_iterator const& rhs) const;
	bool operator!=(const_dictionary_iterator const& rhs) const;

private:
	json::impl::DictNode const* ptr;

};


struct json::dictionary_iterator {

	using iterator_category = std::forward_iterator_tag;
	using value_type = std::pair<std::string,json>;
	using pointer = std::pair<std::string, json>*;
	using reference = std::pair<std::string, json>&;

	dictionary_iterator(json::impl::DictNode* ptr);

	reference operator*() const;
	pointer operator->() const;
	dictionary_iterator& operator++();
	dictionary_iterator operator++(int);
	bool operator==(dictionary_iterator const& rhs) const;
	bool operator!=(dictionary_iterator const& rhs) const;


private:
	json::impl::DictNode* ptr;

};

json::dictionary_iterator::dictionary_iterator(json::impl::DictNode* ptr)
{
	this->ptr = ptr;
}

json::dictionary_iterator::reference json::dictionary_iterator::operator*() const
{
	return this->ptr->value;
}

json::dictionary_iterator::pointer json::dictionary_iterator::operator->() const
{
	return &(this->ptr->value);
}

json::dictionary_iterator& json::dictionary_iterator::operator++()
{
	ptr = ptr->next;
	return *this;
}

json::dictionary_iterator json::dictionary_iterator::operator++(int)
{
	dictionary_iterator copy = this->ptr;
	ptr = ptr->next;
	return copy;
}

bool json::dictionary_iterator::operator==(dictionary_iterator const& rhs) const
{
	return ptr == rhs.ptr;
}

bool json::dictionary_iterator::operator!=(dictionary_iterator const& rhs) const
{
	return ptr != rhs.ptr;
}


bool json::const_dictionary_iterator::operator==(const_dictionary_iterator const& rhs) const
{
	return ptr == rhs.ptr;
}

bool json::const_dictionary_iterator::operator!=(const_dictionary_iterator const& rhs) const
{
	return ptr != rhs.ptr;
}

json::const_dictionary_iterator::const_dictionary_iterator(json::impl::DictNode* ptr)
{
	this->ptr = ptr;
}

json::const_dictionary_iterator::reference json::const_dictionary_iterator::operator*() const
{
	return this->ptr->value;
}

json::const_dictionary_iterator::pointer json::const_dictionary_iterator::operator->() const
{
	return &(this->ptr->value);
}

json::const_dictionary_iterator& json::const_dictionary_iterator::operator++()
{
	ptr = ptr->next;
	return *this;
}

json::const_dictionary_iterator json::const_dictionary_iterator::operator++(int)
{
	const_dictionary_iterator copy = *this;
	ptr = ptr->next;
	return copy;
}


json::dictionary_iterator json::begin_dictionary()
{
	if (!this->is_dictionary())
	{
		throw json_exception();
	}

	return { this->pimpl->dict_head };
}

json::const_dictionary_iterator json::begin_dictionary() const
{
	if (!this->is_dictionary())
	{
		throw json_exception();
	}

	return { this->pimpl->dict_head };
}

json::dictionary_iterator json::end_dictionary()
{
	if (!this->is_dictionary())
	{
		throw json_exception();
	}

	return { nullptr };
}

json::const_dictionary_iterator json::end_dictionary() const
{
	if (!this->is_dictionary())
	{
		throw json_exception();
	}

	return { nullptr };
}


json const& json::operator[](std::string const& key) const
{

	if (!this->is_dictionary())
		throw json_exception();
	
	auto it = begin_dictionary();
	while (it != end_dictionary())
	{
		if (it->first == key)
		{
			return it->second;
		}
		it++;
	}

	throw json_exception();
}

json& json::operator[](std::string const& key)
{
	if (!this->is_dictionary())
		throw json_exception();

	auto it = begin_dictionary();
	while (it != end_dictionary())
	{
		if (it->first == key)
		{
			return it->second;
		}
		it++;
	}	

	json js;
	js.set_null();
	std::pair<std::string, json> dict = { key,js };
	this->insert(dict);

	auto tail = begin_dictionary();
	while (tail->first != key)
		tail++;

	return tail->second;

	
}



std::ostream& operator<<(std::ostream& lhs, json const& rhs)
{
	printJson(lhs,rhs);
	return lhs;
}


void printJson(std::ostream& lhs, json const& rhs)
{
	if (rhs.is_bool())
	{
		lhs << (rhs.get_bool()?"true":"false");
	}

	if (rhs.is_null())
	{
		lhs << "null";
	}

	if (rhs.is_number())
	{
		lhs << rhs.get_number();
	}

	if (rhs.is_string())
	{
		lhs << "\"" + rhs.get_string() + "\"";
	}

	if (rhs.is_dictionary())
	{
		auto it = rhs.begin_dictionary();
		lhs << "{";

		while (it != rhs.end_dictionary())
		{
			if (it != rhs.begin_dictionary())
			{
				lhs << ',';
			}
			
			lhs << "\"" + it->first + "\"" + ":";
			printJson(lhs, it->second);

			it++;
		}

		lhs << '}';

	}

	if (rhs.is_list())
	{
		auto it = rhs.begin_list();
		lhs << "[";
		while (it != rhs.end_list())
		{
			if (it != rhs.begin_list())
				lhs << ',';

			
			printJson(lhs, *it);

			it++;
		}

		lhs << "]";
	}
}
