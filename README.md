# libom2

## net

### Headers and Header Fields

* Ethernet (set and get)
    * destination address (dest_addr - mac_addr)
    * source address (src_addr - mac_addr)
    * ether type (ether_type - uint16_t)
    
* ARP (get only)
    * hardware type (hardware_type - uint16_t)
    * protocol type (protocol_type - uint16_t)
    * operation (operation - uint16_t)
    * sender hardware address (sender_hardware_addr - mac_addr)
    * sender protocol address (sender_protocol_addr - ip4_addr)
    * target hardware address (target_hardware_addr - mac_addr)
    * target protocol address (target_protocol_addr - ip4_addr) 
    
* IP (set and get)
    * total length (total_len - uint16_t)
    * ip identification (id - uint16_t)
    * time to live (ttl - uint8_t)
    * protocol id (proto - uint8_t)
    * source address (src_addr - ip4_addr)
    * destinatiion address (dest_addr - ip4_addr)
    
* ICMP (get only)
    * type (type - uint8_t)
    * code (code - uint8_t)

* TCP (set and get)

* UDP (set and get)
