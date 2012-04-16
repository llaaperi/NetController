#include <embUnit/embUnit.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mockEnc28j60.h"
#include "../src/net/network.h"
#include "../src/net/eth.h"
#include "../src/net/arp.h"



uint8_t _myMacAddr[6] = {0x00, 0xFA, 0x20, 0xFA, 0x47, 0x31};
uint8_t _myIpAddr[4] = {10, 0, 0, 5};
uint8_t _srcMacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
uint8_t _dstMacAddr[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x02};
uint8_t _srcIpAddr[4] = {10, 0, 0, 1};
uint8_t _dstIpAddr[4] = {10, 0, 0, 2};


/*************************************************
 *	HELPER FUNCTIONS
 */

/*
 *
 */
static void initEthHeader(uint16_t type){
	
	memcpy(_recvBuffer, _myMacAddr, 6);	//Destination MAC address (my MAC)
	memcpy(_recvBuffer + 6, _srcMacAddr, 6); //Source MAC
	_recvBuffer[ETH_H_TYPE_H] = (type >> 8);
	_recvBuffer[ETH_H_TYPE_L] = (type & 0xFF);
}


/*
 *
 */
static void checkEthHeader(uint16_t type){
	
	TEST_ASSERT(!memcmp(_sendBuffer, _srcMacAddr, 6));
	TEST_ASSERT(!memcmp(_sendBuffer + 6, _myMacAddr, 6));
	TEST_ASSERT(_sendBuffer[ETH_H_TYPE_H] == (type >> 8));
	TEST_ASSERT(_sendBuffer[ETH_H_TYPE_L] == (type & 0xFF));
}


/*
 *
 */
static void initArpRequest(){
	
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_HTYPE_H] = ARP_HTYPE_H;	//Hardware type Ethernet
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_HTYPE_L] = ARP_HTYPE_L;
	
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_PTYPE_H] = ARP_PTYPE_H;	//Protocol type IP
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_PTYPE_L] = ARP_PTYPE_L;
	
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_HLEN] = ARP_HLEN;	//Hardware address length 6
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_PLEN] = ARP_PLEN;	//Protocol address length 4
	
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_OPER_H] = 0x00;	//Operation reguest
	_recvBuffer[ETH_HEADER_SIZE + ARP_P_OPER_L] = 0x01;
	
	memcpy(_recvBuffer + ETH_HEADER_SIZE + ARP_P_SRC_MAC, _srcMacAddr, 6);
	memcpy(_recvBuffer + ETH_HEADER_SIZE + ARP_P_SRC_IP, _srcIpAddr, 4);
	
	memset(_recvBuffer + ETH_HEADER_SIZE + ARP_P_DEST_MAC, 0, 6);	//Zero dest MAC address in request
	memcpy(_recvBuffer + ETH_HEADER_SIZE + ARP_P_DEST_IP, _myIpAddr, 4);
}


/*
 *
 */
static void checkArpReply(){
	
	int idx = ETH_HEADER_SIZE;
	
	TEST_ASSERT(_sendBuffer[idx + ARP_P_HTYPE_H] == ARP_HTYPE_H);	//Check hardware type
	TEST_ASSERT(_sendBuffer[idx + ARP_P_HTYPE_L] == ARP_HTYPE_L);
	
	TEST_ASSERT(_sendBuffer[idx + ARP_P_PTYPE_H] == ARP_PTYPE_H);	//Check protocol type
	TEST_ASSERT(_sendBuffer[idx + ARP_P_PTYPE_L] == ARP_PTYPE_L);
	
	TEST_ASSERT(_sendBuffer[idx + ARP_P_HLEN] == ARP_HLEN);	//Check address lengths
	TEST_ASSERT(_sendBuffer[idx + ARP_P_PLEN] == ARP_PLEN);
	
	TEST_ASSERT(_sendBuffer[idx + ARP_P_OPER_H] == 0x00);	//Check operation
	TEST_ASSERT(_sendBuffer[idx + ARP_P_OPER_L] == 0x02);	//reply
	
	TEST_ASSERT(!memcmp(_sendBuffer + idx + ARP_P_SRC_MAC, _myMacAddr, 6));
	TEST_ASSERT(!memcmp(_sendBuffer + idx + ARP_P_SRC_IP, _myIpAddr, 4));
	TEST_ASSERT(!memcmp(_sendBuffer + idx + ARP_P_DEST_MAC, _srcMacAddr, 6));
	TEST_ASSERT(!memcmp(_sendBuffer + idx + ARP_P_DEST_IP, _srcIpAddr, 4));
	
}


/**********************************************************
 *	TESTS
 */


static void setUp(void){
	
	//Inialize network addresses
	network_init();
}

static void tearDown(void){
}


/*
 * Test tat init loads network addresses
 */
static void testInit(void){
	
	const uint8_t* macAddr = network_get_mac_addr();
	const uint8_t* ipAddr = network_get_ip_addr();
	
	TEST_ASSERT(!memcmp(macAddr, _network_mac_addr, 6));
	TEST_ASSERT(!memcmp(ipAddr, _network_ip_addr, 4));
}


/*
 *
 */
static void testArpRequest(void){
	
	initEthHeader(ETH_TYPE_ARP);
	initArpRequest();
	
	_recvLen = ETH_HEADER_SIZE + ARP_PACKET_SIZE;
	
	print_recv();
	network_recv();
	print_send();
	
	checkEthHeader(ETH_TYPE_ARP);
	checkArpReply();
	
}


TestRef NetworkTest_tests(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures) {
		new_TestFixture("testInit",testInit),
		new_TestFixture("testArpRequest",testArpRequest),
	};
	
	EMB_UNIT_TESTCALLER(NetworkTest,"NetworkTest",setUp,tearDown,fixtures);
	return (TestRef)&NetworkTest;                        
}

