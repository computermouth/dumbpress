
#include <stdio.h>
#include <string.h>

#include "log.h"

#include "p_fble_rot.h"

unit un_fble_rot(short * chunk, FILE *out, short delim, short location){
	
	log_trace("un_fble_rot");
	
	unit ru = { 0 };
	
	for(int i = 0; i < DELLEN; i++){
		if(chunk[i] != delim){
			log_trace("negative negative");
			return ru;
		}
	}
	
	for(int i = 0; i < DELLEN + 2; i++){
		if(chunk[i] == DP_EOB || chunk[i] == DP_EOF){
			log_error("reached EOB || EOF: %x:%x", i, chunk[i]);
			ru.rc = 1;
			return ru;
		}
	}
	
	int len = chunk[DELLEN];
	int val = chunk[DELLEN + 1];
	
	log_trace("fble_rot %02x:%02x", len, val);
	
	// write out new chunk
	if(log_get_level() <= LOG_DEBUG)
		printf("out:");
	
	for(int i = 0; i < len; i++){
		
		if(log_get_level() <= LOG_DEBUG)
			printf("%3x", val);
			
		if (fputc(val, out) != val){
			log_error("failed to write to output file");
			ru.rc = 1;
		}
	}
	
	if(log_get_level() <= LOG_DEBUG)
		printf("\n");
	
	ru.payload_used = len;
	ru.consumed = DELLEN + 2;
	
	log_trace("un_fble_rot_consume -> %d", DELLEN + 2);
	log_trace("un_fble_rot_rc      -> %d", ru.rc);
	
	return ru;
}

unit fble_rot(short buf[BUFLEN], short index){
	log_trace("fble_rot");
	
	unit hit = { 
		.consumed = 0,
		.payload = { 0x00 },
		.payload_used = 2,
	};
	
	int drop = index / 16;
	int zero = (index - (drop * 4)) % 2;
	int rota = (index - (drop * 16) - (zero)) / 2;
	
	log_trace("fble_rot: drop: %02d rota: %02d zero: %02d", drop, rota, zero);
	
	int len = 0;
	int packed_payload_bytes = 0;
	int place_in_byte = 0;
	
	while(len < BUFLEN && buf[len] != DP_EOF){ // start counting fble_rots
		
		unsigned char shifted = rotate_u8_left(rota, buf[len]);
		log_trace("fble_rot: drop: %02u buf[%02d]: %02x", drop, len, shifted);
		
		for(int i = 0; i <= drop; i++){
			
			if(shifted & (zero << i)){
				log_trace("fble_rot: buf[%02d] %02x bit %d & %02x", len, shifted, i, (zero << i ));
			} else {
				log_trace("fble_rot: nope");
				goto thingy;
			}
		}
		
		for(int i = 7; i > drop; i--){
			hit.payload[packed_payload_bytes] |= ((shifted >> i) & 0x01) << (7 - place_in_byte);
			log_trace("fble_rot: hit.payload[%02d] %02x", packed_payload_bytes, hit.payload[packed_payload_bytes]);
			place_in_byte++;
			
			if(place_in_byte == 8){ // move onto next byte
				place_in_byte = 0;
				packed_payload_bytes++;
			}
		}
		
		//~ drop > 0 && buf[len] & (zero << drop) &&
		//~ int rc = add_bits_to_payload( (8 - (drop + 1)), buf[len], &hit.payload );
		
		len++;
		log_trace("len -- %03d", len);
		
	}
	
	thingy:
	
	if (len < packed_payload_bytes + DELLEN) // no fble_rots
		return hit;
	
	if (place_in_byte != 0) // bytes don't align
		return hit;
	
	// move payload, and store length at the beginning
	// waste of cpu, but easier to keep track of
	void *mm_rc = NULL;
	mm_rc = memmove(hit.payload + 1, hit.payload, sizeof(unsigned char) * packed_payload_bytes );
	
	if (mm_rc != hit.payload + 1){
		log_error("couldn't memmove fble_rot payload");
		return hit;
	}
	
	hit.payload[0] = packed_payload_bytes;
	
	hit.consumed = len;
	hit.payload_used = packed_payload_bytes + 1;
	
	log_trace("fble_rot_consume -> %d", len);
	log_trace("fble_rot_length  -> %d", hit.payload_used);
	
	if(log_get_level() <= LOG_TRACE){
		printf("fble_rot_payload -> ");
		printf("%02x %02x ", index, index);
		for(int i = 0; i < hit.payload_used; i++){
			printf("%02x ", hit.payload[i]);
		}
		printf("\n");
	}
	
	log_trace("fble_rot_value   -> %c", 0);
	
	return hit;
}
