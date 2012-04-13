/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher 
 * Copyright: GPL V2
 *
 * Based on the enc28j60.c file from the AVRlib library by Pascal Stang.
 * For AVRlib See http://www.procyonengineering.com/
 * Used with explicit permission of Pascal Stang.
 *
 * Title: Microchip ENC28J60 Ethernet Interface Driver
 * Chip type           : ATMEGA88 with ENC28J60
 *********************************************/

#include "enc28j60.h"


static uint8_t Enc28j60Bank;
static uint16_t NextPacketPtr;

// set CS to 0 = active
#define CSACTIVE ENC28J60_PORT &= ~(1<<ENC28J60_CS)
// set CS to 1 = passive
#define CSPASSIVE ENC28J60_PORT |= (1<<ENC28J60_CS)

#define waitspi() while(!(SPSR&(1<<SPIF)))

uint8_t enc28j60_read_op(uint8_t op, uint8_t address)
{
        CSACTIVE;
        // issue read command
        SPDR = op | (address & ADDR_MASK);
        waitspi();
        // read data
        SPDR = 0x00;
        waitspi();
        // do dummy read if needed (for mac and mii, see datasheet page 29)
        if(address & 0x80)
        {
                SPDR = 0x00;
                waitspi();
        }
        // release CS
        CSPASSIVE;
        return(SPDR);
}

void enc28j60_write_op(uint8_t op, uint8_t address, uint8_t data)
{
        CSACTIVE;
        // issue write command
        SPDR = op | (address & ADDR_MASK);
        waitspi();
        // write data
        SPDR = data;
        waitspi();
        CSPASSIVE;
}

void enc28j60ReadBuffer(uint16_t len, uint8_t* data)
{
        CSACTIVE;
        // issue read command
        SPDR = ENC28J60_READ_BUF_MEM;
        waitspi();
        while(len)
        {
                len--;
                // read data
                SPDR = 0x00;
                waitspi();
                *data = SPDR;
                data++;
        }
        *data='\0';
        CSPASSIVE;
}

void enc28j60WriteBuffer(uint16_t len, uint8_t* data)
{
        CSACTIVE;
        // issue write command
        SPDR = ENC28J60_WRITE_BUF_MEM;
        waitspi();
        while(len)
        {
                len--;
                // write data
                SPDR = *data;
                data++;
                waitspi();
        }
        CSPASSIVE;
}



void enc28j60_set_bank(uint8_t address)
{
        // set the bank (if needed)
        if((address & BANK_MASK) != Enc28j60Bank)
        {
                // set the bank
                enc28j60_write_op(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
                enc28j60_write_op(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
                Enc28j60Bank = (address & BANK_MASK);
        }
}

uint8_t enc28j60_read(uint8_t address)
{
        // set the bank
        enc28j60_set_bank(address);
        // do the read
        return enc28j60_read_op(ENC28J60_READ_CTRL_REG, address);
}

void enc28j60_write(uint8_t address, uint8_t data)
{
        // set the bank
        enc28j60_set_bank(address);
        // do the write
        enc28j60_write_op(ENC28J60_WRITE_CTRL_REG, address, data);
}

void enc28j60_write_phy(uint8_t address, uint16_t data)
{
        // set the PHY register address
        enc28j60_write(MIREGADR, address);
        // write the PHY data
        enc28j60_write(MIWRL, data);
        enc28j60_write(MIWRH, data>>8);
        // wait until the PHY write completes
        while(enc28j60_read(MISTAT) & MISTAT_BUSY){
                _delay_us(15);
        }
}


void enc28j60_reset(){
	enc28j60_write_op(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	_delay_ms(50);
}


void enc28j60_init(uint8_t* mac_addr){
	
	/*Initialize IO pins*/
	ENC28J60_DDR |= (1<<ENC28J60_SCK) | (1<<ENC28J60_MOSI) | (1<<ENC28J60_CS);	//Outputs
	ENC28J60_DDR &= ~(1<<ENC28J60_MISO);										//Input
	CSPASSIVE;																	//Chip select off
	
	/*Initialize SPI*/
	SPCR = (1<<SPE) | (1<<MSTR);				//Enable SPI with clock rate f/4 and as master
	
	/*Initialize ENC28J60*/
	enc28j60_write_op(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);
	_delay_ms(50);
	//while(!(enc28j60_read(ESTAT) & ESTAT_CLKRDY));
	
	/*Clock output*/
	enc28j60_write(ECOCON, 0x02);			//Main clk/2 = 12.5 MHz
	
	/*Receive buffer*/
	enc28j60_write(ERXSTL, (RXSTART_INIT & 0xFF));			//Set start address
	enc28j60_write(ERXSTH, (RXSTART_INIT>>8));
	
	enc28j60_write(ERXNDL, (RXSTOP_INIT & 0xFF));			//Set end address
	enc28j60_write(ERXNDH, (RXSTOP_INIT>>8));
	
	enc28j60_write(ERXRDPTL, (RXSTART_INIT & 0xFF));		//Set receive address pointer
	enc28j60_write(ERXRDPTH, (RXSTART_INIT>>8));
	
	/*Transmission buffer*/
	enc28j60_write(ETXSTL, (TXSTART_INIT & 0xFF));			//Set start address
	enc28j60_write(ETXSTH, (TXSTART_INIT>>8));
	
	/*Receive filters*/
	enc28j60_write(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);
	
	/*MAC settings*/
	enc28j60_write(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);		//Set MARXEN to enable the MAC to receive frames
	enc28j60_write(MACON2, 0x00);						//Clear MARST (MARST is only set bit after reset)
	enc28j60_write(MACON3, MACON3_PADCFG0 | MACON3_TXCRCEN | MACON3_FRMLNEN);	//Pad frames to 64 bytes, append with CRC, chech frame length
	enc28j60_write(MAMXFLL, (MAX_FRAMELEN & 0xFF));				//Set maximum frame length
	enc28j60_write(MAMXFLH, (MAX_FRAMELEN>>8));
	enc28j60_write(MABBIPG, 0x12);						//Back-to-Back Inter-Packet Gap
	enc28j60_write(MAIPGL, 0x12);						//Non-Back-to-Back Inter-Packet Gap
	enc28j60_write(MAIPGH, 0x0C);
	enc28j60_write(MAADR0, mac_addr[5]);				//Set MAC address
	enc28j60_write(MAADR1, mac_addr[4]);
	enc28j60_write(MAADR2, mac_addr[3]);
	enc28j60_write(MAADR3, mac_addr[2]);
	enc28j60_write(MAADR4, mac_addr[1]);
	enc28j60_write(MAADR5, mac_addr[0]);
	
	/*PHY settings*/
	enc28j60_write_phy(PHCON2, PHCON2_HDLDIS);			//Disable automatic loopback
	
	/*Interrupts and enable RX*/
	ENC28J60_INT_DDR &= ~(1<<ENC28J60_INT_PIN);		//Interrupt pin as input
	EICRA |= (1<<ISC11);							//Interrupt INT1 on falling edge
	EIMSK |= (1<<INT1);								//Enable external interrupt INT1
	
	enc28j60_set_bank(EIE);
	enc28j60_write_op(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE | EIE_PKTIE);	//Enable interrupts when packets arrive
	
	enc28j60_write_op(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);				//Enable reception
}


void enc28j60_send(uint16_t len, uint8_t* packet)
{
	// Set the write pointer to start of transmit buffer area
	enc28j60_write(EWRPTL, TXSTART_INIT&0xFF);
	enc28j60_write(EWRPTH, TXSTART_INIT>>8);
	// Set the TXND pointer to correspond to the packet size given
	enc28j60_write(ETXNDL, (TXSTART_INIT+len)&0xFF);
	enc28j60_write(ETXNDH, (TXSTART_INIT+len)>>8);
	// write per-packet control byte (0x00 means use macon3 settings)
	enc28j60_write_op(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(len, packet);
	// send the contents of the transmit buffer onto the network
	enc28j60_write_op(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
        // Reset the transmit logic problem. See Rev. B4 Silicon Errata point 12.
	if( (enc28j60_read(EIR) & EIR_TXERIF) ){
                enc28j60_write_op(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
        }
}

// Gets a packet from the network receive buffer, if one is available.
// The packet will by headed by an ethernet header.
//      maxlen  The maximum acceptable length of a retrieved packet.
//      packet  Pointer where packet data should be stored.
// Returns: Packet length in bytes if a packet was retrieved, zero otherwise.
uint16_t enc28j60_recv(uint16_t maxlen, uint8_t* packet)
{
	uint16_t rxstat;
	uint16_t len;
	// check if a packet has been received and buffered
	//if( !(enc28j60Read(EIR) & EIR_PKTIF) ){
        // The above does not work. See Rev. B4 Silicon Errata point 6.
	if( enc28j60_read(EPKTCNT) ==0 ){
		return(0);
	}

	// Set the read pointer to the start of the received packet
	enc28j60_write(ERDPTL, (NextPacketPtr));
	enc28j60_write(ERDPTH, (NextPacketPtr)>>8);
	// read the next packet pointer
	NextPacketPtr  = enc28j60_read_op(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= enc28j60_read_op(ENC28J60_READ_BUF_MEM, 0)<<8;
	// read the packet length (see datasheet page 43)
	len  = enc28j60_read_op(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60_read_op(ENC28J60_READ_BUF_MEM, 0)<<8;
        len-=4; //remove the CRC count
	// read the receive status (see datasheet page 43)
	rxstat  = enc28j60_read_op(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60_read_op(ENC28J60_READ_BUF_MEM, 0)<<8;
	// limit retrieve length
        if (len>maxlen-1){
                len=maxlen-1;
        }
        // check CRC and symbol errors (see datasheet page 44, table 7-3):
        // The ERXFCON.CRCEN is set by default. Normally we should not
        // need to check this.
        if ((rxstat & 0x80)==0){
                // invalid
                len=0;
        }else{
                // copy the packet from the receive buffer
                enc28j60ReadBuffer(len, packet);
        }
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	enc28j60_write(ERXRDPTL, (NextPacketPtr));
	enc28j60_write(ERXRDPTH, (NextPacketPtr)>>8);
	// decrement the packet counter indicate we are done with this packet
	enc28j60_write_op(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	return(len);
}



// read the revision of the chip:
uint8_t enc28j60_get_rev(void)
{
	return(enc28j60_read(EREVID));
}
