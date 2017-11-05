#include "attackMessage.h"

AttackMessager::AttackMessager(Player * inputPlayer, std::vector<Enemy*>* inputEnemies) {
	player = inputPlayer;
	enemies = inputEnemies;
	head = nullptr;
}

AttackMessager::~AttackMessager() {
	// clear the linked list
	while (head != nullptr) {
		LinkedListQueue * temp = head;
		head = head->next;
		delete temp;
	}
}

void AttackMessager::addMessage(AttackMessage message) {
	LinkedListQueue * newMessage = new LinkedListQueue();
	newMessage->next = head;
	newMessage->message = message;
	head = newMessage;
}

bool AttackMessager::hasMessage() const {
	return head != nullptr;
}

AttackMessage AttackMessager::dequeue() {
	LinkedListQueue * temp = head;
	AttackMessage tempMessage = temp->message;
	head = head->next;
	delete temp;
	return tempMessage;
}
