//*****************************************************************************************************
//
//      This header serves as the class to hold an array of data bytes, allowing to create queues 
//      inside the buffer and use them using 4 main provided functions
//
//		Data is split in three main parts [queue headers|free area allocator|free buffer]
//
//		In free buffer data is split into segments - each having following metadata 
//		[position of the next byte to read|last stored byte position|next segment|previous segment|segment owner]
//		The rest of the segment is free to write bytes there
// 
//		Buffer is a circular buffer with a fixed segment size. Once some segments is freed, data from the tail is moved to the
//		position where deleted segment was located. This ensures that all data is tightly packed and no dangling free segments are present
// 
//*****************************************************************************************************
#pragma once
class QueueManager {
private:

#pragma pack(push,1)
	struct QueueHeader {
		uint16_t tailPos;
		uint16_t headPos;
		uint8_t inUse;
	};
	struct FreeAreaHeader {
		uint16_t tailPos;
		uint16_t headPos;
	};
#pragma pack(pop)
	static const int DataSize = 2048;
	static const int MaxQueues = 64;
	static const int FreeAreaHeaderOffset = (sizeof(QueueHeader) * MaxQueues);
	// offset where the actual buffer to store information starts
	static const int BufferOffset = (FreeAreaHeaderOffset + sizeof(FreeAreaHeader));
	static const int SegmentHeaderSize = 7;
	static const int SegmentMaxSize = (DataSize - BufferOffset) / MaxQueues;
	static const int SegmentBufferSize = SegmentMaxSize - SegmentHeaderSize;

#pragma pack(push,1)
	struct Segment {
		uint8_t readOff;
		uint8_t count;
		uint16_t next;
		uint16_t prev;
		uint8_t owner;
		uint8_t data[];
	};
#pragma pack(pop)

public:
	using QHandle = QueueHeader;
private:
	unsigned char data[DataSize];
	void on_out_of_memory();
	void on_illegal_operation();

	//*****************************************************************************************************
	//    Generalized read/write functions
	//*****************************************************************************************************
	uint8_t read_UInt8(unsigned char* readPos);
	void write_UInt8(unsigned char* writePos, const uint8_t value);
	void write_UInt16(unsigned char* writePlace, const uint16_t value);
	void write_UInt16(const uint16_t writePlace, const uint16_t value);
	uint16_t ReadUInt16(const unsigned char* uchar);

	//*****************************************************************************************************
	//    Specific read/write functions for the metadata in the array
	//*****************************************************************************************************
	//Manager functions
	void set_free_area_tail(uint16_t position);
	uint16_t get_free_area_tail();
	void set_free_area_head(uint16_t position);
	uint16_t get_free_area_head();
	//Header functions
	bool get_queue_in_use(QHandle* q);
	void set_queue_in_use(QHandle* q, bool isInUse);
	uint16_t get_header_tail(QHandle* q);
	void ser_header_tail(QHandle* q, uint16_t headPos);
	uint16_t get_header_head(QHandle* q);
	void set_header_head(QHandle* q, uint16_t headPos);
	//Segment functions
	void set_segment_data_count(uint16_t segmentPos, uint8_t count);
	uint8_t get_segment_data_count(uint16_t segmentPos);
	void set_segment_next(uint16_t currentSegment, uint16_t nextSegment);
	uint16_t get_segment_next(uint16_t currentSegment);
	void set_segment_prev(uint16_t currentSegment, uint16_t prevSegment);
	uint16_t get_segment_prev(uint16_t currentSegment);
	void set_segment_owner(uint16_t segmentPos, uint8_t owner);
	uint8_t get_segment_owner(uint16_t segmentPos);
	void set_segment_read_off(uint16_t segmentPos, uint8_t off);
	uint8_t get_segment_read_off(uint16_t segmentPos);
	//*****************************************************************************************************

	void print_segment(uint16_t pos);
	void get_result_size_of_whole_queue(int x);
	void print_read_and_byte_off(uint16_t pos);
	void do_full_segment_check(uint16_t segment);
	QueueHeader* queueHeader(int index);
	FreeAreaHeader* freeAreaHeader();
	Segment* segmentByPos(uint16_t pos);

	void initialize();
	bool ensure_free_memory(uint16_t* managerHead);
	void move_tail_segment_to_pos(uint16_t targetPos);
public:
	QueueManager();
	void UnitTest();
	void enqueue_byte(QHandle* q, unsigned char b);
	unsigned char dequeue_byte(QHandle* q);
	QHandle* create_queue();
	void destroy_queue(QHandle* q);
};