/*
 * http.c
 *
 * Created: 10.7.2011 23:58:43
 *  Author: Lauri
 */ 

#ifdef TEST
	#define sprintf_P sprintf
	#define strncmp_P strncmp
	#define sscanf_P sscanf
	#define PSTR(X) X
	#define PROGMEM
	#include "../../tests/mockLcd.h"
	#include "../../tests/mockDs1820.h"
	#include "../../tests/mockRelay.h"
	#include "../time.h"
#else
	#include <avr/pgmspace.h>
	#include "../lcd.h"
	#include "../ds1820.h"
	#include "../relay.h"
	#include "../time.h"
#endif


#include <stdio.h>
#include <string.h>
#include "network.h"
#include "eth.h"
#include "ip.h"
#include "tcp.h"
#include "http.h"

/*
uint16_t http_write_buf(uint8_t* buf, const prog_char* prog_str){
	
	char c;
	uint16_t idx = 0;
	while((c = pgm_read_byte(prog_str++))){
		buf[idx++] = c;
	}
	return idx;
}
*/

struct tcp_socket* _socket;	//Current client socket


/*
 *
 */
uint16_t http_page_main(char* buf){
	
	uint16_t len = 0;
	
	len += sprintf_P((buf + len), PSTR("<p>Hello, who ever you are!<br/>"));
	len += sprintf_P((buf + len), PSTR("Everything is under construction and therefore mostly incomplete.<br/>"));
	len += sprintf_P((buf + len), PSTR("Feel free to try out whatever you like. No harm can be done.</p>"));
	
	return len;
}


/*
 *
 */
uint16_t http_page_time(char* buf){
	
	uint16_t len = 0;
	
	len += sprintf_P((buf + len), PSTR("<h2>Time</h2>"));
	
	return len;
}


/*
 *
 */
uint16_t http_page_sensor(char* buf){
	
	uint16_t len = 0;
	uint8_t sensor_cnt = 3;
	
	int temp = 0;
	int temp_min = 0;
	int temp_max = 0;
	
	len += sprintf_P((buf + len), PSTR("<h2>Sensors</h2>"));
	len += sprintf_P((buf + len), PSTR("<table border=\"2\" cellpadding=\"5\" bgcolor=\"#EEEEEE\"><tr><th>Sensor</th><th>Current</th><th>Min</th><th>Max</th></tr>"));
	for(int i = 0; i < sensor_cnt; i++){
		
		temp = ds1820_get_cur(i);
		temp_min = ds1820_get_min(i);
		temp_max = ds1820_get_max(i);
		
		len += sprintf_P((buf + len), PSTR("<tr><form method=\"get\" action=\"FormSensor\"><td>"));
		
		len += sprintf_P((buf + len), PSTR("<input type=\"text\" name=\"tag\" value=\""));
		len += ds1820_print_tag((buf + len), i);			//Sensor tag
		len += sprintf_P((buf + len), PSTR("\" maxlength=\"%d\">"), DS1820_TAG_SIZE);
		len += sprintf_P((buf + len), PSTR("<input type=\"submit\" name=\"sensor%d\" value=\"Rename\">"), i);
		
		len += sprintf_P((buf + len), PSTR("</td><td>"));
		len += ds1820_print_temp((buf + len), temp);		//Current value
		len += sprintf_P((buf + len), PSTR("</td><td><font color=\"#0000FF\">"));
		len += ds1820_print_temp((buf + len), temp_min);	//Min value
		len += sprintf_P((buf + len), PSTR("</font></td><td><font color=\"#FF0000\">"));
		len += ds1820_print_temp((buf + len), temp_max);	//Max value
		len += sprintf_P((buf + len), PSTR("</font></td><td><input type=\"submit\" name=\"sensor%d\" value=\"Reset\"></td></form></tr>"), i);	//Reset button
		
		if(i < (sensor_cnt - 1)){
			tcp_send(_socket, TCP_FLAG_ACK, len);	//Send sensor rows separately, excluding the last one which is appended with the final part of the page
			len = 0;			//Reset length counter
		}
		
	}
	len += sprintf_P((buf + len), PSTR("</table>"));
	
	//tcp_send(len, 2);	//Send second part of the page
	//len = 0;			//Reset length counter
	
	return len;
}


/*
 *
 */
uint16_t http_page_relay(char* buf, int card){
	
	uint16_t len = 0;
	
	unsigned char state = relay_get_state(card);
	unsigned char ctrl = 0;
	
	len += sprintf_P((buf + len), PSTR("<h2>Relay Card%d</h2>"), (card + 1));
	len += sprintf_P((buf + len), PSTR("<table border=\"2\" cellpadding=\"5\" bgcolor=\"#EEEEEE\"><tr><th>Relay</th><th>State</th><th>Control</th></tr>"));
	for(int relay = 0; relay < 8; relay++){
		
		ctrl = relay_get_ctrl(card, relay);
		
		//len += sprintf_P((buf + len), PSTR("<tr><td>"));
		//len += relay_print_tag((buf + len), card, relay);			//Relay tag
		
		
		len += sprintf_P((buf + len), PSTR("<tr><form method=\"get\" action=\"FormRelayTag\"><td style=\"min-width:227px\">"));
		
		len += sprintf_P((buf + len), PSTR("<input type=\"text\" name=\"tag\" value=\""));
		len += relay_print_tag((buf + len), card, relay);			//Relay tag
		len += sprintf_P((buf + len), PSTR("\" maxlength=\"%d\">"), RELAY_TAG_SIZE);
		len += sprintf_P((buf + len), PSTR("<input type=\"submit\" name=\"Relay%d%d\" value=\"Rename\"></td></form>"), card, relay);
		
		
		len += sprintf_P((buf + len), PSTR("<td style=\"background-color:%s"), (state & (1<<relay))?"#00FF00\">ON":"#FF0000\">OFF"); //Relay state

		len += sprintf_P((buf + len), PSTR("</td><form method=\"get\" action=\"FormRelayCtrl\"><td style=\"text-align:center;min-width:250px\">"));
		len += sprintf_P((buf + len), PSTR("<input type=\"radio\" name=\"Relay%d%d\" value=\"Manual\" onClick=\"submit()\"%s>Manual"), card, relay, (ctrl & RELAY_CTRL_MANUAL)?" checked":" ");
		len += sprintf_P((buf + len), PSTR("<input type=\"radio\" name=\"Relay%d%d\" value=\"Sensor\" onClick=\"submit()\"%s>Sensor"), card, relay, (ctrl & RELAY_CTRL_SENSOR)?" checked":" ");
		len += sprintf_P((buf + len), PSTR("<input type=\"radio\" name=\"Relay%d%d\" value=\"Timer\" onClick=\"submit()\"%s>Timer"), card, relay, (ctrl & RELAY_CTRL_TIMER)?" checked":" ");
		len += sprintf_P((buf + len), PSTR("</td></form>"));
		
		if(ctrl & RELAY_CTRL_MANUAL){	//Add button for switching relay state
			
			if(relay == 0){
				tcp_send(_socket, TCP_FLAG_ACK, len);	//Send separately
				len = 0;			//Reset length counter
			}
			
			len += sprintf_P((buf + len), PSTR("<form method=\"get\" action=\"FormRelaySwitch\"><td>"));
			len += sprintf_P((buf + len), PSTR("<input type=\"submit\" name=\"Relay%d%d\" value=\"Switch\">"), card, relay);
		}else
		if(ctrl & RELAY_CTRL_SENSOR){	//add form for choosing sensor and values
			
			tcp_send(_socket, TCP_FLAG_ACK, len);	//Send separately
			len = 0;			//Reset length counter
			
			len += sprintf_P((buf + len), PSTR("<form method=\"get\" action=\"FormRelaySensor\"><td style=\"min-width:360px\">"));
			
			//Turn on/off
			len += sprintf_P((buf + len), PSTR("Turn "));
			len += sprintf_P((buf + len), PSTR("<select name=\"state\"><option value=\"1\"%s>ON</option><option value=\"0\"%s>OFF</option></select>"), (ctrl & RELAY_CTRL_STATE)?" selected":" ", (ctrl&RELAY_CTRL_STATE)?" ":" selected");
			len += sprintf_P((buf + len), PSTR(" if "));
			//List available sensors
			len += sprintf_P((buf + len), PSTR("<select name=\"sensor\">"));
			for(int i = 0; i < 3; i++){
				len += sprintf_P((buf + len), PSTR("<option value=\"%d\"%s>"), i, (i == (ctrl>>4))?" selected":" ");
				len += ds1820_print_tag((buf + len), i);
				len += sprintf_P((buf + len), PSTR("</option>"));
			}
			len += sprintf_P((buf + len), PSTR("</select>"));
			len += sprintf_P((buf + len), PSTR(" is greater than "));
			//Input for switching value
			len += sprintf_P((buf + len), PSTR("<input style=\"width:48px\" type=\"text\" name=\"value\" maxlength=\"5\" value=\""));
			len += ds1820_print_temp((buf + len), relay_get_sensor(card, relay));
			len += sprintf_P((buf + len), PSTR("\"/><input type=\"submit\" name=\"relay%d%d\" value=\"Set\">"), card, relay);
		}else
		if(ctrl & RELAY_CTRL_TIMER){
			
			tcp_send(_socket, TCP_FLAG_ACK, len);	//Send separately
			len = 0;			//Reset length counter
			
			len += sprintf_P((buf + len), PSTR("<form method=\"get\" action=\"FormRelayTimer\"><td>"));
			
			len += sprintf_P((buf + len), PSTR("Turn "));
			len += sprintf_P((buf + len), PSTR("<select name=\"state\"><option value=\"1\"%s>ON</option><option value=\"0\"%s>OFF</option></select>"), (ctrl & RELAY_CTRL_STATE)?" selected":" ", (ctrl&RELAY_CTRL_STATE)?" ":" selected");
			len += sprintf_P((buf + len), PSTR(" between "));
			
			uint8_t start_h = 0, start_m = 0, end_h = 0, end_m = 0; 
			
			relay_get_timer(card, relay, RELAY_TIMER_START, &start_h, &start_m);
			relay_get_timer(card, relay, RELAY_TIMER_END, &end_h, &end_m);
			
			len += sprintf_P((buf + len), PSTR("<input style=\"width:25px\" type=\"text\" name=\"starth\" maxlength=\"2\" value=\"%02d\"/> : "), start_h);
			len += sprintf_P((buf + len), PSTR("<input style=\"width:25px\" type=\"text\" name=\"startm\" maxlength=\"2\" value=\"%02d\"/>"), start_m);

			len += sprintf_P((buf + len), PSTR(" - "));
			len += sprintf_P((buf + len), PSTR("<input style=\"width:25px\" type=\"text\" name=\"endh\" maxlength=\"2\" value=\"%02d\"/> : "), end_h);
			len += sprintf_P((buf + len), PSTR("<input style=\"width:25px\" type=\"text\" name=\"endm\" maxlength=\"2\" value=\"%02d\"/>"), end_m);
			
			len += sprintf_P((buf + len), PSTR("<input type=\"submit\" name=\"relay%d%d\" value=\"Set\">"), card, relay);
			
		}
		
		len += sprintf_P((buf + len), PSTR("</td></form></tr>"));
		
		if((relay < 7)){
			tcp_send(_socket, TCP_FLAG_ACK, len);	//Send every row separately in the table
			len = 0;			//Reset length counter			
		}

	}
	
	len += sprintf_P((buf + len), PSTR("</table>"));
	tcp_send(_socket, TCP_FLAG_ACK, len);	//Send every row separately in the table
	len = 0;			//Reset length counter		
		
	return len;
}


/*
 * Page 1=main, 2=time, 3=sensor, 4=card1, 5=card2, 6=card3
 */
void http_send_page(char* buf, int page){
	
	uint16_t len = 0;
	
	len += sprintf_P((buf + len), PSTR("<html><head></head>"));
	len += sprintf_P((buf + len), PSTR("<body style=\"margin:0\">"));
	len += sprintf_P((buf + len), PSTR("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" style=\"height:100%%;width:100%%\">"));
	len += sprintf_P((buf + len), PSTR("<tr style=\"height:100;background-color:#206BA4\">"));
	len += sprintf_P((buf + len), PSTR("<td colspan=\"2\" style=\"text-align:center\">"));
	len += sprintf_P((buf + len), PSTR("<h1>NetController</h1> "));
	len += sprintf_P((buf + len), PSTR("<p>Version 0.1</p>"));
	len += sprintf_P((buf + len), PSTR("</td></tr><tr>"));
	len += sprintf_P((buf + len), PSTR("<td style=\"padding:10;width:200;min-width:200;background-color:#BBD9EE;vertical-align:top\">"));
	len += sprintf_P((buf + len), PSTR("<p>Server time: "));
	len += time_print((buf + len));		//Server time
	len += sprintf_P((buf + len), PSTR("<br/>Server uptime: "));
	len += clock_print((buf + len));	//Server uptime
	len += sprintf_P((buf + len), PSTR("</p><p>"));
	len += sprintf_P((buf + len), PSTR("<a href=\"NetController.html\">Main</a><br/>"));
	len += sprintf_P((buf + len), PSTR("<a href=\"Time.html\">Time</a><br/>"));
	len += sprintf_P((buf + len), PSTR("<a href=\"Sensor.html\">Sensors</a><br/>"));
	len += sprintf_P((buf + len), PSTR("<a href=\"Relay1.html\">Relay Card1</a><br/>"));
	len += sprintf_P((buf + len), PSTR("<a href=\"Relay2.html\">Relay Card2</a><br/>"));
	len += sprintf_P((buf + len), PSTR("<a href=\"Relay3.html\">Relay Card3</a><br/>"));
	len += sprintf_P((buf + len), PSTR("</p></td><td style=\"padding:25;background-color:#EBF4FA;vertical-align:top\">"));
	
	tcp_send(_socket, TCP_FLAG_ACK, len);	//Send first part of the page
	len = 0;			//Reset length counter
	
	//Page content starts here
	
	switch(page){
		case 1: len = http_page_main(buf); break;
		case 2: len = http_page_time(buf); break;
		case 3: len = http_page_sensor(buf); break;
		case 4: len = http_page_relay(buf, 0); break;
		case 5: len = http_page_relay(buf, 1); break;
		case 6: len = http_page_relay(buf, 2); break;
	};
	
	//Page content ends here
	
	len += sprintf_P((buf + len), PSTR("</td></tr>"));
	len += sprintf_P((buf + len), PSTR("<tr style=\"height:25;background-color:#206BA4\">"));
	len += sprintf_P((buf + len), PSTR("<td colspan=\"2\" style=\"text-align:center\">"));
	len += sprintf_P((buf + len), PSTR("Copyright &copy; 2011 Lauri Laaperi"));
	len += sprintf_P((buf + len), PSTR("</td></tr>"));
	len += sprintf_P((buf + len), PSTR("</table></body></html>"));
	
	tcp_send(_socket, TCP_FLAG_FIN | TCP_FLAG_ACK, len);	//Send final part of the page (FIN)
}


/*
 *
 */
void http_send_login(char* buf, uint8_t type){
	
	uint16_t len = 0;
	
	len += sprintf_P((buf + len), PSTR("<html><head></head>"));
	len += sprintf_P((buf + len), PSTR("<body style=\"margin:0;background-color:#EBF4FA;\">"));
	len += sprintf_P((buf + len), PSTR("<div style=\"width:100%%;padding-top:10%%;text-align:center;\">"));
	len += sprintf_P((buf + len), PSTR("<h1>NetController</h1>admin:salasana"));
	len += sprintf_P((buf + len), PSTR("<table style=\"margin:auto;\"><tr><td>"));
	len += sprintf_P((buf + len), PSTR("<table><form method=\"post\" action=\"login\"><tr>"));
	len += sprintf_P((buf + len), PSTR("<td align=\"right\"><b>User:</b><input type=\"text\" name=\"usr\" /></td>"));
	len += sprintf_P((buf + len), PSTR("</tr><tr>"));
	len += sprintf_P((buf + len), PSTR("<td align=\"right\"><b>Password:</b><input type=\"password\" name=\"pwd\" /></td>"));
	len += sprintf_P((buf + len), PSTR("<td><input type=\"submit\" value=\"Login\" /></td>"));
	len += sprintf_P((buf + len), PSTR("</tr></form></table>"));
	
	if(type == 1){
		len += sprintf_P((buf + len), PSTR("Invalid username or password."));	
	}
	
	len += sprintf_P((buf + len), PSTR("</body></html>"));	
	
	sprintf_P(lcd_buf_l1, PSTR("HTTP:%u.%u.%u.%u"), _socket->dest_ip[0], _socket->dest_ip[1], _socket->dest_ip[2], _socket->dest_ip[3]);
	lcd_write_buffer(lcd_buf_l1, NULL);
	
	tcp_send(_socket, TCP_FLAG_FIN | TCP_FLAG_ACK, len);	//Send final part of the page (FIN)
}


/*
 *
 */
uint16_t http_header_len(const uint8_t* packet, uint16_t pkt_len){
	
	uint16_t hlen = 0;
	uint8_t end_flag = 0;
	
	while(hlen < pkt_len){
		
		if((packet[hlen] == '\r') && (packet[hlen + 1] == '\n')){
			end_flag++;
			hlen += 2;
			if(end_flag == 2){
				break;
			}
		}else{
			end_flag = 0;
			hlen += 1;
		}
	}
	return hlen;
}


/*
 *
 */
int http_form_handler(const char* packet){
	
	char* ptr = 0;
	uint8_t tag_len = 0;
	
	if(!strncmp_P(packet, PSTR("/FormSensor?tag="), 16)){
		
		//Model string: /FormSensor?tag=System&Sensor0=Reset
		
		ptr = strchr((packet + 16), '&');
		tag_len = ptr - (packet + 16);	//Calculate tag length from the start and end pointers
		
		int sensor = -1;
		if(sscanf_P(ptr + 1, PSTR("sensor%d"), &sensor) != 1){
			return -1;
		}
		
		ptr = strchr((ptr + 7), '=');	//Find the input value because sensor id could be double digits
		
		if(!strncmp_P(ptr, PSTR("=Rename"), 7)){
			ds1820_set_tag(tag_len, (packet + 16), sensor);
			return 3;
		}
		
		if(!strncmp_P(ptr, PSTR("=Reset"), 6)){
			ds1820_reset_temp(sensor);
			return 3;
		}
				
	}else
	if(!strncmp_P(packet, PSTR("/FormRelay"), 10)){
		
		int card = 0;
		int relay = 0;
		uint8_t state = 0;
		
		if(!strncmp_P(packet + 10, PSTR("Tag"), 3)){
			//Model string: /FormRelayTag?tag=Outdoor&Relay00=Rename
			
			ptr = strchr((packet + 18), '&');
			tag_len = ptr - (packet + 18);	//Calculate tag length from the start and end pointers
			
			if(sscanf_P(ptr, PSTR("&Relay%1d%1d"), &card, &relay) != 2){	//Length 8
				return -1;
			}	
			relay_set_tag(tag_len, (packet + 18), card, relay);
			return (card + 4);
			
		}else
		if(!strncmp_P(packet + 10, PSTR("Ctrl"), 4)){
			
			if(sscanf_P((packet + 14), PSTR("?Relay%1d%1d"), &card, &relay) != 2){	//Length 8
				return -1;
			}			
			
			if(!strncmp_P(packet + 22, PSTR("=Manual"), 7)){
				relay_set_ctrl(card, relay, RELAY_CTRL_MANUAL);
				return (card + 4);
			}
			
			if(!strncmp_P(packet + 22, PSTR("=Sensor"), 7)){
				relay_set_ctrl(card, relay, RELAY_CTRL_SENSOR);
				return (card + 4);
			}
			
			if(!strncmp_P(packet + 22, PSTR("=Timer"), 6)){
				relay_set_ctrl(card, relay, RELAY_CTRL_TIMER);
				return (card + 4);
			}

		}else
		if(!strncmp_P(packet + 10, PSTR("Switch"), 6)){
			
			sscanf_P((packet + 16), PSTR("?Relay%1d%1d"), &card, &relay);	//Length 8
			
			if(!strncmp_P(packet + 24, PSTR("=Switch"), 7)){
				relay_switch(card, relay, RELAY_OP_TOGGLE);
				return (card + 4);
			}
			
			if(!strncmp_P(packet + 24, PSTR("=On"), 3)){
				relay_switch(card, relay, RELAY_OP_ON);
				return (card + 4);
			}
			
			if(!strncmp_P(packet + 24, PSTR("=Off"), 4)){
				relay_switch(card, relay, RELAY_OP_OFF);
				return (card + 4);
			}
		}else
		if(!strncmp_P(packet + 10, PSTR("Sensor"), 6)){
			
			//Model string: /FormRelaySensor?state=1&sensor=1&value=%2B28.3&relay00=Set
			//Model string: /FormRelaySensor?state=0&sensor=0&value=5.2&relay02=Set
			uint8_t sensor = 0;
			
			if(sscanf_P((packet + 16), PSTR("?state=%d&sensor=%d"), &state, &sensor) != 2){
				return -1;	//Invalid URL -> page not found
			}
			
			ptr = strchr((packet + 32), '&') + 7;	//Find beginning of the value, sensor could be double digits
			
			int val = 0;
			int sign = 1;
			if(*ptr == '%'){	//+ sign is marked as %2B
				ptr += 3;		//Ignore and skip
			}else
			if(*ptr == '-'){	//- sign is marked as -
				sign = -1;		//save as sign and skip
				ptr++;
			}
			
			char c = 0;
			while((c = *ptr++) != '&'){
				
				if(c != '.'){
					val += (c - 48);
				}else{
					if(*ptr != '&'){
						val += (*ptr - 48);
						ptr += 2;	//advance decimal dot and value
					}else{
						ptr++;	//advance only decimal dot
					}
					break;
				}
				
				val *= 10;
			}
			val *= sign;
			
			//sprintf(lcd_buf_l1, "%c%c%c", *ptr, *(ptr+1), *(ptr+2));
			//sprintf(lcd_buf_l2, "C %d,R %d", card, relay);
			//lcd_write_buffer(lcd_buf_l1, NULL);
			
			if(sscanf_P((ptr), PSTR("relay%1d%1d"), &card, &relay) != 2){
				return -1;	//Invalid
			}
			
			//sprintf(lcd_buf_l1, "State %d", state);
			//sprintf(lcd_buf_l2, "C %d,R %d", card, relay);
			//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
			
			relay_set_ctrl(card, relay, RELAY_CTRL_SENSOR | ((state)?RELAY_CTRL_STATE:0) | (sensor<<4));
			relay_set_sensor(card, relay, val);
			
			return (card + 4);
			
		}else
		if(!strncmp_P((packet + 10), PSTR("Timer"), 5)){
			
			//Model string: /FormRelayTimer?state=1&starth=18&startm=30&endh=20&endm=45&relay00=Set
			
			uint8_t starth = 0, startm = 0, endh = 0, endm = 0;
			
			if(sscanf_P((packet + 16), PSTR("state=%d&starth=%d&startm=%d&endh=%d&endm=%d&relay%1d%1d"), &state, &starth, &startm, &endh, &endm, &card, &relay) != 7){
				return -1;	//Invalid
			}
			
			relay_set_ctrl(card, relay, RELAY_CTRL_TIMER | ((state)?RELAY_CTRL_STATE:0));
			relay_set_timer(card, relay, RELAY_TIMER_START, starth, startm);
			relay_set_timer(card, relay, RELAY_TIMER_END, endh, endm);
			return (card + 4);
			
		}
	}
	return -1;	//Page not found
}


/*
 *
 */
int http_parse_login(char* message, uint16_t msg_len, char* usr, char* pwd){
	
	snprintf(lcd_buf_l1, 16, message);
	sprintf(lcd_buf_l2, "Len: %u", msg_len);
	lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
	return 0;
}


const char uri0[] PROGMEM = "/ ";
const char uri1[] PROGMEM = "/NetController.html";
const char uri2[] PROGMEM = "/Time.html";
const char uri3[] PROGMEM = "/Sensor.html";
const char uri4[] PROGMEM = "/Relay1.html";
const char uri5[] PROGMEM = "/Relay2.html";
const char uri6[] PROGMEM = "/Relay3.html";
const char* uris[] PROGMEM = {uri0, uri1, uri2, uri3, uri4, uri5, uri6};


/*
 *
 */
void http_recv(uint8_t* packet, uint16_t pkt_len, struct tcp_socket* socket){
	
	_socket = socket;	//Set current socket
	
	char* reply = (char *)(_network_buf + ETH_HEADER_SIZE + IP_HEADER_SIZE + TCP_HEADER_SIZE);
	uint16_t reply_len = 0;
	//uint16_t hlen = 0;
	static uint8_t post_flag = 0;

	
	//Handle POST from login
	if(post_flag || !strncmp_P((char*)packet, PSTR("POST "), 5)){
		
		/*
		uint16_t hlen = http_header_len(packet, pkt_len);
		//Check if some of the header is missing
		if(hlen == pkt_len){
			post_flag = 1;	//Wait subsequent packet
			tcp_send(_socket, TCP_FLAG_ACK, 0);
			return 0;
		}
		post_flag = 0;
		*/
		//char user[32] = "admin";
		//char pwd[32] = "salasana";
		
		//if(http_parse_login((char*)(packet + hlen), pkt_len - hlen, user, pwd)){
		//	return 0;
		//}
		
		
		
		//sprintf_P(lcd_buf_l1, PSTR("User: %s"), user);
		//sprintf_P(lcd_buf_l2, PSTR("Pwd: %s"), pwd);
		//lcd_write_buffer(lcd_buf_l1, lcd_buf_l2);
		
		reply_len += sprintf_P(reply, PSTR("HTTP/1.1 303 See Other\r\nContent-Type: text/html\r\nLocation: /NetController.html\r\n\r\n"));
		tcp_send(_socket, TCP_FLAG_FIN | TCP_FLAG_ACK, reply_len);
		return;
	}
	
	if(!strncmp_P((char*)packet, PSTR("GET "), 4)){

		int req_page = -1;
		
		//Check if page URI
		for(int i = 0; i < 7; i++){
			char* str = (char*)pgm_read_word(&uris[i]);	//Read pointer to the URI string from the program memory
			if(!strncmp_P((char*)(packet + 4), str, strlen_P(str))){
				req_page = i;
				break;
			}
		}
		if(req_page >= 0){	//URI found -> send OK and page
			reply_len += sprintf_P(reply, PSTR("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nRefresh: 60\r\n\r\n"));
			tcp_send(_socket, TCP_FLAG_ACK, reply_len);				//Send http header first
			if(req_page == 0){
				//sprintf_P(lcd_buf_l1, PSTR("HTTP: send login"));
				//lcd_write_buffer(lcd_buf_l1, NULL);
				http_send_login(reply, 0);		//Send login page
			}else{
				http_send_page(reply, req_page);	//Send web page
			}
			return;
		}
		
		//Check if form URI
		if(!strncmp_P((char*)(packet + 4), PSTR("/Form"), 5)){	//Card1 page
			req_page = http_form_handler((char*)(packet + 4));	//Return the page where the client will be redirected
		}
		if(req_page >= 0){	//URI found -> redirect
			reply_len += sprintf_P(reply, PSTR("HTTP/1.1 303 See Other\r\nContent-Type: text/html\r\nLocation: "));
			reply_len += sprintf_P((reply + reply_len), (char*)pgm_read_word(&uris[req_page]));
			reply_len += sprintf_P((reply + reply_len), PSTR("\r\n\r\n"));
			tcp_send(_socket, TCP_FLAG_FIN | TCP_FLAG_ACK, reply_len);
			return;
		}
		
		//URI not found
		reply_len += sprintf_P(reply, PSTR("HTTP/1.1 404 Not Found\r\n\r\n"));
		tcp_send(_socket, TCP_FLAG_FIN | TCP_FLAG_ACK, reply_len);
		return;
	}
}
