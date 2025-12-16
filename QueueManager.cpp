#include <iostream>
#include "QueueManager.h";


void QueueManager::on_illegal_operation() {
	std::cout << "\n ON ILLEGAL OPERATION \n";
}
void QueueManager::on_out_of_memory() {
	std::cout << "\n ON OUT OF MEMORY \n";
}

//*******************************************************************************
uint8_t QueueManager::read_UInt8(unsigned char* readPos) {
	return readPos[0];
}
void QueueManager::write_UInt8(unsigned char* writePos, const uint8_t value) {
	writePos[0] = value;
}
void QueueManager::write_UInt16(unsigned char* writePlace, const uint16_t value) {
	writePlace[0] = static_cast<unsigned char>(value);
	writePlace[1] = static_cast<unsigned char>(value >> 8);
}
void QueueManager::write_UInt16(const uint16_t writePlace, const uint16_t value)
{
	if (writePlace + 1 >= DataSize) on_out_of_memory();
	data[writePlace] = static_cast<unsigned char>(value);
	data[writePlace + 1] = static_cast<unsigned char>(value >> 8);
}
uint16_t QueueManager::ReadUInt16(const unsigned char* uchar) {
	return static_cast<uint16_t>((unsigned)uchar[0] | (uchar[1] << 8));
}

//Manager functions
void QueueManager::set_free_area_tail(uint16_t position) {
	freeAreaHeader()->tailPos = position;
}
uint16_t QueueManager::get_free_area_tail() {
	return freeAreaHeader()->tailPos;
}
void QueueManager::set_free_area_head(uint16_t position) {
	freeAreaHeader()->headPos = position;
}
uint16_t QueueManager::get_free_area_head() {
	return freeAreaHeader()->headPos;
}
//Header functions
bool QueueManager::get_queue_in_use(QHandle* q) {
	return (q->inUse != 0);
}
void QueueManager::set_queue_in_use(QHandle* q, bool isInUse) {
	q->inUse = isInUse ? 1 : 0;
}
uint16_t QueueManager::get_header_tail(QHandle* q) {

	return q->tailPos;
}
void QueueManager::ser_header_tail(QHandle* q, uint16_t tailPos) {
	q->tailPos = tailPos;
}
uint16_t QueueManager::get_header_head(QHandle* q) {
	return q->headPos;
}
void QueueManager::set_header_head(QHandle* q, uint16_t headPos) {
	q->headPos = headPos;
}
//Segment functions
void QueueManager::set_segment_data_count(uint16_t segmentPos, uint8_t count) {
	segmentByPos(segmentPos)->count = count;
}
uint8_t QueueManager::get_segment_data_count(uint16_t segmentPos) {
	return segmentByPos(segmentPos)->count;
}
uint16_t QueueManager::get_segment_next(uint16_t currentSegment) {
	return segmentByPos(currentSegment)->next;
}
void QueueManager::set_segment_next(uint16_t currentSegment, uint16_t nextSegment) {
	segmentByPos(currentSegment)->next = nextSegment;
}
uint16_t QueueManager::get_segment_prev(uint16_t currentSegment) {
	return segmentByPos(currentSegment)->prev;
}
void QueueManager::set_segment_prev(uint16_t currentSegment, uint16_t prevSegment) {
	segmentByPos(currentSegment)->prev = prevSegment;
}
void QueueManager::set_segment_owner(uint16_t segmentPos, uint8_t owner) {
	segmentByPos(segmentPos)->owner = owner;
}
uint8_t QueueManager::get_segment_owner(uint16_t segmentPos) {
	return segmentByPos(segmentPos)->owner;
}
void QueueManager::set_segment_read_off(uint16_t segmentPos, uint8_t off) {
	segmentByPos(segmentPos)->readOff = off;
}
uint8_t QueueManager::get_segment_read_off(uint16_t segmentPos) {
	return segmentByPos(segmentPos)->readOff;
}

void QueueManager::print_segment(uint16_t pos) {

	printf("%d is read pos\n", get_segment_read_off(pos));
	printf("%d is data count\n", get_segment_data_count(pos));
	printf("%d is next\n", get_segment_next(pos));
	printf("%d is prev\n", get_segment_prev(pos));
	printf("%d is owner\n", get_segment_owner(pos));
	std::cout << "\n";
}
/// <summary>
/// Function to calculate how much memory from the buffer would take
/// 15 queues with 80 bytes each or 64 queues with 1 byte
/// </summary>
/// <param name="x"></param>
void QueueManager::get_result_size_of_whole_queue(int x) {
	int nubmerOfSegments = std::ceil(80. / (x - SegmentHeaderSize));
	int bytesNeeded = (x - SegmentHeaderSize) * nubmerOfSegments;
	bytesNeeded = bytesNeeded + (nubmerOfSegments * SegmentHeaderSize);
	std::cout << 15 * bytesNeeded << " bytes needed for 15 queues of 80 bytes\n";
	std::cout << 64 * x << " size for 64 queues";
}
void QueueManager::print_read_and_byte_off(uint16_t pos) {
	if (pos == 65535) return;
	uint16_t prev = get_segment_read_off(pos);
	uint16_t next = get_segment_data_count(pos);

	printf("For position %d:\n", pos);
	printf("read %d\n", prev);
	printf("bytes %d\n", next);
}
void QueueManager::do_full_segment_check(uint16_t segment)
{
	uint16_t nextSegm = get_segment_next(segment);
	uint16_t prevSegm = get_segment_prev(segment);
	uint8_t owner = get_segment_owner(segment);
	if (get_header_head(queueHeader(owner)) == UINT16_MAX) {
		printf("We look at segment from %d header but Head is null\n", owner);
	}
	if (get_header_tail(queueHeader(owner)) == UINT16_MAX) {
		printf("We look at segment from %d header but Tail is null\n", owner);
	}
	if (prevSegm != UINT16_MAX && get_segment_next(prevSegm) != segment) {
		printf("We look at segment from %d header and prev and this segments do not allign\n", owner);
	}
	if (nextSegm != UINT16_MAX && get_segment_prev(nextSegm) != segment) {
		printf("We look at segment from %d header and next and this segments do not allign\n", owner);

	}
}
QueueManager::QueueHeader* QueueManager::queueHeader(int index)
{
	return (QueueHeader*)(data)+index;
}
QueueManager::FreeAreaHeader* QueueManager::freeAreaHeader()
{
	return (FreeAreaHeader*)(data + FreeAreaHeaderOffset);
}
QueueManager::Segment* QueueManager::segmentByPos(uint16_t pos)
{
	return(Segment*)(data + pos);
}
//*******************************************************************************

/// <summary>
/// A must call before we can use buffer at all
/// </summary>
void QueueManager::initialize() {
	std::memset(data, UCHAR_MAX, sizeof(data));
	for (uint8_t i = 0; i < MaxQueues; i++)
	{

		set_queue_in_use(queueHeader(i), false);
	}
	set_free_area_tail(BufferOffset);
	set_free_area_head(BufferOffset);
}
bool QueueManager::ensure_free_memory(uint16_t* managerHead) {
	uint16_t managerTail = get_free_area_tail();
	if (*managerHead == managerTail && get_segment_owner(managerTail) != UINT8_MAX) {
		on_out_of_memory();
		return false;
	}
	//Check if manager head is at the free slot
	if (DataSize - *managerHead < SegmentMaxSize) {
		*managerHead = BufferOffset;
		if (*managerHead == managerTail && get_segment_owner(*managerHead) != UINT8_MAX) {
			on_out_of_memory();
			return false;
		}
	}
	return true;
}
/// <summary>
/// Use when freeing segment, copies manager tail segment to the freed space 
/// </summary>
/// <param name="targetPos">Position to move segment to</param>
void QueueManager::move_tail_segment_to_pos(uint16_t targetPos) {
	uint16_t tailSegment = get_free_area_tail();
	// Check if we just deleted the segment at tail
	if (tailSegment == targetPos) {
		uint16_t newTailPos = tailSegment + SegmentMaxSize;
		if (newTailPos + SegmentMaxSize >= DataSize) {
			newTailPos = BufferOffset;
		}
		set_free_area_tail(newTailPos);
		return;
	}
	if (tailSegment == UINT16_MAX || tailSegment < BufferOffset || tailSegment >= DataSize) {
		on_illegal_operation();
		return;
	}

	uint16_t prevTailSegment = get_segment_prev(tailSegment);
	uint16_t nextTailSegment = get_segment_next(tailSegment);
	uint8_t owner = get_segment_owner(tailSegment);
	//Copying segment to another place and erasing it from previous
	std::memcpy(segmentByPos(targetPos), segmentByPos(tailSegment), SegmentMaxSize);
	std::memset(segmentByPos(tailSegment), UINT8_MAX, SegmentMaxSize);
	// Update references
	if (prevTailSegment != UINT16_MAX) set_segment_next(prevTailSegment, targetPos);
	if (nextTailSegment != UINT16_MAX) {
		set_segment_prev(nextTailSegment, targetPos);
	}
	if (get_header_head(queueHeader(owner)) == tailSegment) set_header_head(queueHeader(owner), targetPos);
	if (get_header_tail(queueHeader(owner)) == tailSegment) ser_header_tail(queueHeader(owner), targetPos);
	// Update manager tail position
	uint16_t newTailPos = tailSegment + SegmentMaxSize;
	if (newTailPos + SegmentMaxSize >= 2048) {
		newTailPos = BufferOffset;
	}
	set_free_area_tail(newTailPos);
}

void QueueManager::UnitTest() {
	QHandle* q1 = create_queue();
	QHandle* q2 = create_queue();

	//std::cout << (read_UInt8(&data[0] + H_IN_USE_OFF) != 0) << " - queue one should be in use (true)\n";
	std::cout << (queueHeader(0)->inUse != 0) << " - queue one should be in use (true)\n";
	enqueue_byte(q1, 'h');

	//std::cout << (ReadUInt16(&data[0] + H_TAIL_OFF) == BufferOffset) << " - is segment created at the beginning of the buffer?\n";
	std::cout << (queueHeader(0)->tailPos == BufferOffset) << " - is segment created at the beginning of the buffer?\n";
	std::cout << (read_UInt8(&data[BufferOffset] + SegmentHeaderSize)) << " - checking data we wrote, should be 'h'\n";

	destroy_queue(q1);
	std::cout << (queueHeader(0)->inUse == 0) << " - was queue destroyed?\n";
	QHandle* q3 = create_queue();
	std::cout << (((QHandle*)data - q3) == 0) << " - was queue created at the right location?\n";
}
void QueueManager::enqueue_byte(QHandle* q, unsigned char b) {
	if (q == nullptr || !get_queue_in_use(q)) {
		on_illegal_operation();
		return;
	}
	uint16_t QueueHead = get_header_head(q);

	//Case 1 - If there is no head in the Q yet
	if (QueueHead == UINT16_MAX) {
		uint16_t managerHead = get_free_area_head();

		//Check if manager head is at the free slot
		if (!ensure_free_memory(&managerHead)) return;
		// Setting segment block to the default state
		set_segment_read_off(managerHead, 0);
		set_segment_data_count(managerHead, 1);
		set_segment_next(managerHead, UINT16_MAX);
		set_segment_prev(managerHead, UINT16_MAX);
		set_segment_owner(managerHead, (q - ((QHandle*)data)));

		segmentByPos(managerHead)->data[0] = b;
		// Marking in the header that this is currently the only segment
		set_header_head(q, managerHead);
		ser_header_tail(q, managerHead);

		set_free_area_head(managerHead + SegmentMaxSize);
		return;

	}
	//----------------
	//Case 2 - If there is head in queue
	else {
		uint8_t byteCount = get_segment_data_count(QueueHead);
		// Checking is there is free space in the current segment
		if (byteCount < SegmentBufferSize) {
			write_UInt8(&data[QueueHead + SegmentHeaderSize + byteCount], b);

			set_segment_data_count(QueueHead, byteCount + 1);
			return;
		}
		// Initializing new segment for current header
		else {
			uint16_t managerHead = get_free_area_head();
			if (!ensure_free_memory(&managerHead)) return;

			segmentByPos(managerHead)->data[0] = b;

			set_segment_read_off(managerHead, 0);
			set_segment_data_count(managerHead, 1);
			set_segment_next(managerHead, UINT16_MAX);
			set_segment_prev(managerHead, QueueHead);
			set_segment_owner(managerHead, (q - ((QHandle*)data)));
			set_segment_next(QueueHead, managerHead);

			set_free_area_head(managerHead + SegmentMaxSize);
			set_header_head(q, managerHead);

			return;
		}
	}
}
unsigned char QueueManager::dequeue_byte(QHandle* q) {
	if (q == nullptr || !get_queue_in_use(q)) {
		on_illegal_operation();
		return UINT8_MAX;
	}
	uint16_t headerTail = get_header_tail(q);
	// Case 1 - If there is no tail
	if (headerTail == UINT16_MAX) {
		on_illegal_operation();
		return UINT8_MAX;
	}
	// Case 2 - If there is a tail
	else {
		uint8_t readOff = get_segment_read_off(headerTail);
		uint8_t byteCount = get_segment_data_count(headerTail);
		// Check if our read pointer surpassed the amout of data we have
		if (readOff >= byteCount) {
			on_illegal_operation();
			return UINT8_MAX;
		}
		
		unsigned char resultChar = segmentByPos(headerTail)->data[readOff];
		// Erasing red value from the queue
		segmentByPos(headerTail)->data[readOff] = UINT8_MAX;

		readOff++;
		// Check if we can free the segment
		if (readOff == SegmentBufferSize) {
			uint16_t nextSeg = get_segment_next(headerTail);
			uint8_t owner = get_segment_owner(headerTail);
			if (nextSeg == UINT16_MAX) {
				set_header_head(queueHeader(owner), UINT16_MAX);
				ser_header_tail(queueHeader(owner), UINT16_MAX);
			}
			else {
				ser_header_tail(queueHeader(owner), nextSeg);
				set_segment_prev(nextSeg, UINT16_MAX);
			}
			std::memset(&data[headerTail], UINT8_MAX, SegmentMaxSize);
			move_tail_segment_to_pos(headerTail);
		}
		else {
			set_segment_read_off(headerTail, readOff);
		}
		return resultChar;
	}
	return UINT8_MAX;

}
QueueManager::QHandle* QueueManager::create_queue() {
	for (uint8_t i = 0; i < MaxQueues; i++)
	{
		if (!get_queue_in_use(queueHeader(i))) {
			QueueHeader* newQueue = queueHeader(i);
			set_queue_in_use(newQueue, true);
			return newQueue;
		}
	}
	on_out_of_memory();
	return nullptr;
}
QueueManager::QueueManager() {
	initialize();
	//UnitTest();
}
void QueueManager::destroy_queue(QHandle* q) {
	if (q == nullptr || !get_queue_in_use(q)) {
		on_illegal_operation();
		return;
	}

	uint16_t tail = get_header_tail(q);
	// If no tail, simply set Q to unused state
	if (tail == UINT16_MAX) {
		set_queue_in_use(q, false);
		return;
	}
	// Iterating thorugh the segments of the queue deleting them one by one
	// and shifting the buffer data

	while (tail != UINT16_MAX) {
		uint16_t nextSeg = get_segment_next(tail);
		if (nextSeg == UINT16_MAX) {
			set_header_head(q, UINT16_MAX);
			ser_header_tail(q, UINT16_MAX);
		}
		else {
			ser_header_tail(q, nextSeg);
			set_segment_prev(nextSeg, UINT16_MAX);
		}
		std::memset(&data[tail], UINT8_MAX, SegmentMaxSize);
		move_tail_segment_to_pos(tail);
		tail = get_header_tail(q);
	}

	set_header_head(q, UINT16_MAX);
	ser_header_tail(q, UINT16_MAX);
	set_queue_in_use(q, false);
}