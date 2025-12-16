# Possible improvements
While working on the test, I had multiple ideas on how to implement it, and in the end I 
settled on the solution I sent. Nevertheless, I can still see plenty of room for improvement. 
1. The segment header size could be reduced. Because the data is fragmented, each 
fragment needs to include some indication of which queue it belongs to, as well as 
other metadata for the segment: 
[read offset(1byte)∣data count(1byte)∣next segment(2bytes)∣previous 
segment(2bytes)∣segment owner(1byte)] — a total of 7 bytes. For example, it would 
be possible to remove both the read offset and the data count fields, but this would 
require iterating through the entire segment to find the correct position for reading or 
writing data. This would reduce computational performance but would allow more 
data to be stored, as two additional bytes would become available in each segment. 
2. Another possible improvement would be to introduce dynamic expansion of individual 
segments. Currently, each segment is fixed at 27 bytes - no more, no less. However, 
if the user attempts to enqueue another byte and the head of the queue is the same 
with the manager’s head, it would be possible to expand the segment without 
creating a new segment header. This would save 7 bytes of buffer space in that case. 
The problem with this approach is that, if a segment becomes large enough, a 1-byte 
data count would no longer be sufficient. Dynamically changing the segment header 
size would also remove the uniform segment size, which is currently relied upon to 
pack all segments tightly together. 
# Unit test
I did not use a full-featured unit testing framework such as Google Test, as the interface of the system is relatively small. The queue manager exposes only four user-facing functions, which makes it practical to check correctness using a couple custom test functions.

Introducing a large external testing framework would add additional complexity and boilerplate. Instead, I implemented targeted tests that directly exercise the expected behaviour, including correctness under randomised enqueue/dequeue operations and edge cases. This approach keeps the test code simple, transparent, and closely aligned with the actual usage of the API.
