#include <iostream>
#include <string>
#include <array>
#include <unordered_map>
#include <fstream>

struct ListNode
{
	ListNode* Next;
	ListNode* Prev;
	std::string Data;
	ListNode* Rand;
};

typedef std::fstream FileStream;

struct SerializeStruct {
	std::string data = "";
	int random = -1;
};

typedef std::unordered_map<ListNode*, int> SerializeMarkupAssociation;
typedef std::unordered_map<int, ListNode*> DeserializeMarkupAssociation;

struct DeserializeInit
{
	std::vector<SerializeStruct> vec;
	DeserializeMarkupAssociation dma;
};

class ListRand
{
public:
	ListRand() : Head{ nullptr }, Tail{ nullptr }, Count{ 0 } {}
	
	ListNode* AddNew(std::string el, bool isRandomSelf = false, ListNode* random = nullptr) {
		Count++;
		if (Tail == nullptr) {
			Head = new ListNode{ nullptr, nullptr, el };
			Head->Rand = isRandomSelf ? Head : random;
			Tail = Head;
			return Head;
		}
		Tail->Next = new ListNode{ nullptr, Tail, el };
		Tail = Tail->Next;
		Tail->Rand = isRandomSelf ? Tail : random;
		return Tail;
	}

	// Weight exploit
	// Mark up all nodes for quick access to element indexes from their refs
	SerializeMarkupAssociation MarkupAllNodes() {
		auto current = Head;
		SerializeMarkupAssociation ma;

		for (int i = 0; i < Count; i++) {
			ma.insert(SerializeMarkupAssociation::value_type(current, i));
			current = current->Next;
		}

		delete current;

		return ma;
	}

	std::vector<SerializeStruct> SerializeSteps(SerializeMarkupAssociation ma) {
		auto current = Head;
		std::vector<SerializeStruct> vec(Count);

		ListNode* random;
		for (int i = 0; i < Count; i++) {
			random = current->Rand;
			if (random != nullptr) {
				vec.at(i).random = ma.at(random);
			}
			vec.at(i).data = current->Data;
			current = current->Next;
		}

		delete current;

		return vec;
	}

	void Serialize(std::fstream& f) {
		auto ma = MarkupAllNodes();
		std::vector<SerializeStruct> vec = SerializeSteps(ma);

		int size = vec.size();
		f << size << " ";

		for (auto el : vec) {
			f << el.data << " " << el.random << " ";
		}
	}

	DeserializeInit InitDeserializeSteps(FileStream& f) {
		int size;
		f >> size;
		std::vector<SerializeStruct> vec(size);
		DeserializeMarkupAssociation dma;

		std::string data;
		int random;
		// Another weight exploit
		// Mark up all nodes for quick access to element refs from their indexes
		for (int i = 0; i < size; i++) {
			f >> data >> random;
			dma.insert(DeserializeMarkupAssociation::value_type(i, AddNew(data)));
			vec.at(i) = { data, random };
		}

		return { vec, dma };
	}

	void Deserialize(FileStream& f) {
		auto initStruct = InitDeserializeSteps(f);
		auto vec = initStruct.vec;
		auto dma = initStruct.dma;
		auto size = vec.size();

		ListNode* current = Head;

		int random;
		for (int i = 0; i < size; i++) {
			random = vec.at(i).random;
			if (random != -1) {
				current->Rand = dma.at(random);
			}
			current = current->Next;
		}

		delete current;
	}

	~ListRand() {
		if (Count == 0) return;

		auto current = Head;
		ListNode* nextCurrent = nullptr;
		for (int i = 0; i < Count; i++) {
			nextCurrent = current->Next;
			delete current;
			current = nextCurrent;
		}

		delete current;
		delete nextCurrent;
	}
private:
	ListNode* Head;
	ListNode* Tail;
	int Count;
};

int main() {
	ListRand li;
	FileStream s;
	
	s.open("file.txt");
	li.Deserialize(s);
	s.close();

	s.open("file1.txt", std::ofstream::out | std::ofstream::trunc);
	li.Serialize(s);
	s.close();

	return 0;
}