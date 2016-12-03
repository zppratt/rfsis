struct pico_ip4 destinationIP;

pico_string_to_ipv4("162.21.12.2", &destinationIP.addr);

pico_arp_request(device, &destinationIP, PICO_ARP_ANNOUNCE);
