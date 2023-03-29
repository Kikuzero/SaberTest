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

// List<> here because we can have multiple random connections to a single element
// That's why in worst scenario this algorithm has time complexity of O(N^2)
// in best (without any multiple random connection to a single element) O(N)
typedef std::unordered_map<ListNode*, std::list<int>> SerializeStoreMap;
typedef std::unordered_map<int, std::list<ListNode*>> DeserializeStoreMap;

typedef std::fstream FileStream;

struct SerializeStruct {
	std::string data = "";
	int forward = -1;
	int backward = -1;
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

	// Forward step checks forward Rand connections of Nodes
	std::vector<SerializeStruct> SerializeForwardSteps() {
		ListNode* fCurrent = Head;

		SerializeStoreMap store;
		std::vector<SerializeStruct> vec(Count);

		ListNode* rand;
		for (int i = 0; i < Count; i++) {
			rand = fCurrent->Rand;
			if (rand != nullptr) {
				if (store.find(rand) != store.end()) {
					store.at(rand).push_back(i);
				}
				else {
					store.insert(SerializeStoreMap::value_type(rand, std::list<int>{ i }));
				}
			}
			if (store.find(fCurrent) != store.end()) {
				auto li = store.at(fCurrent);
				for (auto el : li) {
					vec.at(el).forward = i;
				}
				store.erase(fCurrent);
			}
			vec.at(i).data = fCurrent->Data;
			fCurrent = fCurrent->Next;
		}

		return vec;
	}

	// Backward step checks backward Rand connections of Nodes
	std::vector<SerializeStruct> SerializeBackwardSteps(std::vector<SerializeStruct> vec) {
		ListNode* bCurrent = Tail;

		SerializeStoreMap store;

		ListNode* rand;
		for (int i = Count - 1; i >= 0; i--) {
			rand = bCurrent->Rand;
			if (rand != nullptr) {
				if (store.find(rand) != store.end()) {
					store.at(rand).push_back(i);
				}
				else {
					store.insert(SerializeStoreMap::value_type(rand, std::list<int>{ i }));
				}
			}
			if (store.find(bCurrent) != store.end()) {
				auto li = store.at(bCurrent);
				for (auto el : li) {
					vec.at(el).backward = i;
				}
				store.erase(bCurrent);
			}
			vec.at(i).data = bCurrent->Data;
			bCurrent = bCurrent->Prev;
		}

		return vec;
	}

	void Serialize(std::fstream& f) {
		std::vector<SerializeStruct> vec = SerializeBackwardSteps(SerializeForwardSteps());

		int size = vec.size();
		f << size << " ";

		for (auto el : vec) {
			f << el.data << " " << el.forward << " " << el.backward << " ";
		}
	}

	std::vector<SerializeStruct> InitDeserizlizeSteps(std::fstream& f) {
		int size;
		f >> size;
		std::vector<SerializeStruct> vec(size);
	
		std::string data;
		int forward, backward;
		for (int i = 0; i < size; i++) {
			f >> data >> forward >> backward;
			AddNew(data);
			vec.at(i) = { data, forward, backward };
		}

		return vec;
	}

	void Deserialize(std::fstream& f) {
		std::vector<SerializeStruct> vec = InitDeserizlizeSteps(f);
		auto size = vec.size();

		DeserializeStoreMap fStore;
		DeserializeStoreMap bStore;

		ListNode* fCurrent = Head;
		ListNode* bCurrent = Tail;

		//Forward steps of deserialization
		int forward;
		for (int i = 0; i < size; i++) {
			forward = vec.at(i).forward;
			if (forward != -1) {
				if (fStore.find(forward) != fStore.end()) 
					fStore.at(forward).push_back(fCurrent);
				else 
					fStore.insert(DeserializeStoreMap::value_type(forward, std::list<ListNode*>{fCurrent}));
			}
			if (fStore.find(i) != fStore.end()) {
				auto li = fStore.at(i);
				for (auto el : li) 	el->Rand = fCurrent;
				fStore.erase(i);
			}
			fCurrent = fCurrent->Next;
		}

		//Backward steps of deserialization
		int backward;
		for (int i = size - 1; i >= 0; i--) {
			backward = vec.at(i).backward;
			if (backward != -1) {
				if (bStore.find(backward) != bStore.end())
					bStore.at(backward).push_back(bCurrent);
				else
					bStore.insert(DeserializeStoreMap::value_type(backward, std::list<ListNode*>{bCurrent}));
			}
			if (bStore.find(i) != bStore.end()) {
				auto li = bStore.at(i);
				for (auto el : li) 	el->Rand = bCurrent;
				bStore.erase(i);
			}
			bCurrent = bCurrent->Prev;
		}
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

	s.open("file1.txt");
	s.clear();
	li.Serialize(s);
	s.close();

	return 0;
}