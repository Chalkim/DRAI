#ifndef INT_HEADER_H
#define INT_HEADER_H

#include "ns3/buffer.h"
#include <stdint.h>
#include <cstdio>

namespace ns3 {

class IntHop{
public:
	static const uint32_t timeWidth = 24;
	static const uint32_t bytesWidth = 20;
	static const uint32_t qlenWidth = 17;
	static const uint64_t lineRateValues[8];

	#pragma pack(push, 8)  // default 8-byte alignment on x86_64
	#pragma pack(4)
	union{
		struct {
			uint64_t lineRate: 64-timeWidth-bytesWidth-qlenWidth,  // B
			         time: timeWidth,                              // TS
			         bytes: bytesWidth,                            // rxBytes
			         qlen: qlenWidth;                              // qLen
			uint32_t flowNum;                                      // flowNum
		};
		uint32_t buf[3];
	};
	#pragma pack(pop)

	static const uint32_t byteUnit = 128;
	static const uint32_t qlenUnit = 80;
	static uint32_t multi;

	uint64_t GetLineRate(){
		return lineRateValues[lineRate];
	}
	uint64_t GetBytes(){
		return (uint64_t)bytes * byteUnit * multi;
	}
	uint32_t GetQlen(){
		return (uint32_t)qlen * qlenUnit * multi;
	}
	uint64_t GetTime(){
		return time;
	}
	uint32_t GetFlowNum(){
		return flowNum;
	}
	void Set(uint64_t _time, uint64_t _bytes, uint32_t _qlen, uint64_t _rate, uint32_t _flowNum){
		time = _time;
		bytes = _bytes / (byteUnit * multi);
		qlen = _qlen / (qlenUnit * multi);
		flowNum = _flowNum;
		switch (_rate){
			case 25000000000lu:
				lineRate=0;break;
			case 50000000000lu:
				lineRate=1;break;
			case 100000000000lu:
				lineRate=2;break;
			case 200000000000lu:
				lineRate=3;break;
			case 400000000000lu:
				lineRate=4;break;
			default:
				printf("Error: IntHeader unknown rate: %lu\n", _rate);
				break;
		}
	}
	uint64_t GetBytesDelta(IntHop &b){
		if (bytes >= b.bytes)
			return (bytes - b.bytes) * byteUnit * multi;
		else
			return (bytes + (1<<bytesWidth) - b.bytes) * byteUnit * multi;
	}
	uint64_t GetTimeDelta(IntHop &b){
		if (time >= b.time)
			return time - b.time;
		else
			return time + (1<<timeWidth) - b.time;
	}
};

class IntHeader{
public:
	static const uint32_t maxHop = 5;
	enum Mode{
		NORMAL = 0,
		TS = 1,
		PINT = 2,
		NONE
	};
	static Mode mode;
	static int pint_bytes;

	// Note: the structure of IntHeader must have no internal padding, because we will directly transform the part of packet buffer to IntHeader*
	union{
		struct {
			IntHop hop[maxHop];
			uint16_t nhop;
		};
		uint64_t ts;
		union {
			uint16_t power;
			struct{
				uint8_t power_lo8, power_hi8;
			};
		}pint;
	};

	IntHeader();
	static uint32_t GetStaticSize();
	void PushHop(uint64_t time, uint64_t bytes, uint32_t qlen, uint64_t rate, uint32_t flowNum);
	void Serialize (Buffer::Iterator start) const;
	uint32_t Deserialize (Buffer::Iterator start);
	uint64_t GetTs(void);
	uint16_t GetPower(void);
	void SetPower(uint16_t);
};

}

#endif /* INT_HEADER_H */
