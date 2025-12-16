// SCSTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <cassert>
#include "QueueManager.h"
#include <queue>

#define EXPECT_EQ(a, b) if((a) != (b)) {std::cout << "Expected " << b << " actual " << a << " on line " << __LINE__ << "\n";}
#define EXPECT_TRUE(a)  if(!(a)) {std::cout << "On line " <<__LINE__ << " expected true value, but was false\n";}
#define EXPECT_FALSE(a) if(a) {std::cout << "On line " <<__LINE__ << " expected false value, but was true\n";}

void Test_CreateDestroyQueue(QueueManager* qm) {
	QueueManager::QHandle* q = qm->create_queue();
	EXPECT_TRUE(q != nullptr);

	qm->destroy_queue(q);

	// Recreate should reuse the slot
	QueueManager::QHandle* q2 = qm->create_queue();

	EXPECT_TRUE(q2 != nullptr);
	EXPECT_EQ(q2, q);
	qm->destroy_queue(q2);
}
void Test_SingleQueueFIFO(QueueManager* qm) {

	QueueManager::QHandle* q = qm->create_queue();

	qm->enqueue_byte(q, 'a');
	qm->enqueue_byte(q, 'b');
	qm->enqueue_byte(q, 'c');

	EXPECT_EQ(qm->dequeue_byte(q), 'a');
	EXPECT_EQ(qm->dequeue_byte(q), 'b');
	EXPECT_EQ(qm->dequeue_byte(q), 'c');

	qm->destroy_queue(q);
}
void Test_MultipleQueues(QueueManager* qm) {


	QueueManager::QHandle* q0 = qm->create_queue();
	QueueManager::QHandle* q1 = qm->create_queue();

	qm->enqueue_byte(q0, 0);
	qm->enqueue_byte(q0, 1);
	qm->enqueue_byte(q1, 3);
	qm->enqueue_byte(q0, 2);
	qm->enqueue_byte(q1, 4);

	EXPECT_EQ(qm->dequeue_byte(q0), 0);
	EXPECT_EQ(qm->dequeue_byte(q0), 1);

	qm->enqueue_byte(q0, 5);
	qm->enqueue_byte(q1, 6);

	EXPECT_EQ(qm->dequeue_byte(q0), 2);
	EXPECT_EQ(qm->dequeue_byte(q0), 5);

	EXPECT_EQ(qm->dequeue_byte(q1), 3);
	EXPECT_EQ(qm->dequeue_byte(q1), 4);
	EXPECT_EQ(qm->dequeue_byte(q1), 6);

	qm->destroy_queue(q0);
	qm->destroy_queue(q1);
}
void Test_SegmentOverflow(QueueManager* qm) {

	QueueManager::QHandle* q = qm->create_queue();
	for (int i = 0; i < 40 * 3; ++i) {
		qm->enqueue_byte(q, static_cast<unsigned char>(i));
	}

	for (int i = 0; i < 40 * 3; ++i) {
		EXPECT_EQ(qm->dequeue_byte(q), static_cast<unsigned char>(i));
	}

	qm->destroy_queue(q);
}
void Test_IllegalOperations(QueueManager* qm) {


	std::cout << "In the next area need to be 2 errors\n";
	std::cout << "------------- TEST AREA ------\n";
	// dequeue from invalid queue
	qm->dequeue_byte(nullptr);

	auto* q = qm->create_queue();
	qm->destroy_queue(q);

	// enqueue on destroyed queue
	qm->enqueue_byte(q, 10);
	std::cout << "------------- TEST AREA END------\n";

}
void PerformRandomAllocation(QueueManager* customQueue) {
	std::queue<unsigned char> queues[4];
	QueueManager::QHandle* handles[4];
	int queuesCount = 4;
	for (int i = 0; i < queuesCount; i++)
	{
		handles[i] = customQueue->create_queue();
	}
	for (int x = 0; x < 200; x++)
	{
		//if (x == 6)customQueue.dequeue_byte(h1);
		for (int i = 0; i < 200; i++)
		{
			int qNum = std::rand() % queuesCount;
			queues[qNum].push(i);
			customQueue->enqueue_byte(handles[qNum], i);
		}
		
		for (int i = 0; i < 200; i++)
		{
			int qNum = std::rand() % queuesCount;
			if (queues[qNum].empty()) continue;
			unsigned char v1 = queues[qNum].front();
			queues[qNum].pop();
			unsigned char v2 = customQueue->dequeue_byte(handles[qNum]);
			if (v1 != v2) {
				std::cout << "v1 and v2 are not the same!\n";
				printf("Happened at x = %d and i = %d, for queue %d \n", x, i, qNum);
				;
			}
		}
	}

	for (int i = 0; i < queuesCount; i++)
	{
		customQueue->destroy_queue(handles[i]);
	}
}

int main()
{
	QueueManager customQueue = QueueManager::QueueManager();

	EXPECT_EQ(sizeof(customQueue) , 2048);
	// Performing self written tests
	Test_CreateDestroyQueue(&customQueue);
	Test_SingleQueueFIFO(&customQueue);
	Test_MultipleQueues(&customQueue);
	Test_IllegalOperations(&customQueue);
	PerformRandomAllocation(&customQueue);
	
	// Test provided in the email
	QueueManager::QHandle* q0 = customQueue.create_queue();
	customQueue.enqueue_byte(q0, 0);
	customQueue.enqueue_byte(q0, 1);
	QueueManager::QHandle* q1 = customQueue.create_queue();
	customQueue.enqueue_byte(q1, 3);
	customQueue.enqueue_byte(q0, 2);
	customQueue.enqueue_byte(q1, 4);
	printf("%d", customQueue.dequeue_byte(q0));
	printf("%d\n", customQueue.dequeue_byte(q0));
	customQueue.enqueue_byte(q0, 5);
	customQueue.enqueue_byte(q1, 6);
	printf("%d", customQueue.dequeue_byte(q0));
	printf("%d\n", customQueue.dequeue_byte(q0));
	customQueue.destroy_queue(q0);
	printf("%d", customQueue.dequeue_byte(q1));
	printf("%d", customQueue.dequeue_byte(q1));
	printf("%d\n", customQueue.dequeue_byte(q1));
	customQueue.destroy_queue(q1);
}